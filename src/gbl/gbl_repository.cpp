#include "gbl_repository.h"
#include <QTextStream>
#include <QDebug>
#include <QByteArray>
#include <QFileInfo>
//#include "libgit2/include/git2/sys/repository.h"

#include "gbl_filemodel.h"
#include "src/ui/mainwindow.h"

static char git_buf__initbuf[1];

/* Use to initialize buffer structure when git_buf is on stack */
#define GIT_BUF_INIT { git_buf__initbuf, 0, 0 }

static QByteArray g_temp_ba;

GBL_Repository::GBL_Repository(QObject *parent) : QObject(parent)
{
    git_libgit2_init();
    m_pRepo = Q_NULLPTR;
    m_pHist_Arr = Q_NULLPTR;
    m_iErrorCode = 0;
    m_pConfig_Map = new GBL_Config_Map;
}

GBL_Repository::~GBL_Repository()
{
    delete m_pConfig_Map;

    cleanup();
    git_libgit2_shutdown();
}

/**
 * @brief GBL_Repository::cleanup
 */
void GBL_Repository::cleanup()
{
    cleanup_history();

    if (m_pRepo)
    {
        git_repository_free(m_pRepo);
        m_pRepo = Q_NULLPTR;
    }

}

/**
 * @brief GBL_Repository::cleanup_history
 */
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

/**
 * @brief GBL_Repository::get_error_msg
 * @return
 */
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

/**
 * @brief GBL_Repository::qstring2cc
 * @param pQStr
 * @return
 */
const char* GBL_Repository::qstring2cc(QString *pQStr)
{
    g_temp_ba = pQStr->toUtf8();
    const char* str = g_temp_ba.constData();

    return str;
}

/**
 * @brief GBL_Repository::check_libgit_return
 * @param ret
 */
void GBL_Repository::check_libgit_return(int ret)
{
    m_iErrorCode = ret;

    if (ret < 0) throw GBL_RepositoryException();
}

/**
 * @brief GBL_Repository::get_global_config_info
 * @param out
 * @return
 */
bool GBL_Repository::get_global_config_info(GBL_Config_Map **out)
{
    git_config *cfg = NULL;
    git_buf buf = GIT_BUF_INIT;
    //const char *name, *email;
    try
    {
        check_libgit_return(git_config_find_global(&buf));
        check_libgit_return(git_config_open_ondisk(&cfg, buf.ptr));
        git_buf_free(&buf);
        check_libgit_return(git_config_get_string_buf(&buf, cfg, "user.name"));
        m_pConfig_Map->insert(QString("global.user.name"), QString(buf.ptr));
        git_buf_free(&buf);
        check_libgit_return(git_config_get_string_buf(&buf, cfg, "user.email"));
        m_pConfig_Map->insert(QString("global.user.email"),QString(buf.ptr));

       *out = m_pConfig_Map;
    }
    catch(GBL_RepositoryException &e)
    {
        Q_UNUSED(e);

        return false;
    }

    git_buf_free(&buf);
    if (cfg != NULL) git_config_free(cfg);
    return true;
}

/**
 * @brief GBL_Repository::init_repo
 * @param path
 * @param bare
 * @return
 */
bool GBL_Repository::init_repo(QString path, bool bare)
{
    cleanup();
    /*const QByteArray l8b = path.toUtf8();
    const char* spath = l8b.constData();*/
    git_signature *sig = NULL;
    git_index *index = NULL;
    git_oid tree_id, commit_id;
    git_tree *tree = NULL;

    try
    {
        check_libgit_return(git_repository_init(&m_pRepo, qstring2cc(&path), bare));

        //create initial commit
        if (!bare)
        {

            //get the signature to use for commit
            check_libgit_return(git_signature_default(&sig, m_pRepo));

            check_libgit_return(git_repository_index(&index, m_pRepo));
            check_libgit_return(git_index_write_tree(&tree_id, index));
            git_index_free(index);
            index = NULL;
            check_libgit_return(git_tree_lookup(&tree, m_pRepo, &tree_id));
            check_libgit_return(git_commit_create_v(&commit_id, m_pRepo, "HEAD", sig, sig, NULL, "Initial Commit", tree, 0));
            git_tree_free(tree);
            git_signature_free(sig);
        }

        return true;
    }
    catch(GBL_RepositoryException &e)
    {
        Q_UNUSED(e);

        if (index != NULL) git_index_free(index);
        if (tree != NULL) git_tree_free(tree);
        if (sig != NULL) git_signature_free(sig);

        return false;
    }

}

