#include "gbl_storage.h"
#include <QDir>
#include <QStringList>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>
#include <QCoreApplication>

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

QString GBL_Storage::getThemesPath()
{
    QString sThemesPath;
    QTextStream(&sThemesPath) << getStoragePath() << QDir::separator() << "themes";

    return sThemesPath;
}

QString GBL_Storage::getGravatarUrl(QString sEmail)
{
    QByteArray baEmail = sEmail.toUtf8();
    QByteArray ba = QCryptographicHash::hash(baEmail, QCryptographicHash::Md5);
    QString sUrl;
    QTextStream(&sUrl) << "https://www.gravatar.com/avatar/" << ba.toHex() << "?d=identicon&s=40";
    //qDebug() << "getGravatarUrl:" << sUrl;
    return sUrl;
}

QStringList GBL_Storage::getThemes()
{
    QString sThemesPath = getThemesPath();
    QDir themesDir(sThemesPath);
    if (!themesDir.exists())
    {
        themesDir.mkpath(sThemesPath);
    }

    QStringList themes;
    themes << QString(QCoreApplication::tr("none"));
    themes << QString(QCoreApplication::tr("shawshank"));
    themes << QString(QCoreApplication::tr("zihuatanejo"));

    //find themes in the themes dir
    QStringList filters;
    filters << "*.qss";
    themesDir.setNameFilters(filters);
    QStringList exThemes = themesDir.entryList();
    for (int i = 0; i < exThemes.size(); i++)
    {
        QString filename = exThemes.at(i);
        QString lcFName = filename.toLower();
        lcFName = lcFName.replace(".qss", "");
        themes << lcFName;
    }
    themes.sort();
    return themes;
}

