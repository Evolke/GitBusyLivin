#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QMap>


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
class HistoryView;
class GBL_HistoryModel;
class GBL_Repository;
class QDockWidget;
class QItemSelection;
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
    void preferences();
    void toggleToolBar();
    void toggleStatusBar();
    void historySelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void closeEvent(QCloseEvent *event);

private:
    void init();
    void cleanupDocks();
    void createActions();
    void readSettings();
    void writeSettings();
    void setupRepoUI(QString repoDir);

    GBL_Repository *m_qpRepo;
    HistoryView *m_pHistView;
    QPointer<GBL_HistoryModel> m_pHistModel;
    QMap<QString, QDockWidget*> m_docks;
    QMenu *m_pViewMenu;
    QToolBar *m_pToolBar;
};

#endif // MAINWINDOW_H
