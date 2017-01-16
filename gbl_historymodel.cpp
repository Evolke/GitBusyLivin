#include "gbl_historymodel.h"

GBL_HistoryModel::GBL_HistoryModel(GBL_History_Array *pHistArr, QObject *parent) : QAbstractItemModel(parent)
{
    m_pHistArr = pHistArr;
    m_headings.append(QString("Summary"));
    m_headings.append(QString("Author"));
    m_headings.append(QString("Date"));

}

GBL_HistoryModel::~GBL_HistoryModel()
{

}

QModelIndex GBL_HistoryModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex GBL_HistoryModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

/*bool GBL_HistoryModel::hasChildren(const QModelIndex &parent) const
{
    return false;
}*/

int GBL_HistoryModel::rowCount(const QModelIndex &parent) const
{
    return m_pHistArr->length();
}

int GBL_HistoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant GBL_HistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) { return QVariant(); }

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
                return pHistItem->hist_datetime.toString();
        }
    }
    return QVariant();
}

QVariant GBL_HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
        return m_headings.at(section);
    /*if (role == Qt::DecorationRole)
        return QVariant::fromValue(services);*/
    return QAbstractItemModel::headerData(section, orientation, role);
}