/**
 * @brief GBL_Repository::clone_repo
 * @param srcUrl
 * @param dstPath
 * @return
 */
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

/**
 * @brief GBL_Repository::open_repo
 * @param path
 * @return
 */
bool GBL_Repository::open_repo(QString path)
{
    cleanup();
    /*const QByteArray l8b = path.toUtf8();
    const char* spath = l8b.constData();*/
    m_iErrorCode = git_repository_open(&m_pRepo, qstring2cc(&path));
    return m_iErrorCode >= 0;
}

/**
 * @brief GBL_Repository::get_history
 * @param pHist_Arr
 * @return
 */
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
                QTextStream(&author) << QString::fromUtf8(pGit_Sig->name) << " <" << pGit_Sig->email << ">";
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

/**
 * @brief GBL_Repository::get_tree_from_commit_oid
 * @param oid_str
 * @param pFileMod
 * @return
 */
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

/**
 * @brief GBL_Repository::tree_walk
 * @param pTroid
 * @param pFileMod
 */
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

/**
 * @brief GBL_Repository::tree_walk_callback
 * @param root
 * @param entry
 * @param payload
 * @return
 */
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

/**
 * @brief GBL_Repository::get_commit_to_parent_diff_files
 * @param oid_str
 * @param pFileMod
 * @return
 */
bool GBL_Repository::get_commit_to_parent_diff_files(QString oid_str, GBL_File_Array *pHistFileArr)
{
    return get_commit_to_parent_diff(oid_str, GIT_DIFF_FORMAT_NAME_STATUS,  diff_print_files_callback, pHistFileArr);
}

/**
 * @brief GBL_Repository::get_commit_to_parent_diff_lines
 * @param oid_str
 * @param pMain
 * @param path
 * @return
 */
bool GBL_Repository::get_commit_to_parent_diff_lines(QString oid_str, MainWindow *pMain, char *path)
{
    return get_commit_to_parent_diff(oid_str, GIT_DIFF_FORMAT_PATCH, diff_print_lines_callback, pMain, path);
}

/**
 * @brief GBL_Repository::get_commit_to_parent_diff
 * @param oid_str
 * @param format
 * @param callback
 * @param payload
 * @param path
 * @return
 */
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

bool GBL_Repository::get_index_to_work_diff(MainWindow *pMain, char *path)
{
    git_diff *diff = NULL;
    git_diff_options diffopts = GIT_DIFF_OPTIONS_INIT;
    if (path)
    {
        diffopts.pathspec.strings = &path;
        diffopts.pathspec.count = 1;
    }


    m_iErrorCode = git_diff_index_to_workdir(&diff, m_pRepo, NULL, &diffopts);
    if (m_iErrorCode >= 0)
    {
        m_iErrorCode = git_diff_print(diff, GIT_DIFF_FORMAT_PATCH, diff_print_lines_callback, pMain);
        git_diff_free(diff);
    }
    return m_iErrorCode >= 0;
}

/**
 * @brief GBL_Repository::diff_print_files_callback
 * @param pDelta
 * @param pHunk
 * @param pLine
 * @param payload
 * @return
 */
int GBL_Repository::diff_print_files_callback(const git_diff_delta *pDelta, const git_diff_hunk *pHunk, const git_diff_line *pLine, void *payload)
{
    //qDebug() << "old_file" << pDelta->old_file.path;
    //qDebug() << "new_file" << pDelta->new_file.path;lkj
    Q_UNUSED(pHunk);
    Q_UNUSED(pLine);

    GBL_File_Array *pFileArr = (GBL_File_Array*)payload;

    GBL_File_Item *pFItem = new GBL_File_Item;
    QFileInfo fi(pDelta->new_file.path);
    pFItem->file_name = QString(fi.fileName());
    pFItem->sub_dir = QString(fi.path());

    switch (pDelta->status)
    {
        case GIT_DELTA_ADDED:
            pFItem->status = GBL_FILE_STATUS_ADDED;
            break;
        case GIT_DELTA_CONFLICTED:
            pFItem->status = GBL_FILE_STATUS_CONFLICTED;
            break;
        case GIT_DELTA_COPIED:
            pFItem->status = GBL_FILE_STATUS_COPIED;
            break;
        case GIT_DELTA_DELETED:
            pFItem->status = GBL_FILE_STATUS_DELETED;
            break;
        case GIT_DELTA_IGNORED:
            pFItem->status = GBL_FILE_STATUS_IGNORED;
            break;
        case GIT_DELTA_MODIFIED:
            pFItem->status = GBL_FILE_STATUS_MODIFIED;
            break;
         case GIT_DELTA_RENAMED:
            pFItem->status = GBL_FILE_STATUS_RENAMED;
            break;
        case GIT_DELTA_TYPECHANGE:
           pFItem->status = GBL_FILE_STATUS_TYPECHANGE;
           break;
        case GIT_DELTA_UNREADABLE:
           pFItem->status = GBL_FILE_STATUS_UNREADABLE;
           break;
        case GIT_DELTA_UNTRACKED:
           pFItem->status = GBL_FILE_STATUS_UNTRACKED;
           break;
        default:
            pFItem->status = GBL_FILE_STATUS_UNKNOWN;

    }

    //pFItem->status = pDelta->status;
    pFileArr->append(pFItem);

    return 0;
}

