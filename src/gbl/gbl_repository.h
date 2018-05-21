#ifndef GBL_REPOSITORY_H
#define GBL_REPOSITORY_H

#include "gbl_string.h"

#include <QObject>
#include <git2.h>
#include <QDateTime>
#include <QException>
#include <QMap>
#include <QStringList>

#define GBL_FILE_STATUS_ADDED 'A'
#define GBL_FILE_STATUS_DELETED 'D'
#define GBL_FILE_STATUS_MODIFIED 'M'
#define GBL_FILE_STATUS_RENAMED 'R'
#define GBL_FILE_STATUS_TYPECHANGE 'T'
#define GBL_FILE_STATUS_IGNORED 'I'
#define GBL_FILE_STATUS_COPIED 'C'
#define GBL_FILE_STATUS_CONFLICTED '!'
#define GBL_FILE_STATUS_UNTRACKED 'U'
#define GBL_FILE_STATUS_UNREADABLE '*'
#define GBL_FILE_STATUS_UNKNOWN '?'
#define GBL_FILE_STATUS_SYSTEM 'S'



typedef struct GBL_History_Item {
    QString hist_oid;
    QString hist_summary;
    QString hist_message;
    QDateTime hist_datetime;
    QString hist_author;
    QString hist_author_email;
    QStringList hist_parents;
} GBL_History_Item;

typedef QVector<GBL_History_Item*> GBL_History_Array;

typedef struct GBL_File_Item {
    char status;
    QString file_name;
    QString sub_dir;
    QString file_oid;
} GBL_File_Item;

typedef QVector<GBL_File_Item*> GBL_File_Array;


typedef struct GBL_Line_Item {
    char line_change_type;
    int old_line_num;
    int new_line_num;
    QString content;
} GBL_Line_Item;

typedef QVector<GBL_Line_Item*> GBL_Line_Array;
typedef QMap<QString, QString> GBL_Config_Map;


QT_BEGIN_NAMESPACE
class GBL_FileModel;
class MainWindow;
class GBL_RefItem;
class GBL_RefsModel;
class GBL_HistoryModel;
class GBL_Repository;
QT_END_NAMESPACE

typedef QMap<QString, GBL_RefItem*> GBL_Ref_Map;
typedef QList<GBL_RefItem*> GBL_Ref_Children;


typedef struct GBL_Tree_Walk_Payload {
    GBL_File_Array *file_arr;
    GBL_Repository *repo;
} GBL_Tree_Walk_Payload;

class GBL_RepositoryException : QException
{
    void raise() const { throw *this; }
    GBL_RepositoryException *clone() const { return new GBL_RepositoryException(*this); }

};

class GBL_RefItem
{
public:
    explicit GBL_RefItem(QString sKey, QString sRef, GBL_RefItem *pParent = NULL);
    ~GBL_RefItem();

    void cleanup();

    void addChild(GBL_RefItem *pRef);
    GBL_RefItem* findChild(QString sKey);
    GBL_RefItem* getParent() { return m_pParentRef; }
    GBL_RefItem* getChildAt(int index);
    QStringList getChildrenKeys();
    int getChildCount() { return m_refChildren.size(); }
    GBL_Ref_Children* getChildrenList() { return &m_refChildren; }
    int index();
    QString getKey() { return m_sKey; }
    QString getRef() { return m_sRef; }
    QString getName() { return m_sName; }
    void setName(QString sName) { m_sName = sName; }
    QIcon* getIcon() { return m_pIcon; }
    void setIcon(QIcon *pIcon) { m_pIcon = pIcon; }

    GBL_RefItem &operator=(GBL_RefItem &);

private:
    QString m_sKey;
    QString m_sName;
    QString m_sRef;
    GBL_Ref_Children m_refChildren;
    GBL_RefItem *m_pParentRef;
    QIcon *m_pIcon;
};


/**
 * @brief The GBL_Repository class
 */
