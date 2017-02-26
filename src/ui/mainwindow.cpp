#include <QtWidgets>

#include "mainwindow.h"
#include "qaboutdialog.h"
#include "src/gbl/gbl_version.h"
#include "src/gbl/gbl_historymodel.h"
#include "src/gbl/gbl_filemodel.h"
#include "src/ui/historyview.h"
#include "src/ui/fileview.h"
#include "clonedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_qpRepo = NULL;
    m_pHistModel = NULL;
    m_pHistView = NULL;

    init();
}

MainWindow::~MainWindow()
{
    if (m_qpRepo)
    {
        delete m_qpRepo;
    }

    cleanupDocks();
}

void MainWindow::cleanupDocks()
{
    QMapIterator<QString, QDockWidget*> i(m_docks);
    while (i.hasNext()) {
        i.next();
        QDockWidget *pDock = i.value();
        delete pDock;
    }

    m_docks.clear();

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    writeSettings();
}

void MainWindow::about()
{
   /*QMessageBox::about(this, tr("About GitBusyLivin"),
            tr("Hope is a good thing, maybe the best of things, and no good thing ever dies."));*/
   QAboutDialog about(this);
   about.exec();
}

void MainWindow::clone()
{
    CloneDialog cloneDlg(this);
    if (cloneDlg.exec() == QDialog::Accepted)
    {
        QString src = cloneDlg.getSource();
        QString dst = cloneDlg.getDestination();

        if (m_qpRepo->clone_repo(src, dst))
        {
            setupRepoUI(dst);
        }
        else
        {
            QMessageBox::warning(this, tr("Clone Error"), m_qpRepo->get_error_msg());
        }
    }
}

void MainWindow::new_local_repo()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),QString(), QFileDialog::ShowDirsOnly);
    if (!dirName.isEmpty())
    {
        if (!m_qpRepo->init_repo(dirName))
        {
            QMessageBox::warning(this, tr("Creation Error"), m_qpRepo->get_error_msg());
        }
    }
}

void MainWindow::new_network_repo()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),QString(), QFileDialog::ShowDirsOnly);
    if (!dirName.isEmpty())
    {
        if (!m_qpRepo->init_repo(dirName, true))
        {
            QMessageBox::warning(this, tr("Creation Error"), m_qpRepo->get_error_msg());
        }
    }
}

void MainWindow::open()
{
    QString dirName = QFileDialog::getExistingDirectory(this);
    if (!dirName.isEmpty())
    {
        if (m_qpRepo->open_repo(dirName))
        {
            setupRepoUI(dirName);
        }
        else
        {
            QMessageBox::warning(this, tr("Open Error"), m_qpRepo->get_error_msg());
        }
    }
}

void MainWindow::setupRepoUI(QString repoDir)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QFileInfo fi(repoDir);
    QString title(GBL_APP_NAME);
    QTextStream(&title) << " - " << fi.fileName();
    setWindowTitle(title);

    GBL_History_Array *pHistArr;
    m_qpRepo->get_history(&pHistArr);

    m_pHistModel->setModelData(pHistArr);
    m_pHistView->reset();
    QDockWidget *pDock = m_docks["history_files"];
    FileView *pView = (FileView*)pDock->widget();
    pView->reset();
    GBL_FileModel *pMod = (GBL_FileModel*)pView->model();
    pMod->cleanFileArray();


    QApplication::restoreOverrideCursor();

}

void MainWindow::historySelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    QModelIndexList mil = selected.indexes();

    //single row selection
    if (mil.count() > 0)
    {
        QModelIndex mi = mil.at(0);
        int row = mi.row();
        GBL_History_Item *pHistItem = m_pHistModel->getHistoryItemAt(row);
        if (pHistItem)
        {
            QDockWidget *pDock = m_docks["history_files"];
            FileView *pView = (FileView*)pDock->widget();
            pView->reset();
            GBL_FileModel *pMod = (GBL_FileModel*)pView->model();
            pMod->cleanFileArray();
            //m_qpRepo->get_tree_from_commit_oid(pHistItem->hist_oid, pMod);
            m_qpRepo->get_commit_to_parent_diff(pHistItem->hist_oid, pMod);
        }
    }
}

void MainWindow::preferences()
{
    QMessageBox::about(this, tr("D'oh"),
             tr("Under Construction"));
}

void MainWindow::toggleStatusBar()
{
    if (statusBar()->isVisible())
    {
        statusBar()->hide();
    }
    else
    {
       statusBar()->show();
    }
}

void MainWindow::toggleToolBar()
{
    if (m_pToolBar->isVisible())
    {
        m_pToolBar->hide();
    }
    else
    {
        m_pToolBar->show();
    }
}

void MainWindow::init()
{
    readSettings();
    m_qpRepo = new GBL_Repository();
    m_pToolBar = addToolBar(tr(GBL_APP_NAME));
    createActions();
    createDocks();
    setWindowTitle(tr(GBL_APP_NAME));
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *newMenu = fileMenu->addMenu(tr("&New"));
    newMenu->addAction(tr("&Local Repository..."), this, &MainWindow::new_local_repo);
    newMenu->addAction(tr("&Network Repository..."), this, &MainWindow::new_network_repo);
    fileMenu->addAction(tr("&Clone..."), this, &MainWindow::clone);
    fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
 #ifdef Q_OS_WIN
    QString sQuit = tr("&Exit");
 #else
    QString sQuit = tr("&Quit");
 #endif
    QAction *quitAct = fileMenu->addAction(sQuit, this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Preferences..."), this, &MainWindow::preferences);

    m_pViewMenu = menuBar()->addMenu(tr("&View"));
    QAction *tbAct = m_pViewMenu->addAction(tr("&Toolbar"));
    QAction *sbAct = m_pViewMenu->addAction(tr("&Statusbar"));
    tbAct->setCheckable(true);
    tbAct->setChecked(true);
    sbAct->setCheckable(true);
    sbAct->setChecked(true);
    connect(tbAct, &QAction::toggled, this, &MainWindow::toggleToolBar);
    connect(sbAct, &QAction::toggled, this, &MainWindow::toggleStatusBar);
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About GitBusyLivin"), this, &MainWindow::about);
}

void MainWindow::createDocks()
{
    m_pHistModel = new GBL_HistoryModel(NULL);
    m_pHistView = new HistoryView(this);
    m_pHistView->setModel(m_pHistModel);
    m_pHistView->verticalHeader()->hide();
    //m_pHistView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_pHistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pHistView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pHistView->setShowGrid(false);
    m_pHistView->setAlternatingRowColors(true);
    connect(m_pHistView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::historySelectionChanged);
    setCentralWidget(m_pHistView);
    QDockWidget *pDock = new QDockWidget(tr("History - Files"), this);
    FileView *pView = new FileView(pDock);
    pDock->setWidget(pView);
    pView->setModel(new GBL_FileModel(pView));
    m_docks["history_files"] = pDock;
    addDockWidget(Qt::BottomDockWidgetArea, pDock);
    m_pViewMenu->addAction(pDock->toggleViewAction());
    pDock = new QDockWidget(tr("Differences"), this);
    QTextEdit *pText = new QTextEdit(pDock);
    pDock->setWidget(pText);
    m_docks["file_diff"] = pDock;
    addDockWidget(Qt::BottomDockWidgetArea, pDock);
    m_pViewMenu->addAction(pDock->toggleViewAction());

}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

