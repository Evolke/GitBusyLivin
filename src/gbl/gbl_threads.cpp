#include "gbl_threads.h"

#include "gbl_string.h"

#include <QVector>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QTextDocument>

/**
 * @brief GBL_Thread::GBL_Thread
 * @param sRepoPath
 * @param parent
 */
GBL_Thread::GBL_Thread(GBL_String sRepoPath, QObject *parent) : QThread(parent)
{
    m_bRestart = false;
    m_bAbort = false;

    m_pRepo = new GBL_Repository();
    if (!sRepoPath.isEmpty())
    {
        if (m_pRepo->open_repo(sRepoPath))
        {
            m_sRepoPath = sRepoPath;
        }
    }
}

/**
 * @brief GBL_Thread::~GBL_Thread
 */
GBL_Thread::~GBL_Thread()
{
    m_mutex.lock();
    m_bAbort = true;
    m_condition.wakeOne();
    m_mutex.unlock();

    wait();

    delete m_pRepo;

}

/**
 * @brief GBL_Thread::stop_thread
 */
void GBL_Thread::stop_thread()
{
    if (isRunning())
        wait();
    quit();
}

/**
 * @brief GBL_Thread::start_thread
 */
void GBL_Thread::start_thread()
{
    QMutexLocker locker(&m_mutex);

    if (!isRunning()) {
        start(LowPriority);
    } else {
        m_bRestart = true;
        m_condition.wakeOne();
    }

}

/**
 * @brief GBL_CloneThread::GBL_CloneThread
 * @param parent
 */
GBL_CloneThread::GBL_CloneThread(QObject *parent) : GBL_Thread(GBL_String(""),parent)
{

}

/**
 * @brief GBL_CloneThread::clone
 */
void GBL_CloneThread::clone(GBL_String sSrc, GBL_String sDst)
{
    stop_thread();
    m_mutex.lock();
    m_sSrc = sSrc;
    m_sDst = sDst;
    m_mutex.unlock();
    start_thread();
}

/**
 * @brief GBL_CloneThread::run
 */
void GBL_CloneThread::run()
{
    bool bRet = m_pRepo->clone_repo(m_sSrc,m_sDst);
    m_sError = !bRet ? m_pRepo->get_error_msg() : "";
    emit cloneFinished(&m_sError, &m_sDst);

    quit();
}

/**
 * @brief GBL_FetchThread::GBL_FetchThread
 * @param parent
 */
GBL_FetchThread::GBL_FetchThread(QObject *parent) : GBL_Thread(GBL_String(""),parent)
{

}

void GBL_FetchThread::fetch(GBL_String sRepoPath)
{
    stop_thread();
    if (!sRepoPath.isEmpty())
    {
        if (m_pRepo->open_repo(sRepoPath))
        {
            m_mutex.lock();
            m_sRepoPath = sRepoPath;
            m_mutex.unlock();
            start_thread();
        }
    }
    else
    {
        start_thread();
    }
}


/**
 * @brief GBL_FetchThread::run
 */
void GBL_FetchThread::run()
{
   bool bRet = m_pRepo->fetch_remote();
   m_sError = !bRet ? m_pRepo->get_error_msg() : "";
   emit fetchFinished(&m_sError);

   quit();
}

GBL_PullThread::GBL_PullThread(QObject *parent) : GBL_Thread(GBL_String(""),parent)
{
}

void GBL_PullThread::pull(GBL_String sRepoPath, GBL_String sBranch)
{
    stop_thread();
    if (!sRepoPath.isEmpty())
    {
        if (m_pRepo->open_repo(sRepoPath))
        {
            m_mutex.lock();
            m_sRepoPath = sRepoPath;
            m_sBranch = sBranch;
            m_mutex.unlock();
            start_thread();
        }
    }
    else
    {
        start_thread();
    }
}

void GBL_PullThread::run()
{
    bool bRet = m_pRepo->pull_remote("origin", m_sBranch);
    m_sError = !bRet ? m_pRepo->get_error_msg() : "";
    emit pullFinished(&m_sError);

    quit();
}

/**
 * @brief GBL_HistoryThread::GBL_HistoryThread
 * @param sRepoPath
 * @param parent
 */
GBL_HistoryThread::GBL_HistoryThread(GBL_String sRepoPath, QObject *parent) : GBL_Thread(sRepoPath, parent)
{
    m_pHistArr = new GBL_History_Array;
}

GBL_HistoryThread::~GBL_HistoryThread()
{
    cleanup();
}

void GBL_HistoryThread::cleanup()
{
    if (m_pHistArr)
    {
        for (int i = 0; i < m_pHistArr->size(); i++)
        {
            GBL_History_Item *pHI = m_pHistArr->at(i);
            delete pHI;
        }

        m_pHistArr->clear();
    }
}

void GBL_HistoryThread::get_history()
{
    stop_thread();
    start_thread();
}

void GBL_HistoryThread::run()
{
    m_mutex.lock();
    cleanup();
    m_mutex.unlock();

    bool bRet = m_pRepo->get_history(m_pHistArr);
    m_sError = !bRet ? m_pRepo->get_error_msg() : "";
    emit historyUpdated(&m_sError, m_pHistArr);

    quit();
}

GBL_ReferencesThread::GBL_ReferencesThread(QObject *parent) : GBL_Thread(GBL_String(""),parent)
{

}

GBL_ReferencesThread::~GBL_ReferencesThread()
{
    cleanup();
}

void GBL_ReferencesThread::cleanup()
{
}

