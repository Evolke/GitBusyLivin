#include "gbl_repository.h"
#include <QTextStream>
#include <QDebug>
#include <QByteArray>
#include <QFileInfo>

#include "gbl_filemodel.h"
#include "src/ui/mainwindow.h"

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
    g_temp_ba = pQStr->toUtf8();
    const char* str = g_temp_ba.constData();

    return str;
}

bool GBL_Repository::init_repo(QString path, bool bare)
{
    cleanup();
    /*const QByteArray l8b = path.toUtf8();
    const char* spath = l8b.constData();*/
    m_iErrorCode = git_repository_init(&m_pRepo, qstring2cc(&path), bare);
    return m_iErrorCode >= 0;
}

bool GBL_Repository::clone_repo(QString srcUrl, QString dstPath)
{
    cleanup();
    QByteArray baUrl = srcUrl.toUtf8();
    const char* strUrl = baUrl.constData();
    QByteArray baPath = dstPath.toUtf8();
    const char* strPath = baPath.constData();
    m_iErrorCode = git_clone(&m_pRepo, strUrl, strPath, NULL);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::open_repo(QString path)
{
    cleanup();
    /*const QByteArray l8b = path.toUtf8();
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
                pHistItem->hist_message = QString(git_commit_message(commit));
                const git_signature *pGit_Sig = git_commit_author(commit);
                QString author;
                QTextStream(&author) << pGit_Sig->name << " <" << pGit_Sig->email << ">";
                pHistItem->hist_author = author;
                pHistItem->hist_author_email = QString(pGit_Sig->email);
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
        //pFItem->file_oid = QString(git_oid_tostr_s(pTroid));
        pFileMod->addFileItem(pFItem);
    }
    /*if (type == GIT_OBJ_TREE)
    {
        const git_oid *pTroid = git_tree_entry_id(entry);
        pRepo->tree_walk(pTroid);
    }*/

    return 1;
}

bool GBL_Repository::get_commit_to_parent_diff_files(QString oid_str, GBL_FileModel *pFileMod)
{
    return get_commit_to_parent_diff(oid_str, GIT_DIFF_FORMAT_NAME_STATUS,  diff_print_files_callback, pFileMod);
}

bool GBL_Repository::get_commit_to_parent_diff_lines(QString oid_str, MainWindow *pMain, char *path)
{
    return get_commit_to_parent_diff(oid_str, GIT_DIFF_FORMAT_PATCH, diff_print_lines_callback, pMain, path);
}

bool GBL_Repository::get_commit_to_parent_diff(QString oid_str, git_diff_format_t format, git_diff_line_cb callback, void *payload, char *path)
{
    git_oid oid;
    git_tree *pTree = NULL, *pParentTree = NULL;
    git_commit *pCommit = NULL, *pParentCommit = NULL;
    int nParentCount;
    git_diff_options diffopts = GIT_DIFF_OPTIONS_INIT;
    if (path)
    {
        diffopts.pathspec.strings = &path;
        diffopts.pathspec.count = 1;
    }

    git_diff *pDiff = NULL;

    const char* str = qstring2cc(&oid_str);
    m_iErrorCode = git_oid_fromstr(&oid, str);
    if (m_iErrorCode >= 0)
    {
       m_iErrorCode = git_commit_lookup(&pCommit, m_pRepo, &oid);
       if (m_iErrorCode >= 0)
       {
           m_iErrorCode = git_commit_tree(&pTree, pCommit);
           nParentCount = git_commit_parentcount(pCommit);
           if (nParentCount > 0)
           {
               for (int i = 0; i < nParentCount; i++)
               {
                   git_commit_parent(&pParentCommit, pCommit, i);
                   git_commit_tree(&pParentTree, pParentCommit);
                   git_diff_tree_to_tree(&pDiff, m_pRepo, pParentTree, pTree, &diffopts);
                   git_diff_print(pDiff, format, callback, payload);
                   git_diff_free(pDiff);
                   git_commit_free(pParentCommit);
                   git_tree_free(pParentTree);
               }
           }

       }
    }

    return m_iErrorCode >= 0;
}

int GBL_Repository::diff_print_files_callback(const git_diff_delta *pDelta, const git_diff_hunk *pHunk, const git_diff_line *pLine, void *payload)
{
    //qDebug() << "old_file" << pDelta->old_file.path;
    //qDebug() << "new_file" << pDelta->new_file.path;lkj
    Q_UNUSED(pHunk);
    Q_UNUSED(pLine);

    GBL_FileModel *pFileMod = (GBL_FileModel*)payload;

    GBL_File_Item *pFItem = new GBL_File_Item;
    QFileInfo fi(pDelta->new_file.path);
    pFItem->file_name = QString(fi.fileName());
    pFItem->sub_dir = QString(fi.path());
    pFItem->status = pDelta->status;
    pFileMod->addFileItem(pFItem);

    return 0;
}

int GBL_Repository::diff_print_lines_callback(const git_diff_delta *pDelta, const git_diff_hunk *pHunk, const git_diff_line *pLine, void *payload)
{
    Q_UNUSED(pHunk);

    //qDebug() << "old_file" << pDelta->old_file.path;
    //qDebug() << "new_file" << pDelta->new_file.path;

    /*if (pHunk)
    {
        qDebug() << "git_diff_hunk_new_lines" << pHunk->new_lines;
        qDebug() << "git_diff-hunk_new_start" << pHunk->new_start;
        qDebug() << "git_diff_hunk_old_lines" << pHunk->old_lines;
        qDebug() << "git_diff-hunk_old_start" << pHunk->old_start;
        QString sHeader;
        for (int i = 0; i < pHunk->header_len; i++)
        {
            sHeader += pHunk->header[i];
        }
        qDebug() << "git_diff_hunk_header" << sHeader;
    }*/

    if (pLine)
    {
        //qDebug() << "git_diff_line_origin" << pLine->origin;
        //qDebug() << "git_diff_line_new_line#" << pLine->new_lineno;
        //qDebug() << "git_diff_line_old_line#" << pLine->old_lineno;
        QString content(pLine->content);
        content = content.left(pLine->content_len);
        //qDebug() << "git_diff_line_content" << content;
        GBL_Line_Item li;
        li.content = content;
        li.line_change_type = pLine->origin;
        li.new_line_num = pLine->new_lineno;
        li.old_line_num = pLine->old_lineno;
        MainWindow *pMain = (MainWindow*)payload;
        pMain->addToDiffView(&li);

    }

    return 0;
}
