#include <QtWidgets>

#include "mainwindow.h"
#include "qaboutdialog.h"
#include "gbl_historymodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_qpRepo = NULL;
    init();
}

MainWindow::~MainWindow()
{
    if (m_qpRepo)
    {
        delete m_qpRepo;
    }

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
    QMessageBox::about(this, tr("About GitBusyLivin"),
             tr("Hope is a good thing, maybe the best of things, and no good thing ever dies."));
}

void MainWindow::new_local_repo()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),QString(), QFileDialog::ShowDirsOnly);
    if (!dirName.isEmpty())
    {
        if (!m_qpRepo->init(dirName))
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
        if (!m_qpRepo->init(dirName, true))
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
        if (m_qpRepo->open(dirName))
        {
            GBL_History_Array *pHistArr;
            m_qpRepo->get_history(&pHistArr);

            GBL_HistoryModel *pHm = new GBL_HistoryModel(pHistArr);

            QTableView *pTv = new QTableView(this);
            pTv->setModel(pHm);
            pTv->verticalHeader()->hide();
            pTv->setSelectionBehavior(QAbstractItemView::SelectRows);
            setCentralWidget(pTv);
        }
        else
        {
            QMessageBox::warning(this, tr("Open Error"), m_qpRepo->get_error_msg());
        }
    }
}

void MainWindow::init()
{
    readSettings();
    m_qpRepo = new GBL_Repository();
    createActions();
    setWindowTitle(tr("GitBusyLivin"));
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *newMenu = fileMenu->addMenu(tr("&New"));
    newMenu->addAction(tr("&Local Repository..."), this, &MainWindow::new_local_repo);
    newMenu->addAction(tr("&Network Repository..."), this, &MainWindow::new_network_repo);
    fileMenu->addAction(tr("&Clone"), this, &MainWindow::clone);
    fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
 #ifdef Q_OS_WIN
    QString sQuit = tr("&Exit");
 #else
    QString sQuit = tr("&Quit");
 #endif
    QAction *quitAct = fileMenu->addAction(sQuit, this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About GitBusyLivin"), this, &MainWindow::about);

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

