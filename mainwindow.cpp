#include <QtWidgets>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    repo = NULL;
    init();
}

MainWindow::~MainWindow()
{
    if (repo)
    {
        delete repo;
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
    QString dirName = QFileDialog::getExistingDirectory(this);
    if (!dirName.isEmpty())
    {
        if (repo == NULL)
        {
            repo = new GBL_Repository();
        }

        repo->init(dirName);
    }
}

void MainWindow::new_network_repo()
{
    QString dirName = QFileDialog::getExistingDirectory(this);
    if (!dirName.isEmpty())
    {
        if (repo == NULL)
        {
            repo = new GBL_Repository();
        }

        repo->init(dirName, true);
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

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);

}
