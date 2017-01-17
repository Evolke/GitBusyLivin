#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include "src/gbl/gbl_repository.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void about();
    void clone();
    void open();
    void new_local_repo();
    void new_network_repo();
    void closeEvent(QCloseEvent *event);

private:
    void init();
    void createActions();
    void readSettings();
    void writeSettings();

    QPointer<GBL_Repository> m_qpRepo;
};

#endif // MAINWINDOW_H