/**
 * @brief GBL_Repository::diff_print_lines_callback
 * @param pDelta
 * @param pHunk
 * @param pLine
 * @param payload
 * @return
 */
int GBL_Repository::diff_print_lines_callback(const git_diff_delta *pDelta, const git_diff_hunk *pHunk, const git_diff_line *pLine, void *payload)
{
    Q_UNUSED(pDelta);
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

bool GBL_Repository::get_repo_status(GBL_File_Array &stagedArr, GBL_File_Array &unstagedArr)
{
    git_status_list *status;
    git_status_options opts = GIT_STATUS_OPTIONS_INIT;
    opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;

    try
    {
        check_libgit_return(git_status_list_new(&status, m_pRepo, &opts));

        size_t i, maxi = git_status_list_entrycount(status);
        const git_status_entry *s;

        for (i = 0; i < maxi; ++i)
        {
            s = git_status_byindex(status, i);

            //qDebug() << "status" << s->status;

            if (s->head_to_index)
            {
                /*qDebug() << "head_to_index";
                qDebug() << "new_file" << s->head_to_index->new_file.path;
                qDebug() << "old_file" << s->head_to_index->old_file.path;
                */
                GBL_File_Item *pFItem = new GBL_File_Item;
                QFileInfo fi(s->head_to_index->new_file.path);
                pFItem->file_name = QString(fi.fileName());
                pFItem->sub_dir = QString(fi.path());

                if (s->status & GIT_STATUS_INDEX_NEW)
                    pFItem->status = GBL_FILE_STATUS_ADDED;
                if (s->status & GIT_STATUS_INDEX_MODIFIED)
                    pFItem->status = GBL_FILE_STATUS_MODIFIED;
                if (s->status & GIT_STATUS_INDEX_DELETED)
                    pFItem->status = GBL_FILE_STATUS_DELETED;
                if (s->status & GIT_STATUS_INDEX_RENAMED)
                    pFItem->status = GBL_FILE_STATUS_RENAMED;
                if (s->status & GIT_STATUS_INDEX_TYPECHANGE)
                    pFItem->status = GBL_FILE_STATUS_TYPECHANGE;
                if (s->status & GIT_STATUS_IGNORED)
                    pFItem->status = GBL_FILE_STATUS_IGNORED;

                stagedArr.append(pFItem);
            }

            if (s->index_to_workdir)
            {
                //qDebug() << "index_to_workdir";
                //qDebug() << "new_file" << s->index_to_workdir->new_file.path;
                //qDebug() << "old_file" << s->index_to_workdir->old_file.path;

                GBL_File_Item *pFItem = new GBL_File_Item;
                QFileInfo fi(s->index_to_workdir->new_file.path);
                pFItem->file_name = QString(fi.fileName());
                pFItem->sub_dir = QString(fi.path());
                if (s->status & GIT_STATUS_WT_NEW)
                    pFItem->status = GBL_FILE_STATUS_UNKNOWN;
                if (s->status & GIT_STATUS_WT_MODIFIED)
                    pFItem->status = GBL_FILE_STATUS_MODIFIED;
                if (s->status & GIT_STATUS_WT_DELETED)
                    pFItem->status = GBL_FILE_STATUS_DELETED;
                if (s->status & GIT_STATUS_WT_RENAMED)
                    pFItem->status = GBL_FILE_STATUS_RENAMED;
                if (s->status & GIT_STATUS_WT_TYPECHANGE)
                    pFItem->status = GBL_FILE_STATUS_TYPECHANGE;
                if (s->status & GIT_STATUS_IGNORED)
                    pFItem->status = GBL_FILE_STATUS_IGNORED;
                unstagedArr.append(pFItem);

            }
        }

        git_status_list_free(status);
        return true;
    }
    catch(GBL_RepositoryException &e)
    {
    }

    return false;
}
