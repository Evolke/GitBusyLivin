#ifndef GBL_STRING_H
#define GBL_STRING_H

#include <QString>

class GBL_String : public QString
{
public:
    GBL_String();
    explicit GBL_String(const QChar *unicode, int size = -1);
    GBL_String(QChar c);
    GBL_String(int size, QChar c);
    GBL_String(QString &str);
    inline GBL_String(QLatin1String latin1);
    GBL_String &operator=(const QString &);

    const char* toConstChar();

private:
    QByteArray m_baUtf;
};

#endif // GBL_STRING_H
