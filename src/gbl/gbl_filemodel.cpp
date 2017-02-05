#include "gbl_filemodel.h"
#include <QDebug>

GBL_FileModel::GBL_FileModel(QObject *parent) : QAbstractItemModel(parent)
{
    m_pFileArr = new GBL_File_Array;
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
    qDebug() << "filearray_size:" << m_pFileArr->size();
    layoutChanged();
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
    return QVariant();
}

QVariant GBL_FileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return m_headings.at(section);
     /*if (role == Qt::DecorationRole)
        return QVariant::fromValue(services);*/
    return QAbstractItemModel::headerData(section, orientation, role);

}

