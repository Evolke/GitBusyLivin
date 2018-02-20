#ifndef GBL_THREADS_H
#define GBL_THREADS_H

#include "gbl_string.h"
#include "gbl_repository.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

/**
 * @brief The GBL_Thread class
 */
class GBL_Thread : public QThread
{
    Q_OBJECT
public:
    GBL_Thread(GBL_String sRepoPath, QObject *parent = Q_NULLPTR);
    ~GBL_Thread();

protected:
    void start_thread();
    void stop_thread();

    QMutex m_mutex;
    QWaitCondition m_condition;

    bool m_bRestart;
    bool m_bAbort;
    GBL_Repository *m_pRepo;
    GBL_String m_sError;
    GBL_String m_sRepoPath;
};

/**
 * @brief The GBL_CloneThread class
 */
class GBL_CloneThread : public GBL_Thread
{
    Q_OBJECT
public:
    GBL_CloneThread(QObject *parent = Q_NULLPTR);

    void clone(GBL_String sSrc, GBL_String sDst);

signals:
    void cloneFinished(GBL_String*, GBL_String*);
protected:
    void run() override;

private:
    GBL_String m_sSrc, m_sDst;
};

/**
 * @brief The GBL_FetchThread class
 */
class GBL_FetchThread : public GBL_Thread
{
    Q_OBJECT
public:
    GBL_FetchThread(QObject *parent = Q_NULLPTR);

    void fetch(GBL_String sRepoPath);

signals:
    void fetchFinished(GBL_String*);

protected:
    void run() override;
};

class GBL_PullThread : public GBL_Thread
{
    Q_OBJECT
public:
    GBL_PullThread(QObject *parent = Q_NULLPTR);
    void pull(GBL_String sRepoPath, GBL_String sBranch);

signals:
    void pullFinished(GBL_String*);

protected:
    void run() override;

private:
    GBL_String m_sBranch;
};

/**
 * @brief The GBL_HistoryThread class
 */
class GBL_HistoryThread : public GBL_Thread
{
    Q_OBJECT
public:
    GBL_HistoryThread(GBL_String sRepoPath, QObject *parent = Q_NULLPTR);
    ~GBL_HistoryThread();

    void get_history();

signals:
    void historyUpdated(GBL_String*, GBL_History_Array *pHistArr);


protected:
    void run() override;
    void cleanup();

    GBL_History_Array *m_pHistArr;
};

/**
 * @brief The GBL_ReferencesThread class
 */
class GBL_ReferencesThread : public GBL_Thread
{
    Q_OBJECT
public:
    GBL_ReferencesThread(QObject *parent = Q_NULLPTR);
    ~GBL_ReferencesThread();

    void get_references(GBL_String sRepoPath);

signals:
    void refsUpdated(GBL_String*, GBL_RefItem*);

protected:
    void run() override;
    void cleanup();

};

/**
 * @brief The GBL_StatusThread class
 */
class GBL_StatusThread : public GBL_Thread
{
    Q_OBJECT
public:
    GBL_StatusThread(QObject *parent = Q_NULLPTR);
    ~GBL_StatusThread();

    void status(GBL_String sRepoPath);

signals:
    void statusUpdated(GBL_String*, GBL_File_Array*,GBL_File_Array*);
protected:
    void run() override;
    void cleanup();

    GBL_File_Array *m_pStagedArr, *m_pUnstagedArr;
};

/**
 * @brief The GBL_ScanThread class
 */
class GBL_ScanThread : public GBL_Thread
{
    Q_OBJECT
public:
    GBL_ScanThread(QObject *parent = Q_NULLPTR);
    ~GBL_ScanThread();

    void scan(GBL_String sRootPath, GBL_String sSearch);

signals:
    void scanUpdated(int prog_value, int prog_max, GBL_String *psOutput);
    void scanFinished();

protected:
    void run() override;

    GBL_String m_sRootPath;
    GBL_String m_sSearch;
    GBL_String m_sOutput;
};

#endif // GBL_THREADS_H
