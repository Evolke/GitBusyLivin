#ifndef GBL_FILEMODEL_H
#define GBL_FILEMODEL_H

#include <QAbstractItemModel>
#include "gbl_repository.h"
#include <QIcon>

class GBL_FileModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit GBL_FileModel(QObject *parent = Q_NULLPTR);
    ~GBL_FileModel();

    Q_INVOKABLE virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual QModelIndex parent(const QModelIndex &child) const;
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    void cleanFileArray();
    void addFileItem(GBL_File_Item *pFileItem);

signals:

public slots:

private:
    GBL_File_Array *m_pFileArr;
    QVector<QString> m_headings;
    QIcon m_addDocIcon, m_removeDocIcon, m_modifyDocIcon, m_unknownDocIcon;
};

#endif // GBL_FILEMODEL_H
