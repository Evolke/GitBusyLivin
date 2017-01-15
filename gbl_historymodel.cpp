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
    return QModelIndex();
}

QModelIndex GBL_HistoryModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int GBL_HistoryModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int GBL_HistoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant GBL_HistoryModel::data(const QModelIndex &index, int role) const
{

}

QVariant GBL_HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
        return m_headings.at(section);
    /*if (role == Qt::DecorationRole)
        return QVariant::fromValue(services);*/
    return QAbstractItemModel::headerData(section, orientation, role);
}
