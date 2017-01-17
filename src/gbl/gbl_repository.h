#ifndef GBL_REPOSITORY_H
#define GBL_REPOSITORY_H

#include <QObject>
#include <git2.h>
#include <QDateTime>
#include <QMap>

typedef struct GBL_History_Item {
    QString hist_oid;
    QString hist_summary;
    QDateTime hist_datetime;
    QString hist_author;
} GBL_History_Item;

typedef QVector<GBL_History_Item*> GBL_History_Array;

class GBL_Repository : public QObject
{
    Q_OBJECT
public:
    explicit GBL_Repository(QObject *parent = 0);
    ~GBL_Repository();

    QString get_error_msg();
    bool init(QString path, bool bare=false);
    bool open(QString path);
    bool get_history(GBL_History_Array **pHist_Arr);

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
