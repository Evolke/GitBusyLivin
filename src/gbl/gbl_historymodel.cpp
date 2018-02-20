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


GBL_HistoryModel::GBL_HistoryModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_pHistArr = new GBL_History_Array;

    m_headings.append(tr("Graph"));
    m_headings.append(tr("Summary"));
    m_headings.append(tr("Author"));
    m_headings.append(tr("Date"));
    m_colMap["graph"] = 0;
    m_colMap["summary"] = 1;
    m_colMap["author"] = 2;
    m_colMap["date"] = 3;
}

GBL_HistoryModel::~GBL_HistoryModel()
{
    cleanupHistory();


    delete m_pHistArr;
}

void GBL_HistoryModel::cleanupHistory()
{
    if (m_pHistArr)
    {
        for (int i = 0; i < m_pHistArr->size(); i++)
        {
            GBL_History_Item *pHI = m_pHistArr->at(i);
            delete pHI;
        }

        m_pHistArr->clear();
    }

    m_histMap.clear();
}


void GBL_HistoryModel::reset()
{
    QAbstractItemModel::resetInternalData();

    cleanupHistory();
}

void GBL_HistoryModel::addHistoryItem(GBL_History_Item *pHistItem)
{
    GBL_History_Item *pNewHistItem = new GBL_History_Item;
    pNewHistItem->hist_author = pHistItem->hist_author;
    pNewHistItem->hist_author_email = pHistItem->hist_author_email;
    pNewHistItem->hist_datetime = pHistItem->hist_datetime;
    pNewHistItem->hist_message = pHistItem->hist_message;
    pNewHistItem->hist_oid = pHistItem->hist_oid;
    pNewHistItem->hist_parents = pHistItem->hist_parents;
    pNewHistItem->hist_summary = pHistItem->hist_summary;

    m_pHistArr->append(pNewHistItem);
    m_histMap.insert(pHistItem->hist_oid, m_pHistArr->size()-1);
}

void GBL_HistoryModel::historyUpdated()
{
    if (m_pHistArr && !m_pHistArr->isEmpty())
    {
        MainWindow *pMain = MainWindow::getInstance();

        for (int i = 0; i < m_pHistArr->size(); i++)
        {
            GBL_History_Item *pHistItem = m_pHistArr->at(i);
            m_histMap.insert(pHistItem->hist_oid, i);
            QString sEmail = pHistItem->hist_author_email.toLower();
            pMain->addAvatar(sEmail);
            /*else
            {
                qDebug() << "avatarMap.contains = true";
            }*/
        }

        //qDebug() << "avatarMap.size:" << m_avatarMap.size();

        pMain->startAvatarDownload();
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
    if (m_pHistArr && m_pHistArr->length()) return m_headings.size();

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
        if  (index.column() == m_colMap["summary"])
            return pHistItem->hist_summary;
        else if (index.column() == m_colMap["author"])
            return pHistItem->hist_author;
        else if (index.column() == m_colMap["date"])
            return pHistItem->hist_datetime.toString("M/d/yyyy h:mm ap");

    }
    else if (role == Qt::DecorationRole)
    {
        if (index.column() == m_colMap["author"])
        {
            GBL_History_Item *pHistItem = m_pHistArr->at(index.row());
            QString sEmail = pHistItem->hist_author_email;
            MainWindow *pMain = MainWindow::getInstance();
            QPixmap *pPixMap = pMain->getAvatar(sEmail,true);

            if (pPixMap) return QVariant::fromValue(*(pPixMap));

        }
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
