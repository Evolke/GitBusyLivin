#include "gbl_refsmodel.h"

#include <qicon>


GBL_RefsModel::GBL_RefsModel(QObject *parent) : QAbstractItemModel(parent)
{
    //m_pRefRoot = new GBL_RefItem(QString(),QString());
    m_pRefRoot = NULL;
}

GBL_RefsModel::~GBL_RefsModel()
{
    //delete m_pRefRoot;
}


QModelIndex GBL_RefsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) { return QModelIndex(); }

    GBL_RefItem *parentItem;

    if (!parent.isValid())
        parentItem = m_pRefRoot;
    else
        parentItem = static_cast<GBL_RefItem*>(parent.internalPointer());

    GBL_RefItem *childItem = parentItem->getChildAt(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();

}

void GBL_RefsModel::setRefRoot(GBL_RefItem *pRef)
{
    m_pRefRoot = pRef;
    layoutChanged();
}

void GBL_RefsModel::reset()
{
    QAbstractItemModel::resetInternalData();

    m_pRefRoot = NULL;
    layoutChanged();
    //m_pRefRoot->cleanup();
    //initRefItems();
}

QModelIndex GBL_RefsModel::parent(const QModelIndex &child) const
{
    GBL_RefItem *childItem = static_cast<GBL_RefItem*>(child.internalPointer());
    GBL_RefItem *parentItem = childItem->getParent();

    if (parentItem == m_pRefRoot)
        return QModelIndex();

    return createIndex(parentItem->index(), 0, parentItem);
}

int GBL_RefsModel::rowCount(const QModelIndex &parent) const
{
    GBL_RefItem *parentItem;

    if (m_pRefRoot == NULL ) { return 0; }

    else if (!parent.isValid())
        parentItem = m_pRefRoot;
    else
        parentItem = static_cast<GBL_RefItem*>(parent.internalPointer());

    return parentItem->getChildCount();

}

int GBL_RefsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QVariant GBL_RefsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    GBL_RefItem *item = static_cast<GBL_RefItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
    {
        return QVariant(item->getName());
    }
    else if (role == Qt::DecorationRole)
    {
        if (item->getIcon())
        {
            QIcon icn(*item->getIcon());
            return (const QIcon)icn;
        }
    }

    return QVariant();
}

QVariant GBL_RefsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    /*if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return m_headings.at(section);
    */

    return QAbstractItemModel::headerData(section, orientation, role);

}

