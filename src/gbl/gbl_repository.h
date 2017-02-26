#ifndef GBL_REPOSITORY_H
#define GBL_REPOSITORY_H

#include <QObject>
#include <git2.h>
#include <QDateTime>

typedef struct GBL_History_Item {
    QString hist_oid;
    QString hist_summary;
    QDateTime hist_datetime;
    QString hist_author;
    QString hist_author_email;
} GBL_History_Item;

typedef QVector<GBL_History_Item*> GBL_History_Array;

typedef struct GBL_File_Item {
    git_delta_t status;
    QString file_name;
    QString sub_dir;
} GBL_File_Item;

typedef QVector<GBL_File_Item*> GBL_File_Array;

class GBL_FileModel;

class GBL_Repository : public QObject
{
    Q_OBJECT
public:
    explicit GBL_Repository(QObject *parent = 0);
    ~GBL_Repository();

    static const char* qstring2cc(QString *pQStr);
    static int tree_walk_callback(const char *root, const git_tree_entry *entry, void *payload);
    static int diff_print_callback(const git_diff_delta*, const git_diff_hunk*, const git_diff_line*, void *payload);

    QString get_error_msg();
    bool init_repo(QString path, bool bare=false);
    bool open_repo(QString path);
    bool clone_repo(QString srcUrl, QString dstPath);

    bool get_history(GBL_History_Array **pHist_Arr);
    bool get_tree_from_commit_oid(QString oid_str, GBL_FileModel *pFileMod);
    void tree_walk(const git_oid *pTroid, GBL_FileModel *pFileMod);
    bool get_commit_to_parent_diff(QString oid_str, GBL_FileModel *pFileMod);

signals:

public slots:

private:
    void cleanup();
    void cleanup_history();
    git_repository *m_pRepo;
    int m_iErrorCode;
    GBL_History_Array *m_pHist_Arr;
};

#endif // GBL_REPOSITORY_H
