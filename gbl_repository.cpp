#include "gbl_repository.h"
#include <QTextStream>
#include <QDebug>

GBL_Repository::GBL_Repository(QObject *parent) : QObject(parent)
{
    git_libgit2_init();
    m_pRepo = NULL;
    m_iErrorCode = 0;
}

GBL_Repository::~GBL_Repository()
{
    cleanup();
    git_libgit2_shutdown();
}

void GBL_Repository::cleanup()
{
    QMapIterator<QString, GBL_history_item*> i(m_hist_map);
    while (i.hasNext())
    {
        i.next();
        GBL_history_item *pHI = i.value();
        delete pHI;
    }

    m_hist_map.clear();

    if (m_pRepo)
    {
        git_repository_free(m_pRepo);
        m_pRepo = NULL;
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

bool GBL_Repository::get_history(GBL_history_map &hist_map)
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
                m_hist_map[soid] = pHistItem;

                qDebug() << pHistItem->hist_summary << pHistItem->hist_author << pHistItem->hist_datetime;

                // free the commit
                git_commit_free(commit);

            }

            hist_map = m_hist_map;
            git_revwalk_free(walker);
        }
    }
    return false;
}


