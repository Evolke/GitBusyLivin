#include "gbl_repository.h"
#include <QTextStream>

GBL_Repository::GBL_Repository(QObject *parent) : QObject(parent)
{
    git_libgit2_init();
    m_pRepo = NULL;
    m_iErrorCode = 0;
}

GBL_Repository::~GBL_Repository()
{
    cleanup();
    git_libgit2_shutdown();
}

void GBL_Repository::cleanup()
{
    if (m_pRepo)
    {
        git_repository_free(m_pRepo);
        m_pRepo = NULL;
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

bool GBL_Repository::init(QString path, bool bare)
{
    cleanup();
    const QByteArray l8b = path.toLocal8Bit();
    const char* spath = l8b.constData();
    m_iErrorCode = git_repository_init(&m_pRepo, spath, bare);
    return m_iErrorCode >= 0;
}

bool GBL_Repository::open(QString path)
{
    cleanup();
    const char* spath = path.toUtf8().constData();
    m_iErrorCode = git_repository_open(&m_pRepo, spath);
    return m_iErrorCode >= 0;
}
