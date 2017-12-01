#ifndef GBL_REFSMODEL_H
#define GBL_REFSMODEL_H

#include <QAbstractItemModel>
#include "gbl_repository.h"

class GBL_RefsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    GBL_RefsModel(QObject *parent = Q_NULLPTR);
    ~GBL_RefsModel();

    Q_INVOKABLE virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual QModelIndex parent(const QModelIndex &child) const;
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    void setRefRoot(GBL_RefItem *pRef);
    void reset();
    GBL_RefItem* getRefRoot() { return m_pRefRoot; }

private:
    GBL_RefItem *m_pRefRoot;
};

#endif // GBL_REFSMODEL_H