class GBL_Repository : public QObject
{
    Q_OBJECT
public:
    explicit GBL_Repository(QObject *parent = 0);
    ~GBL_Repository();

    static int tree_walk_callback(const char *root, const git_tree_entry *entry, void *payload);
    static int diff_print_files_callback(const git_diff_delta*, const git_diff_hunk*, const git_diff_line*, void *payload);
    static int diff_print_lines_callback(const git_diff_delta*, const git_diff_hunk*, const git_diff_line*, void *payload);
    static int staged_cb(const char *path, const char *matched_pathspec, void *payload);
    static int stash_cb(size_t index, const char *message, const int *stash_id, void *payload);

    QString get_error_msg();
    QString get_libgit2_version();
    bool init_repo(GBL_String path, bool bare=false);
    bool open_repo(GBL_String path);
    bool is_bare();
    bool clone_repo(GBL_String srcUrl, GBL_String dstPath);
    bool is_remote_repo(GBL_String path);
    bool add_to_index(QStringList *pList);
    bool remove_from_index(QStringList *pList);
    bool index_unstage(QStringList *pList);
    bool commit_index(GBL_String sMessage);
    bool get_remotes(QStringList &remote_list);
    bool get_head_branch(QString &branch);
    bool get_upstream_ref(GBL_String sBranchName, git_reference **ref);
    bool create_branch(GBL_String sBranchName, GBL_String sCommitOid="");
    bool get_upstream_branch_name(GBL_String sBranchName, GBL_String &sUpstreamBranchName);
    bool set_upstream_branch(GBL_String sBranch, GBL_String sUpstreamBranch);
    bool get_ahead_behind_count(GBL_String sBranchName, int &ahead, int &behind);
    bool fetch_remote(GBL_String sRemote = "origin");
    bool pull_remote(GBL_String sRemote, GBL_String sBranch);
    bool push_to_remote(GBL_String sRemote, GBL_String sBranch);
    bool checkout_branch(GBL_String sBranchName);

    bool fill_references();
    bool fill_stashes();
    GBL_RefItem* get_references() { return m_pRefRoot; }
    QStringList getBranchNames();
    bool get_history(GBL_History_Array *io_pHistArr);
    bool get_tree_from_commit_oid(GBL_String oid_str, GBL_File_Array *pHistFileArr);
    void tree_walk(const git_oid *pTroid, GBL_File_Array *pHistFileArr);
    bool get_commit_to_parent_diff_files(GBL_String oid_str, GBL_File_Array *pHistFileArr);
    bool get_commit_to_parent_diff_lines(GBL_String oid_str, MainWindow *pMain, char *path);
    bool get_index_to_work_diff(MainWindow *pMain, QStringList *pList);
    bool get_index_to_head_diff(MainWindow *pMain, QStringList *pList);
    bool get_blob_content(GBL_String oid_str, QString& content);
    bool get_global_config_info(GBL_Config_Map **out);
    bool set_global_config_info(GBL_Config_Map *cfgMap);

    bool get_repo_status(GBL_File_Array *pStagedArr, GBL_File_Array *pUnstagedArr);

    git_repository* get_repository() { return m_pRepo; }
    void set_repository(git_repository *pRepo) { m_pRepo = pRepo; }

signals:
    void cleaningRepo();

public slots:

private slots:
    void cleanup();

private:
    //void cleanup_history();
    void init_ref_items();
    void check_libgit_return(int ret);
    bool get_commit_to_parent_diff(GBL_String oid_str, git_diff_format_t format, git_diff_line_cb callback, void *payload, char *path=Q_NULLPTR);

    git_repository *m_pRepo;
    int m_iErrorCode;
    //GBL_History_Array *m_pHist_Arr;
    GBL_Config_Map *m_pConfig_Map;
    GBL_RefItem *m_pRefRoot;
    int m_nCommitCount;
};

#endif // GBL_REPOSITORY_H
