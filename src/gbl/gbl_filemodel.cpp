#include "gbl_filemodel.h"
#include <QDebug>
#include <QPixmap>
#include <QFileInfo>
#include <QFileIconProvider>

GBL_FileModel::GBL_FileModel(QObject *parent) : QAbstractItemModel(parent),
    m_addDocIcon(QPixmap(":/images/add_doc_icon.png")), m_removeDocIcon(QPixmap(":/images/remove_doc_icon.png")),
    m_modifyDocIcon(QPixmap(":/images/modify_doc_icon.png")), m_unknownDocIcon(QPixmap(":/images/unknown_doc_icon.png"))
{
    m_pFileArr = new GBL_File_Array;
    m_pHistItem = NULL;
    m_headings.append(QString("File"));
    m_headings.append(QString("Path"));
    m_pFileTreeRoot = new GBL_FileTreeItem("");
    m_nViewType = GBL_FILETREE_VIEW_TYPE_LIST;
}

GBL_FileModel::~GBL_FileModel()
{
    delete m_pFileArr;
    delete m_pFileTreeRoot;
}

void GBL_FileModel::cleanUp()
{
    QAbstractItemModel::resetInternalData();

    if (m_pFileArr->size())
    {
        for (int i = 0; i < m_pFileArr->size(); i++)
        {
            GBL_File_Item *pFI = m_pFileArr->at(i);
            delete pFI;
        }
    }

    m_pFileArr->clear();
    m_pFileTreeRoot->cleanup();
    layoutChanged();
}

void GBL_FileModel::addFileItem(GBL_File_Item *pFileItem)
{
    m_pFileArr->append(pFileItem);
    layoutChanged();
}

void GBL_FileModel::setFileArray(GBL_File_Array *pArr)
{
    cleanUp();

    for (int i = 0; i < pArr->size(); i++)
    {
        GBL_File_Item *pItem = pArr->at(i);
        GBL_File_Item *pNewItem = new GBL_File_Item();
        pNewItem->file_name = pItem->file_name;
        pNewItem->file_oid = pItem->file_oid;
        pNewItem->status = pItem->status;
        pNewItem->sub_dir = pItem->sub_dir;
        m_pFileArr->append(pNewItem);
        if (pItem->sub_dir == ".")
        {
            m_pFileTreeRoot->addChild(new GBL_FileTreeItem("",i,m_pFileTreeRoot));
        }
        else
        {
            QStringList dirParts = pItem->sub_dir.split("/");
            GBL_FileTreeItem *pParent = m_pFileTreeRoot;
            for (int j = 0; j < dirParts.size(); j++)
            {
                QString sfldr = dirParts.at(j);

                if (sfldr.isEmpty()) continue;

                GBL_FileTreeItem *pNewParent = pParent->findChild(sfldr);
                if (pNewParent == NULL)
                {
                    pNewParent = new GBL_FileTreeItem(sfldr,-1,pParent);
                    pParent->addChild(pNewParent);
                }

                pParent = pNewParent;
            }

            pParent->addChild(new GBL_FileTreeItem("",i,pParent));


        }
    }

    layoutChanged();
}

GBL_File_Item* GBL_FileModel::getFileItemAt(int index)
{
    if (m_pFileArr && index >= 0 && index < m_pFileArr->length())
    {
        return m_pFileArr->at(index);
    }
    return NULL;
}

GBL_File_Item* GBL_FileModel::getFileItemFromModelIndex(const QModelIndex mi)
{
    switch (m_nViewType)
    {
        case GBL_FILETREE_VIEW_TYPE_LIST:
            return getFileItemAt(mi.row());
            break;

        case GBL_FILETREE_VIEW_TYPE_TREE:
            {
                GBL_FileTreeItem *pTreeItem = static_cast<GBL_FileTreeItem*>(mi.internalPointer());
                int nIndex = pTreeItem->getArrayIndex();
                if (nIndex >= 0)
                {
                    return m_pFileArr->at(nIndex);
                }
            }
            break;
    }

    return NULL;
}

void GBL_FileModel::setHistoryItem(GBL_History_Item *pHistItem)
{
    m_pHistItem = pHistItem;
}

GBL_History_Item* GBL_FileModel::getHistoryItem()
{
    return m_pHistItem;
}

QModelIndex GBL_FileModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) { return QModelIndex(); }

    switch (m_nViewType)
    {
        case GBL_FILETREE_VIEW_TYPE_LIST:
            return createIndex(row, column);
            break;

        case GBL_FILETREE_VIEW_TYPE_TREE:
            {
                GBL_FileTreeItem *parentItem;

                if (!parent.isValid())
                    parentItem = m_pFileTreeRoot;
                else
                    parentItem = static_cast<GBL_FileTreeItem*>(parent.internalPointer());

                GBL_FileTreeItem *childItem = parentItem->getChildAt(row);
                if (childItem)
                    return createIndex(row, column, childItem);
                else
                    return QModelIndex();
            }
            break;
    }

}

QModelIndex GBL_FileModel::parent(const QModelIndex &child) const
{
    switch (m_nViewType)
    {
        case GBL_FILETREE_VIEW_TYPE_LIST:
            return QModelIndex();
            break;

        case GBL_FILETREE_VIEW_TYPE_TREE:
            {
                GBL_FileTreeItem *childItem = static_cast<GBL_FileTreeItem*>(child.internalPointer());
                GBL_FileTreeItem *parentItem = childItem->getParent();

                if (parentItem == m_pFileTreeRoot)
                    return QModelIndex();

                return createIndex(parentItem->index(), 0, parentItem);
            }
            break;
    }
}

