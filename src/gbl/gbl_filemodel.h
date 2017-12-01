#ifndef GBL_FILEMODEL_H
#define GBL_FILEMODEL_H

#define GBL_FILETREE_VIEW_TYPE_LIST 1
#define GBL_FILETREE_VIEW_TYPE_TREE 2

#include <QAbstractItemModel>
#include "gbl_repository.h"
#include <QIcon>

QT_BEGIN_NAMESPACE
class GBL_FileTreeItem;
QT_END_NAMESPACE

typedef QList<GBL_FileTreeItem*> GBL_FileTreeItem_list;

class GBL_FileTreeItem
{
public:
    GBL_FileTreeItem(QString sFolder, int nIndex=-1, GBL_FileTreeItem *parent=NULL);
    ~GBL_FileTreeItem();
    void addChild(GBL_FileTreeItem *pTreeItem);
    GBL_FileTreeItem* findChild(QString sFolder);
    GBL_FileTreeItem* getParent() { return m_pParent; }
    GBL_FileTreeItem* getChildAt(int index);
    QString getFolder() { return m_sFolder; }
    int getArrayIndex() { return m_nIndex; }
    int getChildCount() { return m_children.size(); }
    GBL_FileTreeItem_list* getChildrenList() { return &m_children; }
    int index();
    void cleanup();


private:
    GBL_FileTreeItem_list m_children;
    GBL_FileTreeItem *m_pParent;
    QString m_sFolder;
    int m_nIndex;
};

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

    void cleanUp();
    void addFileItem(GBL_File_Item *pFileItem);
    void setFileArray(GBL_File_Array *pArr);
    GBL_File_Array* getFileArray() { return m_pFileArr; }
    GBL_File_Item* getFileItemAt(int index);
    GBL_File_Item* getFileItemFromModelIndex(const QModelIndex mi);
    void setHistoryItem(GBL_History_Item *pHistItem);
    GBL_History_Item* getHistoryItem();
    void setRepoPath(const QString &path) { m_sRepoPath = path; }
    void setViewType(int nType) { m_nViewType = nType; }

signals:

public slots:

private:
    GBL_File_Array *m_pFileArr;
    GBL_History_Item *m_pHistItem;
    QVector<QString> m_headings;
    QIcon m_addDocIcon, m_removeDocIcon, m_modifyDocIcon, m_unknownDocIcon;
    QString m_sRepoPath;
    GBL_FileTreeItem *m_pFileTreeRoot;
    int m_nViewType;
};

#endif // GBL_FILEMODEL_H
