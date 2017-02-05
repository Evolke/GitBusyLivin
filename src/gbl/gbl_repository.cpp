#include "gbl_repository.h"
#include <QTextStream>
#include <QDebug>
#include <QByteArray>
#include "gbl_filemodel.h"

static QByteArray g_temp_ba;

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
    cleanup_history();

    if (m_pRepo)
    {
        git_repository_free(m_pRepo);
        m_pRepo = Q_NULLPTR;
    }

}

void GBL_Repository::cleanup_history()
{
    if (m_pHist_Arr)
    {
        for (int i = 0; i < m_pHist_Arr->size(); i++)
        {
            GBL_History_Item *pHI = m_pHist_Arr->at(i);
            delete pHI;
        }

        m_pHist_Arr->clear();
        delete m_pHist_Arr;
        m_pHist_Arr = Q_NULLPTR;
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

const char* GBL_Repository::qstring2cc(QString *pQStr)
{
    g_temp_ba = pQStr->toLocal8Bit();
    const char* str = g_temp_ba.constData();

    return str;
}

bool GBL_Repository::init(QString path, bool bare)
{
    cleanup();
    /*const QByteArray l8b = path.toLocal8Bit();
    const char* spath = l8b.constData();*/
    m_iErrorCode = git_repository_init(&m_pRepo, qstring2cc(&path), bare);
    return m_iErrorCode >= 0;
}

bool GBL_Repository::open(QString path)
{
    cleanup();
    /*const QByteArray l8b = path.toLocal8Bit();
    const char* spath = l8b.constData();*/
    m_iErrorCode = git_repository_open(&m_pRepo, qstring2cc(&path));
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
            cleanup_history();
            m_pHist_Arr = new GBL_History_Array;
            git_oid oid;
            while (!git_revwalk_next(&oid, walker))
            {
                git_commit * commit = nullptr;
                git_commit_lookup(&commit, m_pRepo, &oid);

                QString soid(git_oid_tostr_s(&oid));
                GBL_History_Item *pHistItem = new GBL_History_Item;
                pHistItem->hist_oid = soid;
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

    return m_iErrorCode >= 0;
}

bool GBL_Repository::get_tree_from_commit_oid(QString oid_str, GBL_FileModel *pFileMod)
{

   git_oid oid;
   const char* str = qstring2cc(&oid_str);
   m_iErrorCode = git_oid_fromstr(&oid, str);
   if (m_iErrorCode >= 0)
   {
        git_tree *pTree = NULL;
        git_commit *pCommit = NULL;
        m_iErrorCode = git_commit_lookup(&pCommit, m_pRepo, &oid);
        if (m_iErrorCode >= 0)
        {
            const git_oid *pTroid;
            pTroid = git_commit_tree_id(pCommit);
            git_commit_free(pCommit);
            if (m_iErrorCode >= 0)
            {
                tree_walk(pTroid, pFileMod);
            }
        }
   }

   return m_iErrorCode >= 0;
}

void GBL_Repository::tree_walk(const git_oid *pTroid, GBL_FileModel *pFileMod)
{
    git_tree *pTree = NULL;
    m_iErrorCode = git_tree_lookup(&pTree, m_pRepo, pTroid);
    if (m_iErrorCode >= 0)
    {
        m_iErrorCode = git_tree_walk(pTree, GIT_TREEWALK_POST, (git_treewalk_cb)tree_walk_callback, pFileMod);
        git_tree_free(pTree);
    }
}

int GBL_Repository::tree_walk_callback(const char *root, const git_tree_entry *entry, void *payload)
{
    //qDebug() << "root:" << root;
    //qDebug() << "entry.filename:" << git_tree_entry_name(entry);
    git_otype type = git_tree_entry_type(entry);
    //qDebug() << "type:" << type;

    GBL_FileModel *pFileMod = (GBL_FileModel*)payload;

    if (type == GIT_OBJ_BLOB)
    {
        GBL_File_Item *pFItem = new GBL_File_Item;
        pFItem->file_name = git_tree_entry_name(entry);
        pFItem->sub_dir = QString(root);
        const git_oid *pTroid = git_tree_entry_id(entry);
        pFItem->file_oid = QString(git_oid_tostr_s(pTroid));
        pFileMod->addFileItem(pFItem);
    }
    /*if (type == GIT_OBJ_TREE)
    {
        const git_oid *pTroid = git_tree_entry_id(entry);
        pRepo->tree_walk(pTroid);
    }*/

    return 1;
}
