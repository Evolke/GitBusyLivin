#ifndef GBL_STORAGE_H
#define GBL_STORAGE_H

#define GBL_STORAGE_DIR ".gitbusylivin"
#define GBL_STORAGE_CACHE_DIR "cache"
#define GBL_STORAGE_THEMES_DIR "themes"
#define GBL_STORAGE_BOOKMARKS_FILE "bookmarks.json"


#include <QString>

class GBL_Storage
{
public:
    GBL_Storage();

    static QString getStoragePath();
    static QString getCachePath();
    static QString getGravatarUrl(QString sEmail);
    static QString getThemesPath();
    QStringList getThemes();
    QByteArray readBookmarks();
    bool saveBookmarks(QByteArray bookmarkData);

};

#endif // GBL_STORAGE_H
