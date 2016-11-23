#include <QtWidgets>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    init();
}

MainWindow::~MainWindow()
{

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

void MainWindow::open()
{
    QString dirName = QFileDialog::getExistingDirectory(this);
    if (!dirName.isEmpty())
    {

    }
}

void MainWindow::init()
{
    createActions();
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *cloneAct = fileMenu->addAction(tr("&Clone"), this, &MainWindow::clone);

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);

}
