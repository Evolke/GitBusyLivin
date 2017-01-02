#ifndef GBL_REPOSITORY_H
#define GBL_REPOSITORY_H

#include <QObject>
#include <git2.h>

class GBL_Repository : public QObject
{
    Q_OBJECT
public:
    explicit GBL_Repository(QObject *parent = 0);
    ~GBL_Repository();

    int init(QString path, bool bare=false);
    int open(QString path);

signals:

public slots:

private:
    void cleanup();

    git_repository *repo;
};

#endif // GBL_REPOSITORY_H
