#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QMap>

#include "src/gbl/gbl_repository.h"

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
class QNetworkAccessManager;
class QNetworkDiskCache;
class QNetworkReply;
class UrlPixmap;
class QAction;
struct GBL_Line_Item;
class FileView;
class ToolbarCombo;
class BadgeToolButton;
class QMdiArea;
class MdiChild;
class QMdiSubWindow;
class GBL_Thread;
class StatusProgressBar;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    QNetworkAccessManager* getNetworkAccessManager() { return m_pNetAM; }
    QNetworkDiskCache* getNetworkCache() { return m_pNetCache; }

    void addToDiffView(GBL_Line_Item *pLineItem);
    void setTheme(const QString &theme);
    QString getTheme() { return m_sTheme; }
    QString getSelectedCode() { return m_sSelectedCode; }

    GBL_Repository* getRepo() { return m_qpRepo; }
    QToolBar* getToolBar() { return m_pToolBar; }
    static MainWindow* getInstance() { return m_pSingleInst; }
    static void setInstance(MainWindow* pInst) { m_pSingleInst = pInst; }

    void addAvatar(QString &sEmail);
    void startAvatarDownload();
    void getAvatarFromUrl(QString sUrl, QString sEmail);
    QPixmap* getAvatar(QString sEmail, bool bSmall=false);

    MdiChild* currentMdiChild();
    GBL_Repository* getCurrentRepository();

public slots:
    void stageAll();
    void stageSelected();
    void unstageAll();
    void unstageSelected();
    void commit();
    void avatarDownloaded(QNetworkReply* pReply);
    void historySelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void commitTabChanged(int tabID);
    void applicationStateChanged(Qt::ApplicationState state);
    void statusUpdated(GBL_String *psError, GBL_File_Array *pStagedArr, GBL_File_Array *pUnstagedArr);
    void refsUpdated(GBL_String *psError, GBL_RefItem *pRefItem);
    void fetchFinished(GBL_String *psError);
    void pullFinished(GBL_String *psError);
    void cloneFinished(GBL_String* psError, GBL_String* psDst);

private slots:
    void about();
    void clone();
    void open();
    void new_local_repo();
    void new_network_repo();
    void preferences();
    void toggleToolBar();
    void toggleStatusBar();
    void pushAction();
    void pullAction();
    void fetchAction();
    void sslVersion();
    void libgit2Version();
    void progressTest();
    void historyFileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void workingFileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void stagedFileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void activateChild(QMdiSubWindow *window);
    QMdiSubWindow* findMdiChild(const QString &fileName) const;
    void closeEvent(QCloseEvent *event);
    void updateRecentRepoActions();
    void openRecentRepo();
    void cleaningRepo();
    void timerEvent(QTimerEvent *event);
    void refresh();
    void updateCommitFiles();
    void scanAction();

private:
    enum { MaxRecentRepos = 10 };

    void init();
    void cleanupDocks();
    void cleanupAvatars();
    void createActions();
    void createDocks();
    void resetDocks(bool bRepaint = false);
    void createHistoryTable();
    void readSettings();
    void writeSettings();
    void setupRepoUI(QString repoDir);
    static bool hasRecentRepos();
    void prependToRecentRepos(const QString &dirName);
    void setRecentReposVisible(bool visible);
    void updateStatus();
    void updatePushPull();
    bool openRepoTab(QString &path);
    void updateBranchCombo();
    void updateReferences();

    GBL_Repository *m_qpRepo;
    QMap<QString, QDockWidget*> m_docks;
    QMap<QString, FileView*> m_fileviews;
    QMap<QString, GBL_Thread*> m_threads;
    QMenu *m_pViewMenu, *m_pRepoMenu;
    QToolBar *m_pToolBar;
    StatusProgressBar *m_pStatProg;
    ToolbarCombo *m_pBranchCombo;
    QNetworkAccessManager *m_pNetAM;
    QNetworkDiskCache *m_pNetCache;
    QString m_sTheme;

    QAction *m_pRecentRepoActs[MaxRecentRepos];
    QAction *m_pRecentRepoSeparator;
    QAction *m_pRecentRepoSubMenuAct;
    QMap<QString, QAction*> m_actionMap;
    BadgeToolButton *m_pPullBtn, *m_pPushBtn;

    static MainWindow *m_pSingleInst;
    int m_nMainTimer;
    QString m_sSelectedCode;

    QMdiArea *m_pMdiArea;

    QMap<QString, UrlPixmap*> m_avatarMap;
    QMap<QString, QString> m_gravMap;
    QList<QString> m_emailList;

    int m_nCommitTabID;
    MdiChild *m_pCurrentChild;
};

#endif // MAINWINDOW_H