int GBL_FileModel::rowCount(const QModelIndex &parent) const
{
    switch (m_nViewType)
    {
        case GBL_FILETREE_VIEW_TYPE_LIST:
            if (parent.isValid()) return 0;

            return m_pFileArr->size();
            break;

        case GBL_FILETREE_VIEW_TYPE_TREE:
            {
                GBL_FileTreeItem *parentItem;

                if (m_pFileTreeRoot == NULL ) { return 0; }

                else if (!parent.isValid())
                    parentItem = m_pFileTreeRoot;
                else
                    parentItem = static_cast<GBL_FileTreeItem*>(parent.internalPointer());

                return parentItem->getChildCount();
            }
            break;
    }
}

int GBL_FileModel::columnCount(const QModelIndex &parent) const
{
    switch (m_nViewType)
    {
        case GBL_FILETREE_VIEW_TYPE_LIST:
            return m_headings.length();
            break;
        case GBL_FILETREE_VIEW_TYPE_TREE:
            return 1;
            break;
    }

}

QVariant GBL_FileModel::data(const QModelIndex &index, int role) const
{
    if (index.row() > (m_pFileArr->length()-1) || index.row() < 0 ) { return QVariant(); }

    switch (m_nViewType)
    {
        case GBL_FILETREE_VIEW_TYPE_LIST:
            if (!index.isValid()) { return QVariant(); }

            if (role == Qt::DisplayRole)
            {
                GBL_File_Item *pFileItem = m_pFileArr->at(index.row());
                switch (index.column())
                {
                    case 0:
                        return pFileItem->file_name;
                    case 1:
                        return pFileItem->sub_dir;
                }
            }
            else if (role == Qt::DecorationRole && index.column() == 0)
            {
                GBL_File_Item *pFileItem = m_pFileArr->at(index.row());
                switch (pFileItem->status)
                {
                    case GBL_FILE_STATUS_ADDED:
                        return QVariant::fromValue(m_addDocIcon);
                        break;

                    case GBL_FILE_STATUS_DELETED:
                        return QVariant::fromValue(m_removeDocIcon);
                        break;

                    case GBL_FILE_STATUS_MODIFIED:
                        return QVariant::fromValue(m_modifyDocIcon);
                        break;

                    case GBL_FILE_STATUS_SYSTEM:
                        {
                            QString sPath = m_sRepoPath;
                            sPath += "/" + pFileItem->sub_dir + pFileItem->file_name;
                            QFileIconProvider icnp;
                            //qDebug() << sPath;
                            return QVariant::fromValue(icnp.icon(QFileInfo(sPath)));
                        }
                        break;

                    default:
                        return QVariant::fromValue(m_unknownDocIcon);
                        break;
                }
            }

            break;

        case GBL_FILETREE_VIEW_TYPE_TREE:
            {
                GBL_FileTreeItem *item = static_cast<GBL_FileTreeItem*>(index.internalPointer());
                int nIndex = item->getArrayIndex();
                if (role == Qt::DisplayRole)
                {
                    QString sName;
                    if (nIndex >= 0)
                    {
                        GBL_File_Item *pFileItem = m_pFileArr->at(nIndex);
                        sName = pFileItem->file_name;
                    }
                    else
                    {
                        sName = item->getFolder();
                    }
                    return QVariant(sName);
                }
                else if (role == Qt::DecorationRole)
                {
                    QFileIconProvider icnp;
                    if (nIndex >= 0)
                    {
                        GBL_File_Item *pFileItem = m_pFileArr->at(nIndex);
                        QString sPath = m_sRepoPath;
                        sPath += "/" + pFileItem->sub_dir + pFileItem->file_name;
                        QFileInfo fi(sPath);
                        QIcon icn = fi.isFile() ? icnp.icon(fi) : icnp.icon(QFileIconProvider::File);
                        return QVariant::fromValue(icn);
                    }
                    else
                    {
                        return QVariant::fromValue(icnp.icon(QFileIconProvider::Folder));
                    }
                }
            }
            break;
    }

    return QVariant();

}

QVariant GBL_FileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (m_nViewType)
    {
        case GBL_FILETREE_VIEW_TYPE_LIST:
            if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
                return m_headings.at(section);

        case GBL_FILETREE_VIEW_TYPE_TREE:
            return QAbstractItemModel::headerData(section, orientation, role);
            break;
    }

}



GBL_FileTreeItem::GBL_FileTreeItem(QString sFolder, int nIndex, GBL_FileTreeItem *parent)
{
    m_sFolder = sFolder;
    m_pParent = parent;
    m_nIndex = nIndex;
}

GBL_FileTreeItem::~GBL_FileTreeItem()
{
    cleanup();
}

void GBL_FileTreeItem::cleanup()
{
    for (int i = 0; i < m_children.size(); i++)
    {
        GBL_FileTreeItem *pTreeItem = m_children.at(i);
        delete pTreeItem;
    }

    m_children.clear();
}

void GBL_FileTreeItem::addChild(GBL_FileTreeItem *pFileTreeItem)
{
    m_children.append(pFileTreeItem);
}

GBL_FileTreeItem* GBL_FileTreeItem::findChild(QString sFolder)
{
    for (int i = 0; i < m_children.size(); i++)
    {
        GBL_FileTreeItem *pTreeItem = m_children.at(i);
        if (pTreeItem->getFolder() == sFolder) return pTreeItem;
    }

    return NULL;
}

GBL_FileTreeItem* GBL_FileTreeItem::getChildAt(int index)
{
    if (index >= 0 && index < m_children.size())
    {
        return m_children.at(index);
    }

    return NULL;
}

int GBL_FileTreeItem::index()
{
    if (m_pParent)
    {
        return m_pParent->getChildrenList()->indexOf(this);
    }

    return -1;
}


