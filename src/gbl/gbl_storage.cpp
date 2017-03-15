#include "gbl_storage.h"
#include <QDir>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>

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

QString GBL_Storage::getGravatarUrl(QString sEmail)
{
    QByteArray baEmail = sEmail.toUtf8();
    QByteArray ba = QCryptographicHash::hash(baEmail, QCryptographicHash::Md5);
    QString sUrl;
    QTextStream(&sUrl) << "https://www.gravatar.com/avatar/" << ba.toHex() << "?d=identicon&s=48";
    //qDebug() << "getGravatarUrl:" << sUrl;
    return sUrl;
}