/**
 * @brief GBL_ReferencesThread::get_references
 * @param sRepoPath
 */
void GBL_ReferencesThread::get_references(GBL_String sRepoPath)
{
    stop_thread();
    if (!sRepoPath.isEmpty())
    {
        if (m_pRepo->open_repo(sRepoPath))
        {
            start_thread();
        }
    }
    else
    {
        start_thread();
    }
}

/**
 * @brief GBL_ReferencesThread::run
 */
void GBL_ReferencesThread::run()
{
    m_mutex.lock();
    cleanup();
    m_mutex.unlock();

    bool bRet = m_pRepo->fill_references();
    m_sError = !bRet ? m_pRepo->get_error_msg() : "";
    emit refsUpdated(&m_sError, m_pRepo->get_references());

    quit();

}

/**
 * @brief GBL_StatusThread::GBL_StatusThread
 * @param parent
 */
GBL_StatusThread::GBL_StatusThread(QObject *parent) : GBL_Thread(GBL_String(""),parent)
{
    m_pStagedArr = new GBL_File_Array();
    m_pUnstagedArr = new GBL_File_Array();
}

/**
 * @brief GBL_StatusThread::~GBL_StatusThread
 */
GBL_StatusThread::~GBL_StatusThread()
{
    cleanup();
}

/**
 * @brief GBL_StatusThread::cleanup
 */
void GBL_StatusThread::cleanup()
{
    GBL_File_Item *pFI;
    if (m_pStagedArr)
    {
        for (int i = 0; i < m_pStagedArr->size(); i++)
        {
            pFI = m_pStagedArr->at(i);
            delete pFI;
        }

        m_pStagedArr->clear();
    }

    if (m_pUnstagedArr)
    {
        for (int i = 0; i < m_pUnstagedArr->size(); i++)
        {
            pFI = m_pUnstagedArr->at(i);
            delete pFI;
        }

        m_pUnstagedArr->clear();
    }

}

/**
 * @brief GBL_StatusThread::status
 * @param sRepoPath
 */
void GBL_StatusThread::status(GBL_String sRepoPath)
{
    stop_thread();
    if (!sRepoPath.isEmpty())
    {
        if (m_pRepo->open_repo(sRepoPath))
        {
            start_thread();
        }
    }
    else
    {
        start_thread();
    }
}

/**
 * @brief GBL_StatusThread::run
 */
void GBL_StatusThread::run()
{
    m_mutex.lock();
    cleanup();
    m_mutex.unlock();

    bool bRet = m_pRepo->get_repo_status(m_pStagedArr, m_pUnstagedArr);
    m_sError = !bRet ? m_pRepo->get_error_msg() : "";

    emit statusUpdated(&m_sError, m_pStagedArr, m_pUnstagedArr);
    quit();
}

/**
 * @brief GBL_ScanThread::GBL_ScanThread
 * @param parent
 */
GBL_ScanThread::GBL_ScanThread(QObject *parent) : GBL_Thread(GBL_String(""),parent)
{

}

/**
 * @brief GBL_ScanThread::~GBL_ScanThread
 */
GBL_ScanThread::~GBL_ScanThread()
{

}

void GBL_ScanThread::scan(GBL_String sRootPath, GBL_String sSearch)
{
    stop_thread();
    m_mutex.lock();
    m_sRootPath = sRootPath;
    m_sSearch = sSearch;
    m_mutex.unlock();
    start_thread();

}

void GBL_ScanThread::run()
{
    QDir dir(m_sRootPath);
    QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    GBL_String sRepoPath;
    int dirSize = dirs.size();
    QString sOutput;
    sOutput = "<h1>Search: "+m_sSearch+"</h1>";
    for (int i = 0; i < dirSize; i++)
    {
        if (isInterruptionRequested()) break;

        sRepoPath = m_sRootPath;
        sRepoPath += "/" + dirs[i];

        m_mutex.lock();
        sOutput += "<h2>"+sRepoPath+"</h2>";
        m_mutex.unlock();
        emit scanUpdated(i,dirSize, &m_sOutput);
        if (m_pRepo->open_repo(sRepoPath))
        {
            GBL_File_Array fa;
            m_pRepo->get_tree_from_commit_oid("", &fa);
            sOutput += "<table cellspacing='2'>";
            for (int j = 0; j < fa.size(); j++)
            {
                if (isInterruptionRequested()) break;

                GBL_File_Item *pFI = fa[j];
                QString content, filePath, blurb;
                if (m_pRepo->get_blob_content(GBL_String(pFI->file_oid), content))
                {
                    int pos = content.indexOf(m_sSearch);
                    if (pos > -1 )
                    {
                        filePath = pFI->sub_dir;
                        filePath += pFI->file_name;
                        int start = pos >= 50 ? pos - 50 : 0;
                        int blurbSize = m_sSearch.size() + 100;
                        if ((pos + blurbSize) > content.size()) blurbSize = content.size() - start;
                        blurb = content.mid(start, blurbSize);
                        blurb.remove(QRegExp("<[^>]*>"));

                        sOutput += "<tr><td><b>";
                        sOutput += filePath + "</b></td><td>";
                        sOutput += "<td>" + blurb;
                        sOutput += "</td></tr>";
                    }
                }
            }
            sOutput += "</table>";
            emit scanUpdated(i,dirSize, &m_sOutput);
        }
    }
    m_mutex.lock();
    m_sOutput = sOutput;
    m_mutex.unlock();
    emit scanUpdated(dirSize,dirSize, &m_sOutput);
    emit scanFinished();
}
