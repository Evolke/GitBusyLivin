#ifndef GBL_REPOSITORY_H
#define GBL_REPOSITORY_H

#include <QObject>
#include <git2.h>
#include <QDateTime>
#include <QException>

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



typedef struct GBL_History_Item {
    QString hist_oid;
    QString hist_summary;
    QString hist_message;
    QDateTime hist_datetime;
    QString hist_author;
    QString hist_author_email;
} GBL_History_Item;

typedef QVector<GBL_History_Item*> GBL_History_Array;

typedef struct GBL_File_Item {
    char status;
    QString file_name;
    QString sub_dir;
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
QT_END_NAMESPACE

class GBL_RepositoryException : QException
{
    void raise() const { throw *this; }
    GBL_RepositoryException *clone() const { return new GBL_RepositoryException(*this); }

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

    static const char* qstring2cc(QString *pQStr);
    static int tree_walk_callback(const char *root, const git_tree_entry *entry, void *payload);
    static int diff_print_files_callback(const git_diff_delta*, const git_diff_hunk*, const git_diff_line*, void *payload);
    static int diff_print_lines_callback(const git_diff_delta*, const git_diff_hunk*, const git_diff_line*, void *payload);
    static int staged_cb(const char *path, const char *matched_pathspec, void *payload);

    QString get_error_msg();
    bool init_repo(QString path, bool bare=false);
    bool open_repo(QString path);
    bool clone_repo(QString srcUrl, QString dstPath);
    bool is_remote_repo(QString path);
    bool add_to_index(QStringList *pList);
    bool remove_from_index(QStringList *pList);
    bool index_unstage(QStringList *pList);
    bool commit_index(QString sMessage);
    bool get_history(GBL_History_Array **pHist_Arr);
    bool get_tree_from_commit_oid(QString oid_str, GBL_FileModel *pFileMod);
    void tree_walk(const git_oid *pTroid, GBL_FileModel *pFileMod);
    bool get_commit_to_parent_diff_files(QString oid_str, GBL_File_Array *pHistFileArr);
    bool get_commit_to_parent_diff_lines(QString oid_str, MainWindow *pMain, char *path);
    bool get_index_to_work_diff(MainWindow *pMain, QStringList *pList);
    bool get_index_to_head_diff(MainWindow *pMain, QStringList *pList);

    bool get_global_config_info(GBL_Config_Map **out);

    bool get_repo_status(GBL_File_Array &stagedArr, GBL_File_Array &unstagedArr);

signals:

public slots:

private:
    void cleanup();
    void cleanup_history();
    void check_libgit_return(int ret);
    bool get_commit_to_parent_diff(QString oid_str, git_diff_format_t format, git_diff_line_cb callback, void *payload, char *path=Q_NULLPTR);

    git_repository *m_pRepo;
    int m_iErrorCode;
    GBL_History_Array *m_pHist_Arr;
    GBL_Config_Map *m_pConfig_Map;

};

#endif // GBL_REPOSITORY_H
