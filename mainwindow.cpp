#include <QtWidgets>

#include "mainwindow.h"

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

void MainWindow::about()
{
   QMessageBox::about(this, tr("About GitBusyLivin"),
            tr("Hope is a good thing, maybe the best of things, and no good thing ever dies."));
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
        QMessageBox::information(this, tr("info"), dirName);
    }
}

void MainWindow::init()
{
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
    QAction *quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);

}
