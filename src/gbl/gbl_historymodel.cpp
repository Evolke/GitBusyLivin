#include "gbl_historymodel.h"
#include <QCryptographicHash>
#include <QUrl>
#include <QPixmap>
#include <QTextStream>
#include <QIcon>
#include <QDebug>
#include "src/ui/urlpixmap.h"

GBL_HistoryModel::GBL_HistoryModel(GBL_History_Array *pHistArr, QObject *parent) : QAbstractTableModel(parent)
{
    m_pHistArr = pHistArr;
    m_headings.append(QString("Summary"));
    m_headings.append(QString("Author"));
    m_headings.append(QString("Date"));
    m_pAvMapIt = NULL;
}

GBL_HistoryModel::~GBL_HistoryModel()
{
    cleanupAvatars();
    if (m_pAvMapIt) delete m_pAvMapIt;
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
    for (int i = 0; i < m_pHistArr->size(); i++)
    {
        GBL_History_Item *pHistItem = m_pHistArr->at(i);
        QString sEmail = pHistItem->hist_author_email.toLower();
        if (!m_avatarMap.contains(sEmail))
        {
            UrlPixmap *pUrlpm = new UrlPixmap(this);
            m_avatarMap[sEmail] = pUrlpm;
        }
    }

    if (m_pAvMapIt) delete m_pAvMapIt;
    m_pAvMapIt = new QMapIterator<QString, UrlPixmap*>(m_avatarMap);

    if (m_pAvMapIt->hasNext())
    {
       m_pAvMapIt->next();
       QString sEmail = m_pAvMapIt->key();
       UrlPixmap *pUrlPM = m_pAvMapIt->value();
       QString sUrl = getGravatarUrl(sEmail);
       pUrlPM->loadFromUrl(sUrl);
       connect(pUrlPM, SIGNAL (downloaded()), this, SLOT (avatarDownloaded()));
    }

    layoutChanged();
}

QString GBL_HistoryModel::getGravatarUrl(QString sEmail)
{
    QByteArray baEmail = sEmail.toLocal8Bit();
    QByteArray ba = QCryptographicHash::hash(baEmail, QCryptographicHash::Md5);
    QString sUrl;
    QTextStream(&sUrl) << "https://www.gravatar.com/avatar/" << ba.toHex() << "?d=identicon&s=48";
    //qDebug() << sUrl;
    return sUrl;
}

void GBL_HistoryModel::avatarDownloaded()
{
    if (m_pAvMapIt->hasNext())
    {
       m_pAvMapIt->next();
       QString sEmail = m_pAvMapIt->key();
       UrlPixmap *pUrlPM = m_pAvMapIt->value();
       QString sUrl = getGravatarUrl(sEmail);
       pUrlPM->loadFromUrl(sUrl);
       connect(pUrlPM, SIGNAL (downloaded()), this, SLOT (avatarDownloaded()));
    }
    else
    {
        layoutChanged();
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
    return 3;
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
                return pHistItem->hist_datetime.toString("M/d/yyyy h:m:s ap");
        }
    }
    else if (role == Qt::DecorationRole && index.column() == 1)
    {
        GBL_History_Item *pHistItem = m_pHistArr->at(index.row());
        QString sEmail = pHistItem->hist_author_email.toLower();
        if (m_avatarMap.contains(sEmail))
        {
            UrlPixmap *pUP = m_avatarMap[sEmail];
            return QVariant::fromValue(*(pUP->getSmallPixmap(16)));
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
