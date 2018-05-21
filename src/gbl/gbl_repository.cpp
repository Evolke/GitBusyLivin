#include "gbl_repository.h"
#include <QTextStream>
#include <QDebug>
#include <QByteArray>
#include <QFileInfo>
//#include "libgit2/include/git2/sys/repository.h"

#include "gbl_filemodel.h"
#include "src/ui/mainwindow.h"
#include "gbl_historymodel.h"

static char git_buf__initbuf[1];

/* Use to initialize buffer structure when git_buf is on stack */
#define GIT_BUF_INIT { git_buf__initbuf, 0, 0 }

static QByteArrayList g_temp_balist;

GBL_Repository::GBL_Repository(QObject *parent) : QObject(parent)
{
    git_libgit2_init();
    m_pRepo = Q_NULLPTR;
    //m_pHist_Arr = Q_NULLPTR;
    m_iErrorCode = 0;
    m_nCommitCount = 0;
    m_pConfig_Map = new GBL_Config_Map;
    m_pRefRoot = new GBL_RefItem(QString(),QString());
}

GBL_Repository::~GBL_Repository()
{
    delete m_pConfig_Map;
    cleanup();

    delete m_pRefRoot;

    git_libgit2_shutdown();
}

/**
 * @brief GBL_Repository::cleanup
 */
void GBL_Repository::cleanup()
{
    emit cleaningRepo();

    //cleanup_history();
    m_pRefRoot->cleanup();

    if (m_pRepo)
    {
        git_repository_free(m_pRepo);
        m_pRepo = Q_NULLPTR;
    }

}

/**
 * @brief GBL_Repository::cleanup_history
 */
/*void GBL_Repository::cleanup_history()
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
}*/




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
 * @brief GBL_Repository::check_libgit_return
 * @param ret
 */
void GBL_Repository::check_libgit_return(int ret)
{
    m_iErrorCode = ret;

    if (ret < 0)
    {
        qDebug() << get_error_msg();

        throw GBL_RepositoryException();
    }
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
    }
    catch(GBL_RepositoryException &e)
    {
        Q_UNUSED(e);

    }

    *out = m_pConfig_Map;

    git_buf_free(&buf);
    if (cfg != NULL) git_config_free(cfg);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::set_global_config_info(GBL_Config_Map *cfgMap)
{
    git_config *cfg = NULL;
    git_buf buf = GIT_BUF_INIT;
    //const char *name, *email;
    try
    {
        check_libgit_return(git_config_open_default(&cfg));

        QMapIterator<QString, QString> i(*cfgMap);
        GBL_String sKey, sVal;
        while (i.hasNext()) {
            i.next();
            sKey = i.key();
            sVal = i.value();
            check_libgit_return(git_config_set_string(cfg, sKey.toConstChar(), sVal.toConstChar()));
        }

    }
    catch(GBL_RepositoryException &e)
    {
        Q_UNUSED(e);

    }

    if (cfg) git_config_free(cfg);

    return m_iErrorCode >= 0;
}

QString GBL_Repository::get_libgit2_version()
{
    int iMajor, iMinor, iRev;

    git_libgit2_version(&iMajor, &iMinor, &iRev);

    QString sVer;
    QTextStream(&sVer) << iMajor << "." << iMinor << "." << iRev;
    return sVer;
}

/**
 * @brief GBL_Repository::init_repo
 * @param path
 * @param bare
 * @return
 */
