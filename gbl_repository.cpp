#include "gbl_repository.h"
#include <QTextStream>

GBL_Repository::GBL_Repository(QObject *parent) : QObject(parent)
{
    git_libgit2_init();
    repo = NULL;
    error_code = 0;
}

GBL_Repository::~GBL_Repository()
{
    cleanup();
    git_libgit2_shutdown();
}

void GBL_Repository::cleanup()
{
    if (repo)
    {
        git_repository_free(repo);
        repo = NULL;
    }
}

QString GBL_Repository::get_error_msg()
{
    const git_error *e = giterr_last();
    QString error_msg;
    QTextStream(&error_msg) << "Error " << e->klass << ", " << e->message;

    return error_msg;
}

bool GBL_Repository::init(QString path, bool bare)
{
    cleanup();
    const QByteArray l8b = path.toLocal8Bit();
    const char* spath = l8b.constData();
    error_code = git_repository_init(&repo, spath, bare);
    return error_code >= 0;
}

bool GBL_Repository::open(QString path)
{
    cleanup();
    const char* spath = path.toUtf8().constData();
    error_code = git_repository_open(&repo, spath);
    return error_code < 0;
}
