#include "gbl_storage.h"
#include <QDir>
#include <QTextStream>

GBL_Storage::GBL_Storage()
{

}

QString GBL_Storage::getStoragePath()
{
    QString sStoragePath;
    QTextStream(&sStoragePath) << QDir::homePath() << QDir::separator() << ".gitbusylivin";

    return sStoragePath;
}

QString GBL_Storage::getCachePath()
{
    QString sCachePath;
    QTextStream(&sCachePath) << getStoragePath() << QDir::separator() << "cache";

    return sCachePath;
}
