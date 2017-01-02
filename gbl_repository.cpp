#include "gbl_repository.h"

GBL_Repository::GBL_Repository(QObject *parent) : QObject(parent)
{
    git_libgit2_init();
    repo = NULL;
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

int GBL_Repository::init(QString path, bool bare)
{
    cleanup();
    const char* spath = path.toUtf8().constData();
    int error = git_repository_init(&repo, spath, bare);


    return error;
}

int GBL_Repository::open(QString path)
{
    cleanup();
    const char* spath = path.toUtf8().constData();
    int error = git_repository_open(&repo, spath);
    return error;
}
