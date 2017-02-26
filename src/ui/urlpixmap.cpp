#include "urlpixmap.h"
#include <QPixmap>
#include <QIcon>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


UrlPixmap::UrlPixmap(QObject *parent) : QObject(parent)
{
    m_pPixmap = new QPixmap();
    m_pSmallPixmap = new QPixmap();
    m_pNetAM = new QNetworkAccessManager();
}

UrlPixmap::~UrlPixmap()
{
    delete m_pPixmap;
    delete m_pNetAM;
    if (m_pSmallPixmap) delete m_pSmallPixmap;
}

void UrlPixmap::loadFromUrl(QString imageUrl)
{
    connect(
      m_pNetAM, SIGNAL (finished(QNetworkReply*)),
      this, SLOT (fileDownloaded(QNetworkReply*))
      );

    QNetworkRequest request(imageUrl);
    m_pNetAM->get(request);
}

void UrlPixmap::fileDownloaded(QNetworkReply *pReply)
{
    QByteArray baImg = pReply->readAll();
    m_pPixmap->loadFromData(baImg);
    emit downloaded();
}

QPixmap* UrlPixmap::getPixmap()
{
    return m_pPixmap;
}

QPixmap* UrlPixmap::getSmallPixmap(int size)
{
    if (!m_pPixmap->isNull())
    {
        QPixmap smPM = m_pPixmap->scaledToWidth(size);
         m_pSmallPixmap->swap(smPM);
    }


    return m_pSmallPixmap;
}
