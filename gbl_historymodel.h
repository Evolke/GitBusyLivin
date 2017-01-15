#ifndef GBL_HISTORYMODEL_H
#define GBL_HISTORYMODEL_H

#include <QAbstractItemModel>
#include "gbl_repository.h"

class GBL_HistoryModel : public QAbstractItemModel
{
public:
    GBL_HistoryModel(GBL_History_Array *pHistArr, QObject *parent = Q_NULLPTR);
    ~GBL_HistoryModel();

    Q_INVOKABLE virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual QModelIndex parent(const QModelIndex &child) const;
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

 private:
    GBL_History_Array *m_pHistArr;
    QVector<QString> m_headings;
};

#endif // GBL_HISTORYMODEL_H
