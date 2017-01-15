#include "gbl_repository.h"
#include <QTextStream>
#include <QDebug>

GBL_Repository::GBL_Repository(QObject *parent) : QObject(parent)
{
    git_libgit2_init();
    m_pRepo = Q_NULLPTR;
    m_pHist_Arr = Q_NULLPTR;
    m_iErrorCode = 0;
}

GBL_Repository::~GBL_Repository()
{
    cleanup();
    git_libgit2_shutdown();
}

void GBL_Repository::cleanup()
{
    if (m_pHist_Arr)
    {
        for (int i = 0; i < m_pHist_Arr->size(); i++)
        {
            GBL_history_item *pHI = m_pHist_Arr->at(i);
            delete pHI;
        }

        m_pHist_Arr->clear();
        delete m_pHist_Arr;
        m_pHist_Arr = Q_NULLPTR;
    }


    if (m_pRepo)
    {
        git_repository_free(m_pRepo);
        m_pRepo = Q_NULLPTR;
    }

}

QString GBL_Repository::get_error_msg()
{
    QString error_msg;

    const git_error *e = giterr_last();
    if (e)
    {
        QTextStream(&error_msg) << "Error " << e->klass << ", " << e->message;
    }

    return error_msg;
}

bool GBL_Repository::init(QString path, bool bare)
{
    cleanup();
    const QByteArray l8b = path.toLocal8Bit();
    const char* spath = l8b.constData();
    m_iErrorCode = git_repository_init(&m_pRepo, spath, bare);
    return m_iErrorCode >= 0;
}

bool GBL_Repository::open(QString path)
{
    cleanup();
    const char* spath = path.toUtf8().constData();
    m_iErrorCode = git_repository_open(&m_pRepo, spath);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::get_history(GBL_History_Array **pHist_Arr)
{
    git_revwalk *walker;
    m_iErrorCode = git_revwalk_new(&walker, m_pRepo);
    if (m_iErrorCode >= 0)
    {
        m_iErrorCode = git_revwalk_push_head(walker);
        if (m_iErrorCode >= 0)
        {
            git_oid oid;
            while (!git_revwalk_next(&oid, walker))
            {
                git_commit * commit = nullptr;
                git_commit_lookup(&commit, m_pRepo, &oid);

                QString soid(git_oid_tostr_s(&oid));
                GBL_history_item *pHistItem = new GBL_history_item;
                pHistItem->hist_summary = QString(git_commit_summary(commit));
                const git_signature *pGit_Sig = git_commit_author(commit);
                QString author;
                QTextStream(&author) << pGit_Sig->name << " <" << pGit_Sig->email << ">";
                pHistItem->hist_author = author;
                pHistItem->hist_datetime = QDateTime::fromTime_t(pGit_Sig->when.time);
                m_pHist_Arr->append(pHistItem);

                //qDebug() << pHistItem->hist_summary << pHistItem->hist_author << pHistItem->hist_datetime;

                // free the commit
                git_commit_free(commit);

            }

            *pHist_Arr = m_pHist_Arr;
            git_revwalk_free(walker);
        }
    }
    return false;
}


