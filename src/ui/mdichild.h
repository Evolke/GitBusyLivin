#ifndef MDICHILD_H
#define MDICHILD_H

#include <QFrame>
#include "src/gbl/gbl_repository.h"

QT_BEGIN_NAMESPACE
class HistoryView;
class GBL_HistoryModel;
class GBL_Repository;
class GBL_Thread;
QT_END_NAMESPACE

class MdiChild : public QFrame
{
    Q_OBJECT
public:
    explicit MdiChild(QWidget *parent = nullptr);
    ~MdiChild();

    bool init(QString sRepoPath);

    HistoryView* getHistoryView() { return m_pHistView; }
    GBL_HistoryModel* getHistoryModel() { return m_pHistModel; }
    GBL_Repository* getRepository() { return m_qpRepo; }
    void updateHistory();
    void updateStatus();
    void updateReferences();
    void fetch();
    void pull(GBL_String sBranch);

    QString currentPath() { return m_sRepoPath; }

signals:

public slots:
    void historyUpdated(GBL_String *psError, GBL_History_Array *pHistArr);
    void statusUpdated(GBL_String *psError, GBL_File_Array *pStagedArr, GBL_File_Array *pUnstagedArr);
    void refsUpdated(GBL_String *psError, GBL_RefItem *pRefItem);
    void fetchFinished(GBL_String *psError);
    void pullFinished(GBL_String *psError);

private slots:
    virtual void resizeEvent(QResizeEvent *event);

private:
    void createHistoryTable();

    GBL_Repository *m_qpRepo;
    QString m_sRepoPath;
    HistoryView *m_pHistView;
    GBL_HistoryModel *m_pHistModel;
    QMap<QString, GBL_Thread*> m_threads;
    GBL_RefItem *m_pRefRoot;
    MainWindow *m_pMainWnd;
};

#endif // MDICHILD_H
