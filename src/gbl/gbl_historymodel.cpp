#include "gbl_historymodel.h"
#include <QUrl>
#include <QPixmap>
#include <QTextStream>
#include <QIcon>
#include <QDebug>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "src/ui/urlpixmap.h"
#include "src/ui/mainwindow.h"
#include "gbl_storage.h"


GBL_HistoryModel::GBL_HistoryModel(GBL_History_Array *pHistArr, QObject *parent) : QAbstractTableModel(parent)
{
    m_pHistArr = pHistArr;
    m_headings.append(QString("Summary"));
    m_headings.append(QString("Author"));
    m_headings.append(QString("Date"));
}

GBL_HistoryModel::~GBL_HistoryModel()
{
    cleanupAvatars();
}

void GBL_HistoryModel::cleanupAvatars()
{
    QMapIterator<QString, UrlPixmap*> i(m_avatarMap);
    while (i.hasNext()) {
        UrlPixmap *pUrlpm = (UrlPixmap*)i.next().value();
        delete pUrlpm;
    }
    m_avatarMap.clear();
}

void GBL_HistoryModel::setModelData(GBL_History_Array *pHistArr)
{
    m_pHistArr = pHistArr;
    cleanupAvatars();
    m_emailList.clear();
    m_gravMap.clear();

    if (!pHistArr->isEmpty())
    {
        MainWindow *pMain = (MainWindow*)parent();
        QNetworkAccessManager *pNetAM = pMain->getNetworkAccessManager();
        //QNetworkDiskCache *pNetCache = pMain->getNetworkCache();

        for (int i = 0; i < m_pHistArr->size(); i++)
        {
            GBL_History_Item *pHistItem = m_pHistArr->at(i);
            QString sEmail = pHistItem->hist_author_email.toLower();
            if (!m_avatarMap.contains(sEmail))
            {
                UrlPixmap *pUrlpm = new UrlPixmap(pNetAM, this);
                m_avatarMap[sEmail] = pUrlpm;
                m_emailList.append(sEmail);
            }
            /*else
            {
                qDebug() << "avatarMap.contains = true";
            }*/
        }

        //qDebug() << "avatarMap.size:" << m_avatarMap.size();

        if (!m_emailList.isEmpty())
        {
           QString sEmail = m_emailList.first();
           m_emailList.removeFirst();
           QString sUrl = GBL_Storage::getGravatarUrl(sEmail);
           m_gravMap[sUrl] = sEmail;
           getAvatarFromUrl(sUrl, sEmail);
           //pUrlPM->loadFromUrl(sUrl);
           //connect(pUrlPM, SIGNAL (downloaded()), this, SLOT (avatarDownloaded()));
        }

        layoutChanged();
    }
}

QPixmap* GBL_HistoryModel::getAvatar(QString sEmail)
{
    QString slcEmail = sEmail.toLower();
    UrlPixmap *pAvatar = (UrlPixmap*)m_avatarMap[slcEmail];
    if (pAvatar) return pAvatar->getPixmap();

    return NULL;
}

void GBL_HistoryModel::getAvatarFromUrl(QString sUrl, QString sEmail)
{
    MainWindow *pMain = (MainWindow*)parent();
    QNetworkAccessManager *pNetAM = pMain->getNetworkAccessManager();

    connect(
      pNetAM, SIGNAL (finished(QNetworkReply*)),
      this, SLOT (avatarDownloaded(QNetworkReply*))
      );

    QNetworkRequest request(sUrl);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    pNetAM->get(request);
}

void GBL_HistoryModel::avatarDownloaded(QNetworkReply* pReply)
{
    if (m_avatarMap.isEmpty() || m_gravMap.isEmpty())
    {
        pReply->deleteLater();
        return;
    }

    QString sUrl = pReply->url().toString();

    QByteArray baImg = pReply->readAll();
    pReply->deleteLater();

    if (baImg.size() > 0)
    {
        QString sEmail = m_gravMap[sUrl];
        if (!sEmail.length()) return;

        UrlPixmap *pUrlpm = m_avatarMap[sEmail];
        if (!pUrlpm) return;

        //qDebug() << "avatarDownloaded;" << sUrl;
        //qDebug() << "avatarDownloaded size:" << baImg.size();
        layoutChanged();
        pUrlpm->setPixmapData(baImg);
        pReply->close();

        if (!m_emailList.isEmpty())
        {
           QString sEmail = m_emailList.first();
           m_emailList.removeFirst();
           //UrlPixmap *pUrlPM = m_pAvMapIt->value();
           //qDebug() << "next_email:" << sEmail;
           QString sUrl = GBL_Storage::getGravatarUrl(sEmail);
           m_gravMap[sUrl] = sEmail;
           getAvatarFromUrl(sUrl, sEmail);
           //pUrlPM->loadFromUrl(sUrl);
           //connect(pUrlPM, SIGNAL (downloaded()), this, SLOT (avatarDownloaded()));
        }
    }
}

QModelIndex GBL_HistoryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) { return QModelIndex(); }

    return createIndex(row, column);
}

int GBL_HistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_pHistArr)
    {
        return m_pHistArr->length();
    }

    return 0;
}

int GBL_HistoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (m_pHistArr) return 3;

    return 0;
}

GBL_History_Item* GBL_HistoryModel::getHistoryItemAt(int index)
{
    if (m_pHistArr && index >= 0 && index < m_pHistArr->length())
    {
        return m_pHistArr->at(index);
    }
    return NULL;
}

QVariant GBL_HistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_pHistArr) { return QVariant(); }
    if (index.row() > (m_pHistArr->length() -1) || index.row() < 0 ) { return QVariant(); }

    if (role == Qt::DisplayRole)
    {
        GBL_History_Item *pHistItem = m_pHistArr->at(index.row());
        switch (index.column())
        {
            case 0:
                return pHistItem->hist_summary;
            case 1:
                return pHistItem->hist_author;
            case 2:
                return pHistItem->hist_datetime.toString("M/d/yyyy h:mm ap");
        }
    }
    else if (role == Qt::DecorationRole && index.column() == 1)
    {
        GBL_History_Item *pHistItem = m_pHistArr->at(index.row());
        QString sEmail = pHistItem->hist_author_email.toLower();
        if (m_avatarMap.contains(sEmail))
        {
            UrlPixmap *pUP = m_avatarMap[sEmail];
            return QVariant::fromValue(*(pUP->getSmallPixmap(20)));
        }

        return QVariant();
    }
    return QVariant();
}

QVariant GBL_HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return m_headings.at(section);
     /*if (role == Qt::DecorationRole)
        return QVariant::fromValue(services);*/
    return QAbstractTableModel::headerData(section, orientation, role);
}
