#include "gbl_filemodel.h"
#include <QDebug>
#include <QPixmap>

GBL_FileModel::GBL_FileModel(QObject *parent) : QAbstractItemModel(parent),
    m_addDocIcon(QPixmap(":/images/add_doc_icon.png")), m_removeDocIcon(QPixmap(":/images/remove_doc_icon.png")),
    m_modifyDocIcon(QPixmap(":/images/modify_doc_icon.png")), m_unknownDocIcon(QPixmap(":/images/unknown_doc_icon.png"))
{
    m_pFileArr = new GBL_File_Array;
    m_pHistItem = NULL;
    m_headings.append(QString("File"));
    m_headings.append(QString("Path"));

}

GBL_FileModel::~GBL_FileModel()
{
    delete m_pFileArr;
}

void GBL_FileModel::cleanFileArray()
{
    if (m_pFileArr->size())
    {
        for (int i = 0; i < m_pFileArr->size(); i++)
        {
            GBL_File_Item *pFI = m_pFileArr->at(i);
            delete pFI;
        }
    }

    m_pFileArr->clear();
    layoutChanged();
}

void GBL_FileModel::addFileItem(GBL_File_Item *pFileItem)
{
    m_pFileArr->append(pFileItem);
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

    return createIndex(row, column);

}

QModelIndex GBL_FileModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int GBL_FileModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return m_pFileArr->size();
}

int GBL_FileModel::columnCount(const QModelIndex &parent) const
{
    return m_headings.length();
}

QVariant GBL_FileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) { return QVariant(); }
    if (index.row() > (m_pFileArr->length()-1) || index.row() < 0 ) { return QVariant(); }

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
            case GIT_DELTA_ADDED:
                return QVariant::fromValue(m_addDocIcon);
                break;
            case GIT_DELTA_DELETED:
                return QVariant::fromValue(m_removeDocIcon);
                break;
            case GIT_DELTA_MODIFIED:
                return QVariant::fromValue(m_modifyDocIcon);
                break;
            default:
                return QVariant::fromValue(m_unknownDocIcon);
        }
    }
    return QVariant();
}

QVariant GBL_FileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return m_headings.at(section);

    return QAbstractItemModel::headerData(section, orientation, role);

}