bool GBL_Repository::init_repo(GBL_String path, bool bare)
{
    cleanup();
    /*const QByteArray l8b = path.toUtf8();
    const char* spath = l8b.constData();*/
    git_signature *sig = NULL;
    git_index *index = NULL;
    git_oid tree_id, commit_id;
    git_tree *tree = NULL;
    git_repository_init_options opts = GIT_REPOSITORY_INIT_OPTIONS_INIT;

    try
    {
        opts.flags |= GIT_REPOSITORY_INIT_MKPATH | GIT_REPOSITORY_INIT_MKDIR;
        if (bare)
        {
            opts.flags |= GIT_REPOSITORY_INIT_BARE;
            opts.mode = GIT_REPOSITORY_INIT_SHARED_ALL;
        }

        check_libgit_return(git_repository_init_ext(&m_pRepo, path.toConstChar(), &opts));

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
bool GBL_Repository::clone_repo(GBL_String srcUrl, GBL_String dstPath)
{
    git_remote *pRemote = NULL;

    cleanup();
    try
    {
        check_libgit_return(git_clone(&m_pRepo, srcUrl.toConstChar(), dstPath.toConstChar(), NULL));

        //check to see if a remote exists
        QStringList remotes;
        get_remotes(remotes);
        if (remotes.isEmpty())
        {
            if (open_repo(dstPath))
            {
                git_remote_create(&pRemote, m_pRepo, "origin", srcUrl.toConstChar());
            }
        }
    }
    catch(GBL_RepositoryException &e)
    {

    }

    if (pRemote) git_remote_free(pRemote);

    return m_iErrorCode >= 0;
}

/**
 * @brief GBL_Repository::open_repo
 * @param path
 * @return
 */
bool GBL_Repository::open_repo(GBL_String path)
{
    cleanup();
    /*const QByteArray l8b = path.toUtf8();
    const char* spath = l8b.constData();*/
    m_iErrorCode = git_repository_open_ext(&m_pRepo, path.toConstChar(),GIT_REPOSITORY_OPEN_NO_SEARCH,NULL);
    return m_iErrorCode >= 0;
}

bool GBL_Repository::is_bare()
{
    return git_repository_is_bare(m_pRepo);
}

bool GBL_Repository::get_remotes(QStringList &remote_list)
{
    git_strarray remotes = {0};
    m_iErrorCode = git_remote_list(&remotes, m_pRepo);

    qDebug() << "get_remotes:" << remotes.count;

    for (int i=0; i < remotes.count; i++)
    {
        qDebug() << remotes.strings[i];
        remote_list.append(QString(remotes.strings[i]));
    }

    return m_iErrorCode >= 0;
}

bool GBL_Repository::get_head_branch(QString &branch)
{
    git_branch_iterator *pBIter = NULL;
    git_reference *pRef = NULL;
    git_branch_t branchType;
    const char *sBranchName;

    try
    {
        check_libgit_return(git_branch_iterator_new(&pBIter, m_pRepo, GIT_BRANCH_LOCAL));
        while (git_branch_next(&pRef, &branchType, pBIter) == 0)
        {
            git_branch_name(&sBranchName, pRef);
            qDebug() << "branch name:" << sBranchName;
            if (git_branch_is_head(pRef))
            {
                qDebug() << "is head branch";
                branch = sBranchName;
            }

            git_reference_free(pRef);
        }
    }
    catch(GBL_RepositoryException &e)
    {

    }

    if (pBIter) git_branch_iterator_free(pBIter);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::push_to_remote(GBL_String sRemote, GBL_String sBranch)
{
    git_reference *pRef = NULL;
    git_remote *pRemote = NULL;
    GBL_String sRefspec = "refs/heads/";
    sRefspec += sBranch + ":refs/heads/" + sBranch;
    char* refspec = new char[sRefspec.size()+1];
    strcpy(refspec, sRefspec.toConstChar());
    git_strarray refspecs = {
        &refspec,
        1,
    };

    try
    {
        GBL_String sUpstreamBranch;
        get_upstream_branch_name(sBranch, sUpstreamBranch);
        if (sUpstreamBranch.isEmpty())
        {
            GBL_String sRemoteBranch = "origin/";
            sRemoteBranch += sBranch;
            set_upstream_branch(sBranch,sRemoteBranch);
        }

        check_libgit_return(git_remote_lookup(&pRemote, m_pRepo, sRemote.toConstChar()));

         // connect to remote
         //check_libgit_return(git_remote_connect(pRemote, GIT_DIRECTION_PUSH, NULL, NULL, NULL));
         //git_remote_get_push_refspecs(&refspecs,(const git_remote*)pRemote);
         // add a push refspec
         //check_libgit_return(git_remote_add_push(m_pRepo, sRemote.toConstChar(), sRefspec.toConstChar()));

         // configure options
         git_push_options options;
         git_push_init_options( &options, GIT_PUSH_OPTIONS_VERSION );

         // do the push
         check_libgit_return(git_remote_push(pRemote, &refspecs, &options));

    }
    catch (GBL_RepositoryException &e)
    {

    }

    //git_strarray_free(refspecs);
    delete refspec;
    if (pRemote) git_remote_free(pRemote);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::pull_remote(GBL_String sRemote, GBL_String sBranch)
{
    git_annotated_commit *pAnnCommit = NULL;
    git_reference *pRef = NULL, *pHeadRef = NULL, *pNewRef = NULL;
    git_merge_analysis_t merge_analysis;
    git_merge_preference_t merge_preference;
    git_object *pRemoteObj = NULL;
    git_tree *pRemoteTreeObj = NULL;
    git_merge_options merge_options = GIT_MERGE_OPTIONS_INIT;
    git_checkout_options checkout_options = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_options.checkout_strategy = GIT_CHECKOUT_SAFE;
    git_index *index = NULL;

    if (fetch_remote(sRemote))
    {
        try
        {
            GBL_String sRemoteBranch = sRemote;
            sRemoteBranch += "/";
            sRemoteBranch += sBranch;
            get_upstream_ref(sBranch, &pRef);

            check_libgit_return(git_annotated_commit_from_ref(&pAnnCommit,m_pRepo,pRef));
            check_libgit_return(git_merge_analysis(&merge_analysis, &merge_preference, m_pRepo, (const git_annotated_commit **)&pAnnCommit, 1));

            if (merge_analysis & GIT_MERGE_ANALYSIS_FASTFORWARD)
            {
                check_libgit_return(git_reference_peel(&pRemoteObj, pRef, GIT_OBJ_COMMIT));
                check_libgit_return(git_repository_head(&pHeadRef, m_pRepo));
                check_libgit_return(git_commit_tree(&pRemoteTreeObj,(git_commit*)pRemoteObj));
                check_libgit_return(git_checkout_tree(m_pRepo, (git_object*)pRemoteTreeObj, &checkout_options));
                check_libgit_return(git_reference_set_target(&pNewRef, pHeadRef, git_commit_id((const git_commit*)pRemoteObj), NULL));
                //check_libgit_return(git_checkout_head(m_pRepo, &checkout_options));
            }
            else if (merge_analysis & GIT_MERGE_ANALYSIS_NORMAL)
            {
                check_libgit_return(git_merge(m_pRepo, (const git_annotated_commit **)&pAnnCommit, 1, &merge_options, &checkout_options));
                check_libgit_return(git_repository_index(&index, m_pRepo));

                if (!git_index_has_conflicts(index))
                {
                    GBL_String msg("Merged branch ");
                    msg += sBranch;
                    commit_index(msg);
                }
            }
        }
        catch (GBL_RepositoryException &e)
        {

        }

        if (pAnnCommit) git_annotated_commit_free(pAnnCommit);
        if (pRef) git_reference_free(pRef);
        if (pRemoteTreeObj) git_tree_free(pRemoteTreeObj);
        if (pRemoteObj) git_object_free(pRemoteObj);
        if (pHeadRef)git_reference_free(pHeadRef);
        if (pNewRef) git_reference_free(pNewRef);
    }

    return  m_iErrorCode >= 0;
}

bool GBL_Repository::fetch_remote(GBL_String sRemote)
{
    git_remote *remote = NULL;
    git_fetch_options options = GIT_FETCH_OPTIONS_INIT;

    try
    {
        check_libgit_return(git_remote_lookup(&remote, m_pRepo, sRemote.toConstChar()));

        check_libgit_return(git_remote_fetch(remote,NULL,&options,NULL));
    }
    catch (GBL_RepositoryException &e)
    {
        Q_UNUSED(e);
    }

    if (remote) git_remote_free(remote);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::checkout_branch(GBL_String sBranchName)
{
    git_object *pTreeObj = NULL;
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    try
    {
        check_libgit_return(git_revparse_single(&pTreeObj, m_pRepo, sBranchName.toConstChar()));
        check_libgit_return(git_checkout_tree(m_pRepo, pTreeObj, &opts));

        GBL_String sRef("refs/heads/");
        sRef += sBranchName;
        qDebug() << sRef;
        check_libgit_return(git_repository_set_head(m_pRepo, sRef.toConstChar()));
    }
    catch(GBL_RepositoryException &e)
    {

    }

    if (pTreeObj) git_object_free(pTreeObj);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::get_upstream_ref(GBL_String sBranchName, git_reference **upStreamRef)
{
    git_reference *ref = NULL;

    try
    {
        check_libgit_return(git_branch_lookup(&ref, m_pRepo, sBranchName.toConstChar(), GIT_BRANCH_LOCAL));
        check_libgit_return(git_branch_upstream(upStreamRef,ref));
    }
    catch (GBL_Repository &e)
    {
    }
    if (ref) git_reference_free(ref);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::create_branch(GBL_String sBranchName, GBL_String sCommitOid)
{
    git_reference *pHeadRef = NULL, *pBranchRef = NULL;
    git_commit *pBranchCommit = NULL;
    git_oid commitOid;

    try
    {
        if (sCommitOid.isEmpty())
        {
            check_libgit_return(git_repository_head(&pHeadRef, m_pRepo));
            check_libgit_return(git_reference_peel((git_object**)&pBranchCommit, pHeadRef, GIT_OBJ_COMMIT));
        }
        else
        {
            check_libgit_return(git_oid_fromstr(&commitOid, sCommitOid.toConstChar()));
            check_libgit_return(git_commit_lookup(&pBranchCommit, m_pRepo, (const git_oid*)&commitOid));
        }

        if (pBranchCommit)
        {
            check_libgit_return(git_branch_create(&pBranchRef, m_pRepo, sBranchName.toConstChar(), pBranchCommit,0));
        }
    }
    catch (GBL_RepositoryException &e)
    {

    }

    if (pHeadRef) git_reference_free(pHeadRef);
    if (pBranchRef) git_reference_free(pBranchRef);
    if (pBranchCommit) git_commit_free(pBranchCommit);
    return m_iErrorCode >= 0;

}

bool GBL_Repository::get_upstream_branch_name(GBL_String sBranchName, GBL_String &sUpstreamBranchName)
{
    git_reference *ref = NULL, *upStreamRef = NULL;
    const char *sUpStreamName;

    try
    {
        get_upstream_ref(sBranchName, &upStreamRef);
        check_libgit_return(git_branch_name(&sUpStreamName, upStreamRef));
    }
    catch (GBL_RepositoryException &e)
    {

    }

    if (upStreamRef) git_reference_free(upStreamRef);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::set_upstream_branch(GBL_String sBranch, GBL_String sUpstreamBranch)
{
    git_reference *pBranchRef = NULL;
    GBL_String sLocalBranch = "refs/heads/";
    sLocalBranch += sBranch;
    try
    {
        check_libgit_return(git_reference_lookup(&pBranchRef, m_pRepo, sLocalBranch.toConstChar()));
        check_libgit_return(git_branch_set_upstream(pBranchRef, sUpstreamBranch.toConstChar()));

    }
    catch (GBL_RepositoryException &e)
    {

    }

    if (pBranchRef) git_reference_free(pBranchRef);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::get_ahead_behind_count(GBL_String sBranchName, int &ahead, int &behind)
{
    git_reference *ref = NULL, *upStreamRef = NULL;
    const char *sUpStreamName;
    git_remote *remote = NULL;
    git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;

    git_oid local_oid, upstream_oid;
    size_t iAhead = 0, iBehind = 0;
    //git_strarray array;

    try
    {
        check_libgit_return(git_branch_lookup(&ref, m_pRepo, sBranchName.toConstChar(), GIT_BRANCH_LOCAL));
        check_libgit_return(git_branch_upstream(&upStreamRef,ref));
        check_libgit_return(git_branch_name(&sUpStreamName, upStreamRef));
        qDebug() << "upstream name:" << sUpStreamName;
        QString sUpStream  = sUpStreamName;
        QStringList pieces = sUpStream.split('/');
        GBL_String sRemote;
        sRemote = pieces.at(0);
        GBL_String sLocalRef("refs/heads/"), sRemoteRef("refs/remotes/");
        sLocalRef += sBranchName;
        sRemoteRef += sUpStreamName;
        int nLocal = git_reference_name_to_id(&local_oid,m_pRepo,sLocalRef.toConstChar());
        int nRemote = git_reference_name_to_id(&upstream_oid, m_pRepo, sRemoteRef.toConstChar());

        check_libgit_return(git_remote_lookup(&remote, m_pRepo, sRemote.toConstChar()));
        //check_libgit_return(git_remote_connect(remote, GIT_DIRECTION_FETCH, &callbacks, NULL, NULL));
        git_graph_ahead_behind(&iAhead, &iBehind, m_pRepo, &local_oid, &upstream_oid);
        ahead = iAhead;
        behind = iBehind;
        qDebug() << "ahead:" << iAhead;
        qDebug() << "behind:" << iBehind;
    }
    catch(GBL_RepositoryException &e)
    {
        Q_UNUSED(e);
    }

    if (ref) git_reference_free(ref);
    if (upStreamRef) git_reference_free(upStreamRef);
    if (remote)
    {
        //git_remote_disconnect(remote);
        git_remote_free(remote);
    }

    //git_strarray_free(&array);

    return m_iErrorCode >= 0;
}

void GBL_Repository::init_ref_items()
{
    GBL_RefItem *pChildRef = new GBL_RefItem(QString("heads"),QString(),m_pRefRoot);
    pChildRef->setName(tr("Branches"));
    m_pRefRoot->addChild(pChildRef);
    pChildRef = new GBL_RefItem(QString("remotes"),QString(),m_pRefRoot);
    pChildRef->setName(tr("Remotes"));
    m_pRefRoot->addChild(pChildRef);
    pChildRef = new GBL_RefItem(QString("tags"),QString(),m_pRefRoot);
    pChildRef->setName(tr("Tags"));
    m_pRefRoot->addChild(pChildRef);
    pChildRef = new GBL_RefItem(QString("stashes"),QString(),m_pRefRoot);
    pChildRef->setName(tr("Stashes"));
    m_pRefRoot->addChild(pChildRef);
}

bool GBL_Repository:: fill_references()
{
    git_strarray refs = {0};
    m_iErrorCode = git_reference_list(&refs, m_pRepo);

    //qDebug() << "get_references:" << refs.count;

    m_pRefRoot->cleanup();
    init_ref_items();

    QString sRef;
    for (int i=0; i < refs.count; i++)
    {
        //qDebug() << refs.strings[i];
       sRef = refs.strings[i];
       QStringList refPieces = sRef.split('/');
       if (refPieces.size() >= 3)
       {
           GBL_RefItem *pRef = m_pRefRoot;
           for (int i = 1; i < refPieces.size(); i++)
           {
               QString sReference;
               QString sKey = refPieces.at(i);
               GBL_RefItem *pChildRef;
               pChildRef = pRef->findChild(sKey);
               if (!pChildRef)
               {
                   if (i == refPieces.size()-1) sReference = sRef;
                   pChildRef = new GBL_RefItem(sKey,sReference, pRef);
                   pRef->addChild(pChildRef);
               }
               pRef = pChildRef;
           }
       }
    }

    fill_stashes();

    return m_iErrorCode >= 0;
}

bool GBL_Repository::fill_stashes()
{
    GBL_RefItem *pRef = m_pRefRoot->findChild(QString("stashes"));

    m_iErrorCode = git_stash_foreach(m_pRepo, (git_stash_cb)stash_cb, pRef);

    return m_iErrorCode >= 0;
}

int GBL_Repository::stash_cb(size_t index, const char *message, const int *stash_id, void *payload)
{
    //qDebug() << "stash_cb_index:" << index;
    //qDebug() << "stash_cb_message" << message;
    //qDebug() << "stash_cb_id:" << stash_id;

    GBL_RefItem *pRef = (GBL_RefItem*)payload;
    GBL_RefItem *pChildRef = new GBL_RefItem(QString::number(*stash_id),QString(),pRef);
    pChildRef->setName(message);
    pRef->addChild(pChildRef);

    return 0;
}

QStringList GBL_Repository::getBranchNames()
{
    QStringList branches;
    GBL_RefItem *pRef = m_pRefRoot->findChild(QString("heads"));
    if (pRef)
    {
        branches = pRef->getChildrenKeys();
    }

    return branches;
}


/**
 * @brief GBL_Repository::get_history
 * @param pHist_Arr
 * @return
 */
bool GBL_Repository::get_history(GBL_History_Array *io_pHistArr)
{
    git_revwalk *walker;
    git_oid oid;
    const git_oid *poid;

    try
    {
        int nMaxRevs = 300;
        m_nCommitCount = 0;

        check_libgit_return(git_revwalk_new(&walker, m_pRepo));
        check_libgit_return(git_revwalk_push_head(walker));
        //check_libgit_return(git_revwalk_push_range(walker, "HEAD~300..HEAD"));

        while (!git_revwalk_next(&oid, walker))
        {
            if (m_nCommitCount < nMaxRevs)
            {
                git_commit *pCommit = nullptr, *pParentCommit = nullptr;
                git_commit_lookup(&pCommit, m_pRepo, &oid);

                QString soid(git_oid_tostr_s(&oid));
                GBL_History_Item *pHistItem = new GBL_History_Item;
                pHistItem->hist_oid = soid;
                pHistItem->hist_summary = QString(git_commit_summary(pCommit));
                pHistItem->hist_message = QString(git_commit_message(pCommit));
                const git_signature *pGit_Sig = git_commit_author(pCommit);
                QString author;
                QTextStream(&author) << QString::fromUtf8(pGit_Sig->name) << " <" << pGit_Sig->email << ">";
                pHistItem->hist_author = author;
                pHistItem->hist_author_email = QString(pGit_Sig->email);
                pHistItem->hist_datetime = QDateTime::fromTime_t(pGit_Sig->when.time);

                int nParentCount = git_commit_parentcount(pCommit);
                if (nParentCount > 0)
                {
                    for (int i = 0; i < nParentCount; i++)
                    {
                        git_commit_parent(&pParentCommit, pCommit, i);
                        poid = git_commit_id(pParentCommit);
                        pHistItem->hist_parents.append(QString(git_oid_tostr_s(poid)));
                        git_commit_free(pParentCommit);
                    }
                }


                io_pHistArr->append(pHistItem);

                //qDebug() << pHistItem->hist_summary << pHistItem->hist_author << pHistItem->hist_datetime;

                // free the commit
                git_commit_free(pCommit);
            }

            m_nCommitCount++;
        }

    }
    catch(GBL_RepositoryException &e)
    {
    }

    qDebug() << "commit count:" << m_nCommitCount;

    if (walker) git_revwalk_free(walker);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::add_to_index(QStringList *pList)
{
    git_index *index = NULL;
    git_strarray array = {0};

    try
    {
        check_libgit_return(git_repository_index(&index,m_pRepo));

        array.count = pList->size();
        array.strings = (char**)::malloc(sizeof(char*) * array.count);

        g_temp_balist.clear();
        for (int i = 0; i < pList->size(); i++)
        {
            QString path = pList->at(i);
            QByteArray baPath = path.toUtf8();
            g_temp_balist.append(baPath);
            array.strings[i] = (char*)g_temp_balist.at(i).data();
        }

        git_index_add_all(index,&array,0, (git_index_matched_path_cb)staged_cb,NULL);
        git_index_write(index);
    }
    catch(GBL_RepositoryException &e)
    {

    }

    if (index) git_index_free(index);

    return m_iErrorCode >= 0;
}

int GBL_Repository::staged_cb(const char *path, const char *matched_pathspec, void *payload)
{
    //qDebug() << "staged_path:" << path << "staged_pathspec:" << matched_pathspec;

    return 0;
}

bool GBL_Repository::remove_from_index(QStringList *pList)
{
    git_index *index = NULL;
    git_strarray array = {0};

    try
    {
        check_libgit_return(git_repository_index(&index,m_pRepo));

        array.count = pList->size();
        array.strings = (char**)::malloc(sizeof(char*) * array.count);

        g_temp_balist.clear();
        for (int i = 0; i < pList->size(); i++)
        {
            QString path = pList->at(i);
            QByteArray baPath = path.toUtf8();
            g_temp_balist.append(baPath);
            array.strings[i] = (char*)g_temp_balist.at(i).data();
        }

        git_index_remove_all(index, &array, NULL, NULL);
        git_index_write(index);
    }
    catch(GBL_RepositoryException &e)
    {

    }

    if (index) git_index_free(index);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::index_unstage(QStringList *pList)
{
    git_strarray farr = {0};
    git_reference *head = NULL;
    git_object *head_obj = NULL;

    farr.count = pList->size();
    farr.strings = (char**)::malloc(sizeof(char*) * farr.count);

    g_temp_balist.clear();
    for (int i = 0; i < pList->size(); i++)
    {
        QString path = pList->at(i);
        QByteArray baPath = path.toUtf8();
        g_temp_balist.append(baPath);
        farr.strings[i] = (char*)g_temp_balist.at(i).data();
    }

    try
    {
        check_libgit_return(git_repository_head(&head, m_pRepo));
        check_libgit_return(git_reference_peel(&head_obj, head, GIT_OBJ_COMMIT));
        check_libgit_return(git_reset_default(m_pRepo,head_obj,&farr));
    }
    catch(GBL_RepositoryException &e)
    {

    }

    if (head) git_reference_free(head);
    if (head_obj) git_object_free(head_obj);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::commit_index(GBL_String sMessage)
{
    git_index *index = NULL;
    git_signature *sig = NULL;
    git_oid tree_id, commit_id, head_id;
    git_tree *tree = NULL;
    git_commit *parent = NULL;

    try
    {
        check_libgit_return(git_signature_default(&sig, m_pRepo));
        check_libgit_return(git_repository_index(&index, m_pRepo));
        check_libgit_return(git_index_write_tree(&tree_id, index));
        check_libgit_return(git_tree_lookup(&tree, m_pRepo, &tree_id));
        int error = git_reference_name_to_id(&head_id, m_pRepo, "HEAD");
        bool bHead = error >= 0;
        if (bHead)
        {
            check_libgit_return(git_commit_lookup(&parent,m_pRepo,&head_id));
            check_libgit_return(git_commit_create_v(&commit_id, m_pRepo, "HEAD", sig, sig, "UTF-8", sMessage.toConstChar(), tree, 1, parent));
        }
        else
        {
            check_libgit_return(git_commit_create_v(&commit_id, m_pRepo, "HEAD", sig, sig, NULL, sMessage.toConstChar(), tree, 0));
        }
    }
    catch(GBL_RepositoryException &e)
    {

    }

    if (index) git_index_free(index);
    if (sig) git_signature_free(sig);
    if (tree) git_tree_free(tree);
    if (parent) git_commit_free(parent);

    return m_iErrorCode >= 0;
}

/**
 * @brief GBL_Repository::get_tree_from_commit_oid
 * @param oid_str
 * @param pFileMod
 * @return
 */
bool GBL_Repository::get_tree_from_commit_oid(GBL_String oid_str, GBL_File_Array *pHistFileArr)
{

   git_oid oid;
   const char* str;
   if (oid_str.isEmpty())
   {
       git_reference *head_ref = NULL;
       git_object *head_obj = NULL;
       try
       {
           check_libgit_return(git_reference_dwim(&head_ref,m_pRepo, "HEAD"));
           check_libgit_return(git_reference_peel(&head_obj, head_ref, GIT_OBJ_COMMIT));
           oid = *git_object_id(head_obj);
       }
       catch(GBL_RepositoryException &e)
       {

       }
       if (head_ref) git_reference_free(head_ref);
       if (head_obj) git_object_free(head_obj);
   }
   else
   {
       str = oid_str.toConstChar();
       m_iErrorCode = git_oid_fromstr(&oid, str);
   }
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
                tree_walk(pTroid, pHistFileArr);
            }
        }
   }

   return m_iErrorCode >= 0;
}

bool GBL_Repository::get_blob_content(GBL_String oid_str, QString& content)
{
    const char* str = oid_str.toConstChar();
    git_blob *blob = NULL;
    git_oid oid;

    try
    {
        check_libgit_return(git_oid_fromstr(&oid, str));
        check_libgit_return(git_blob_lookup(&blob, m_pRepo, &oid));
        if (git_blob_is_binary(blob) == 0)
        {
            content = (const char*)git_blob_rawcontent(blob);
        }
    }
    catch(GBL_RepositoryException &e)
    {

    }

    if (blob) git_blob_free(blob);

    return m_iErrorCode >= 0;
}

/**
 * @brief GBL_Repository::tree_walk
 * @param pTroid
 * @param pFileMod
 */
void GBL_Repository::tree_walk(const git_oid *pTroid, GBL_File_Array *pHistFileArr)
{
    git_tree *pTree = NULL;
    m_iErrorCode = git_tree_lookup(&pTree, m_pRepo, pTroid);
    if (m_iErrorCode >= 0)
    {
        GBL_Tree_Walk_Payload twp;
        twp.file_arr = pHistFileArr;
        twp.repo = this;

        m_iErrorCode = git_tree_walk(pTree, GIT_TREEWALK_POST, (git_treewalk_cb)tree_walk_callback, &twp);
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

    GBL_Tree_Walk_Payload *ptwp = (GBL_Tree_Walk_Payload*)payload;
    GBL_File_Array *pHistFileArr = ptwp->file_arr;

    if (type == GIT_OBJ_BLOB)
    {
        GBL_File_Item *pFItem = new GBL_File_Item;
        pFItem->file_name = git_tree_entry_name(entry);
        pFItem->sub_dir = QString(root);
        pFItem->status = GBL_FILE_STATUS_SYSTEM;

        const git_oid *pTroid = git_tree_entry_id(entry);
        pFItem->file_oid = QString(git_oid_tostr_s(pTroid));
        pHistFileArr->append(pFItem);
    }
    /*if (type == GIT_OBJ_TREE)
    {
        const git_oid *pTroid = git_tree_entry_id(entry);
        GBL_Tree_Walk_Payload twp;
        twp.file_arr = pHistFileArr;
        twp.repo = ptwp->repo;
        ptwp->repo->tree_walk(pTroid, pHistFileArr);
    }*/

    return 1;
}

/**
 * @brief GBL_Repository::get_commit_to_parent_diff_files
 * @param oid_str
 * @param pFileMod
 * @return
 */
bool GBL_Repository::get_commit_to_parent_diff_files(GBL_String oid_str, GBL_File_Array *pHistFileArr)
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
bool GBL_Repository::get_commit_to_parent_diff_lines(GBL_String oid_str, MainWindow *pMain, char *path)
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
bool GBL_Repository::get_commit_to_parent_diff(GBL_String oid_str, git_diff_format_t format, git_diff_line_cb callback, void *payload, char *path)
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

    const char* str = oid_str.toConstChar();
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
           else
           {
               git_diff_tree_to_tree(&pDiff, m_pRepo, NULL, pTree, &diffopts);
               git_diff_print(pDiff, format, callback, payload);
               git_diff_free(pDiff);

           }
           git_tree_free(pTree);
           git_commit_free(pCommit);
       }
    }

    return m_iErrorCode >= 0;
}

/**
 * @brief GBL_Repository::get_index_to_work_diff
 * @param pMain
 * @param pList
 * @return
 */
bool GBL_Repository::get_index_to_work_diff(MainWindow *pMain, QStringList *pList)
{
    git_diff *diff = NULL;
    git_diff_options diffopts = GIT_DIFF_OPTIONS_INIT;
    if (pList && pList->size())
    {
        diffopts.pathspec.count = pList->size();
        diffopts.pathspec.strings = (char**)::malloc(sizeof(char*) * pList->size());
        diffopts.flags = GIT_DIFF_INCLUDE_UNTRACKED|GIT_DIFF_SHOW_UNTRACKED_CONTENT|GIT_DIFF_RECURSE_UNTRACKED_DIRS;

        g_temp_balist.clear();
        for (int i = 0; i < pList->size(); i++)
        {
            QString path = pList->at(i);
            QByteArray baPath = path.toUtf8();
            g_temp_balist.append(baPath);
            diffopts.pathspec.strings[i] = (char*)g_temp_balist.at(i).data();
        }

        //diffopts.pathspec.count = pList->size();
    }

    try
    {
        check_libgit_return(git_diff_index_to_workdir(&diff, m_pRepo, NULL, &diffopts));
        check_libgit_return(git_diff_print(diff, GIT_DIFF_FORMAT_PATCH, diff_print_lines_callback, pMain));
    }
    catch(GBL_RepositoryException &e)
    {
    }

    if (diff) git_diff_free(diff);

    return m_iErrorCode >= 0;
}

bool GBL_Repository::get_index_to_head_diff(MainWindow *pMain, QStringList *pList)
{
    git_object *obj = NULL;
    git_diff *diff = NULL;
    git_tree *tree = NULL;
    git_diff_options diffopts = GIT_DIFF_OPTIONS_INIT;
    if (pList && pList->size())
    {
        diffopts.pathspec.count = pList->size();
        diffopts.pathspec.strings = (char**)::malloc(sizeof(char*) * pList->size());
        //diffopts.flags = GIT_DIFF_INCLUDE_UNTRACKED|GIT_DIFF_SHOW_UNTRACKED_CONTENT|GIT_DIFF_RECURSE_UNTRACKED_DIRS;

        g_temp_balist.clear();
        for (int i = 0; i < pList->size(); i++)
        {
            QString path = pList->at(i);
            QByteArray baPath = path.toUtf8();
            g_temp_balist.append(baPath);
            diffopts.pathspec.strings[i] = (char*)g_temp_balist.at(i).data();
        }

        //diffopts.pathspec.count = pList->size();
    }

    try
    {
        check_libgit_return(git_revparse_single(&obj, m_pRepo, "HEAD^{tree}"));
        check_libgit_return(git_tree_lookup(&tree, m_pRepo, git_object_id(obj)));
        check_libgit_return(git_diff_tree_to_index(&diff, m_pRepo, tree, NULL, &diffopts));
        check_libgit_return(git_diff_print(diff, GIT_DIFF_FORMAT_PATCH, diff_print_lines_callback, pMain));
    }
    catch(GBL_RepositoryException &e)
    {
    }

    if (obj) git_object_free(obj);
    if (tree) git_tree_free(tree);
    if (diff) git_diff_free(diff);

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

bool GBL_Repository::get_repo_status(GBL_File_Array *pStagedArr, GBL_File_Array *pUnstagedArr)
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

                pStagedArr->append(pFItem);
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
                pUnstagedArr->append(pFItem);

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

GBL_RefItem::GBL_RefItem(QString sKey, QString sRef, GBL_RefItem *pParent)
{
    m_sKey = sKey;
    m_sName = sKey;
    m_sRef = sRef;
    m_pParentRef = pParent;
    m_pIcon = NULL;
}

GBL_RefItem::~GBL_RefItem()
{
    cleanup();
}

void GBL_RefItem::cleanup()
{
    for (int i = 0; i < m_refChildren.size(); i++)
    {
        GBL_RefItem *pRef = m_refChildren.at(i);
        delete pRef;
    }

    m_refChildren.clear();

}

void GBL_RefItem::addChild(GBL_RefItem *pRef)
{
    m_refChildren.append(pRef);
}

GBL_RefItem* GBL_RefItem::findChild(QString sKey)
{
    for (int i = 0; i < m_refChildren.size(); i++)
    {
        GBL_RefItem *pRef = m_refChildren.at(i);
        if (pRef->getKey() == sKey) return pRef;
    }

    return NULL;
}

GBL_RefItem* GBL_RefItem::getChildAt(int index)
{
    if (index >= 0 && index < m_refChildren.size())
    {
        return m_refChildren.at(index);
    }

    return NULL;
}

QStringList GBL_RefItem::getChildrenKeys()
{
    QStringList sKeys;

    for (int i = 0; i < m_refChildren.size(); i++)
    {
        GBL_RefItem *pRef = m_refChildren.at(i);
        sKeys.append(pRef->getKey());
    }

    return sKeys;

}

int GBL_RefItem::index()
{
    if (m_pParentRef)
    {
        return m_pParentRef->getChildrenList()->indexOf(this);
    }

    return -1;
}

GBL_RefItem& GBL_RefItem::operator=(GBL_RefItem &ref)
{
    cleanup();
    m_sKey = ref.getKey();
    m_sName = ref.getName();
    m_sRef = ref.getRef();

    for (int i=0; i < ref.getChildCount(); i++)
    {
        GBL_RefItem *pRefItem = new GBL_RefItem("","",this);
        *pRefItem = *ref.getChildAt(i);
        m_refChildren.append(pRefItem);
    }

    return *this;
}
