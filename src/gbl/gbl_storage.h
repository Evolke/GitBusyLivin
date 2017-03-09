#ifndef GBL_STORAGE_H
#define GBL_STORAGE_H

#include <QString>

class GBL_Storage
{
public:
    GBL_Storage();

    static QString getStoragePath();
    static QString getCachePath();
    static QString getGravatarUrl(QString sEmail);

};

#endif // GBL_STORAGE_H