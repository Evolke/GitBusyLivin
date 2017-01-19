#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
class QTableView;
class GBL_HistoryModel;
class GBL_Repository;
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

    GBL_Repository *m_qpRepo;
    QPointer<QTableView> m_pHistView;
    QPointer<GBL_HistoryModel> m_pHistModel;
};

#endif // MAINWINDOW_H
