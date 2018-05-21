#include "gbl_string.h"

GBL_String::GBL_String() : QString()
{

}


GBL_String::GBL_String(const QChar *unicode, int size) : QString(unicode, size)
{

}

GBL_String::GBL_String(QChar c) : QString(c)
{

}

GBL_String::GBL_String(int size, QChar c) : QString(size, c)
{

}

GBL_String::GBL_String(const char *str) : QString(str)
{

}

GBL_String::GBL_String(QLatin1String latin1) : QString(latin1)
{

}

GBL_String::GBL_String(QString &str) : QString(str)
{

}

GBL_String &GBL_String::operator=(const QString &str)
{
    return (GBL_String&)QString::operator=(str);
}

const char* GBL_String::toConstChar()
{
    m_baUtf = toUtf8();
    return m_baUtf.constData();
}

