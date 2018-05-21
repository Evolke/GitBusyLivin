#ifndef BOOKMARKSDOCK_H
#define BOOKMARKSDOCK_H

#include "src/gbl/gbl_string.h"

#include <QDockWidget>
#include <QTreeView>
#include <QScrollArea>
#include <QAbstractItemModel>
#include <QList>

#define ADD_BOOKMARK_ID 1
#define DELETE_BOOKMARK_ID 2

QT_BEGIN_NAMESPACE
class QToolButton;
class BookmarkItem;
class QJsonDocument;
class QByteArray;
QT_END_NAMESPACE

typedef QList<BookmarkItem*> bookmarkList;

class BookmarkItem
{
public:
    BookmarkItem(QString sName, QString sPath, BookmarkItem *pParent = NULL);
    ~BookmarkItem();

    void cleanup();

    QString getName() { return m_sName; }
    QString getPath() { return m_sPath; }
    BookmarkItem* getParent() { return m_pParent; }
    BookmarkItem* getChildAt(int index);
    int getChildCount() { return m_children.size(); }
    bookmarkList* getChildrenList() { return &m_children; }
    void addChild(QString sName, QString sPath);
    int index();
    void deleteChild(int index);

private:
    QString m_sName;
    QString m_sPath;

    bookmarkList m_children;
    BookmarkItem *m_pParent;
};

class BookmarksModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit BookmarksModel(QObject *parent = Q_NULLPTR);
    ~BookmarksModel();

    void addBookmark(GBL_String sName, GBL_String sPath);
    QJsonDocument* getJDoc();
    void readBookmarkData(QByteArray* pData);
    void deleteIndex(const QModelIndex &index);

    Q_INVOKABLE virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual QModelIndex parent(const QModelIndex &child) const;
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
private:
    QVector<QString> m_headings;
    BookmarkItem *m_pBMRootItem;
    QJsonDocument *m_pBMDoc;
    QIcon m_icon;

};

class BookmarkTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit BookmarkTreeView(QWidget *parent = Q_NULLPTR);

    static QString bookmarkMimeType() { return QStringLiteral("application/bookmark"); }

signals:

public slots:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;


};

class BookmarksButtonBar : public QFrame
{
    Q_OBJECT
public:
    explicit BookmarksButtonBar(QWidget *parent = Q_NULLPTR);
    void enableButton(bool bEnable, int buttonID);

signals:

public slots:

private:
    QToolButton *m_pAddBMBtn, *m_pDelBMBtn;
};


class BookmarksScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit BookmarksScrollArea(QWidget *parent = Q_NULLPTR);

    BookmarkTreeView* getTreeView() { return m_pBMView; }
    BookmarksButtonBar* getButtonBar() { return m_pBtnBar; }

signals:

public slots:

private:
    BookmarkTreeView *m_pBMView;
    BookmarksButtonBar *m_pBtnBar;

};

class BookmarksDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit BookmarksDock(const QString &title, QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());

    void addBookmark(QString sName, QString sPath);
    void enableAdd(bool bEnable);
    QJsonDocument* getJDoc();
    BookmarksModel *getTreeModel();

signals:

public slots:
    void bookmarkSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void deleteBookmark();

private:
    BookmarksScrollArea *m_pBmSa;
};

#endif // BOOKMARKSDOCK_H

