#include "bookmarksdock.h"
#include "urlpixmap.h"
#include "mainwindow.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QToolBar>
#include <QDebug>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMimeData>
#include <QMessageBox>

BookmarksDock::BookmarksDock(const QString &title, QWidget *parent, Qt::WindowFlags flags) : QDockWidget(title, parent,flags)
{
    m_pBmSa = new BookmarksScrollArea(this);
    setWidget(m_pBmSa);

    BookmarkTreeView *pTreeView = m_pBmSa->getTreeView();

    connect(pTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &BookmarksDock::bookmarkSelectionChanged);

}

void BookmarksDock::addBookmark(QString sName,QString sPath)
{
    BookmarkTreeView *pTreeView = m_pBmSa->getTreeView();
    BookmarksModel *pModel = (BookmarksModel*)pTreeView->model();

    pModel->addBookmark(sName,sPath);
}

void BookmarksDock::deleteBookmark()
{
    if (QMessageBox::question(this, tr("Delete Bookmark?"), tr("Are you sure you want to delete the bookmark?")) == QMessageBox::Yes)
    {
        BookmarkTreeView *pTreeView = m_pBmSa->getTreeView();
        BookmarksModel *pModel = (BookmarksModel*)pTreeView->model();

        QModelIndexList mil = pTreeView->selectionModel()->selectedRows();
        for (int i=0; i < mil.size(); i++)
        {
            pModel->deleteIndex(mil.at(i));
        }

        m_pBmSa->getButtonBar()->enableButton(false, DELETE_BOOKMARK_ID);
    }
}

BookmarksModel* BookmarksDock::getTreeModel()
{
   return (BookmarksModel*)m_pBmSa->getTreeView()->model();
}

QJsonDocument* BookmarksDock::getJDoc()
{
    BookmarkTreeView *pTree = m_pBmSa->getTreeView();
    BookmarksModel *pModel = (BookmarksModel*)pTree->model();

    return pModel->getJDoc();
}

void BookmarksDock::enableAdd(bool bEnable)
{
    m_pBmSa->getButtonBar()->enableButton(bEnable, ADD_BOOKMARK_ID);

}

void BookmarksDock::bookmarkSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    QModelIndexList mil = selected.indexes();

    m_pBmSa->getButtonBar()->enableButton(mil.size() > 0, DELETE_BOOKMARK_ID);

}

/**
 * @brief BookmarksScrollArea::BookmarksScrollArea
 * @param parent
 */
BookmarksScrollArea::BookmarksScrollArea(QWidget *parent) : QScrollArea(parent)
{
    setViewportMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
    m_pBMView = new BookmarkTreeView(this);
    m_pBMView->setModel(new BookmarksModel(m_pBMView));
    //m_pBMView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pBtnBar = new BookmarksButtonBar(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pBtnBar, Qt::AlignVCenter);
    mainLayout->addWidget(m_pBMView);
    setLayout(mainLayout);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(2);

}

BookmarkTreeView::BookmarkTreeView(QWidget *parent) : QTreeView(parent)
{
    setHeaderHidden(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setIndentation(0);

    MainWindow *pMain = MainWindow::getInstance();
    connect(this, &BookmarkTreeView::doubleClicked, pMain, &MainWindow::openBookmarkDoubleClick);
}

void BookmarkTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(BookmarkTreeView::bookmarkMimeType()))
        event->accept();
    else
        event->ignore();
}

void BookmarkTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(BookmarkTreeView::bookmarkMimeType())) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        event->ignore();
    }
}

void BookmarkTreeView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(BookmarkTreeView::bookmarkMimeType())) {
//        QByteArray pieceData = event->mimeData()->data(BookmarkTreeView::bookmarkMimeType());
//        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
//        QPixmap pixmap;
//        QPoint location;
//        dataStream >> pixmap >> location;

//        addPiece(pixmap, location);

        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        event->ignore();
    }
}


void BookmarkTreeView::startDrag(Qt::DropActions /*supportedActions*/)
{
//    QListWidgetItem *item = currentItem();

//    QByteArray itemData;
//    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
//    QPixmap pixmap = qvariant_cast<QPixmap>(item->data(Qt::UserRole));
//    QPoint location = item->data(Qt::UserRole+1).toPoint();

//    dataStream << pixmap << location;

//    QMimeData *mimeData = new QMimeData;
//    mimeData->setData(PiecesList::puzzleMimeType(), itemData);

//    QDrag *drag = new QDrag(this);
//    drag->setMimeData(mimeData);
//    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
//    drag->setPixmap(pixmap);

//    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
//        delete takeItem(row(item));
}



BookmarksModel::BookmarksModel(QObject *parent) : QAbstractItemModel(parent)
{
    m_pBMRootItem = new BookmarkItem("","");
    m_pBMDoc = new QJsonDocument();

    UrlPixmap svgpix(NULL);
    MainWindow *pMain = MainWindow::getInstance();
    QToolBar *pToolBar = pMain->getToolBar();
    QPalette pal = pToolBar->palette();
    QColor txtClr = pal.color(QPalette::Text);
    QString sBorderClr = txtClr.name(QColor::HexRgb);
    svgpix.loadSVGResource(":/images/git_logo_16.svg", sBorderClr, QSize(16,16));

    m_icon.addPixmap(*svgpix.getSmallPixmap(16));
}

BookmarksModel::~BookmarksModel()
{
    delete m_pBMRootItem;
    delete m_pBMDoc;
}

void BookmarksModel::addBookmark(GBL_String sName, GBL_String sPath)
{
    m_pBMRootItem->addChild(sName,sPath);

    layoutChanged();
}

QJsonDocument* BookmarksModel::getJDoc()
{
    bookmarkList *pChildren = m_pBMRootItem->getChildrenList();

    QJsonArray jArr;
    BookmarkItem *pChild;
    for (int i=0; i < pChildren->size(); i++)
    {
        pChild = pChildren->at(i);
        QJsonObject jChild;
        jChild.insert("name", QJsonValue(pChild->getName()));
        jChild.insert("path", QJsonValue(pChild->getPath()));
        jArr.append(QJsonValue(jChild));
    }

    m_pBMDoc->setArray(jArr);
    return m_pBMDoc;
}

void BookmarksModel::readBookmarkData(QByteArray* pData)
{
    *m_pBMDoc = QJsonDocument::fromJson(*pData);
    if (m_pBMDoc->isArray())
    {
        QJsonArray jArr = m_pBMDoc->array();
        m_pBMRootItem->cleanup();
        for (int i=0; i < jArr.size(); i++)
        {
            QJsonObject jChild = jArr.at(i).toObject();
            QString sName = jChild.value("name").toString();
            QString sPath = jChild.value("path").toString();
            m_pBMRootItem->addChild(sName,sPath);
        }

    }
}

void BookmarksModel::deleteIndex(const QModelIndex &index)
{
    beginResetModel();

    QModelIndex parentIndex = index.parent();
    if (parentIndex.isValid())
    {
        BookmarkItem *parentItem = static_cast<BookmarkItem*>(parentIndex.internalPointer());
        parentItem->deleteChild(index.row());
    }
    else
    {
        m_pBMRootItem->deleteChild(index.row());
    }

    layoutChanged();
    endResetModel();
}

QModelIndex BookmarksModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) { return QModelIndex(); }

    BookmarkItem *parentItem;

    if (!parent.isValid())
        parentItem = m_pBMRootItem;
    else
        parentItem = static_cast<BookmarkItem*>(parent.internalPointer());

    BookmarkItem *childItem = parentItem->getChildAt(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex BookmarksModel::parent(const QModelIndex &child) const
{
    BookmarkItem *childItem = static_cast<BookmarkItem*>(child.internalPointer());
    BookmarkItem *parentItem = childItem->getParent();

    if (parentItem == m_pBMRootItem)
        return QModelIndex();

    return createIndex(parentItem->index(), 0, parentItem);
}

int BookmarksModel::rowCount(const QModelIndex &parent) const
{
    BookmarkItem *parentItem;

    if (m_pBMRootItem->getChildCount() == 0 ) { return 0; }

    else if (!parent.isValid())
        parentItem = m_pBMRootItem;
    else
        parentItem = static_cast<BookmarkItem*>(parent.internalPointer());

    return parentItem->getChildCount();
}

int BookmarksModel::columnCount(const QModelIndex &parent) const
{
    return 1;//m_headings.length();
}

QVariant BookmarksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    BookmarkItem *item = static_cast<BookmarkItem*>(index.internalPointer());

    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant(item->getName());
            break;

        case Qt::DecorationRole:
            return (const QIcon)m_icon;
            break;

        case Qt::ToolTipRole:
            return QVariant(item->getPath());
        break;

        default:
            return QVariant();
    }

}

QVariant BookmarksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return m_headings.at(section);

    return QVariant();
}


BookmarksButtonBar::BookmarksButtonBar(QWidget *parent) : QFrame(parent)
{
    setContentsMargins(0,0,0,0);
    m_pAddBMBtn = new QToolButton(this);
    m_pAddBMBtn->setText(tr("Add Bookmark"));
    m_pAddBMBtn->setToolTip(tr("Add Bookmark"));

    m_pDelBMBtn = new QToolButton(this);
    m_pDelBMBtn->setText(tr("Delete Bookmark"));
    m_pDelBMBtn->setToolTip(tr("Delete Bookmark"));

    setMaximumHeight(16);

    UrlPixmap svgpix(NULL);
    MainWindow *pMain = MainWindow::getInstance();
    QToolBar *pToolBar = pMain->getToolBar();
    QPalette pal = pToolBar->palette();
    QColor txtClr = pal.color(QPalette::Text);
    QString sBorderClr = txtClr.name(QColor::HexRgb);
    svgpix.loadSVGResource(":/images/add_all_icon.svg", sBorderClr, QSize(16,16));

    m_pAddBMBtn->setDisabled(true);
    m_pAddBMBtn->setIcon(QIcon(*svgpix.getSmallPixmap(16)));
    connect(m_pAddBMBtn,&QToolButton::clicked, pMain, &MainWindow::addBookmark);

    m_pDelBMBtn->setDisabled(true);
    svgpix.loadSVGResource(":/images/unstage_all_icon.svg", sBorderClr, QSize(16,16));
    m_pDelBMBtn->setIcon(QIcon(*svgpix.getSmallPixmap(16)));

    BookmarksDock *pDock = (BookmarksDock*)parent->parent();
    connect(m_pDelBMBtn,&QToolButton::clicked, pDock, &BookmarksDock::deleteBookmark);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_pAddBMBtn, Qt::AlignVCenter);
    mainLayout->addWidget(m_pDelBMBtn, Qt::AlignVCenter);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setMargin(1);
    mainLayout->setSpacing(2);
    setLayout(mainLayout);

}

void BookmarksButtonBar::enableButton(bool bEnable, int buttonID)
{
    switch (buttonID)
    {
        case ADD_BOOKMARK_ID:
            m_pAddBMBtn->setEnabled(bEnable);
            break;
        case DELETE_BOOKMARK_ID:
            m_pDelBMBtn->setEnabled(bEnable);
            break;
    }
}

BookmarkItem::BookmarkItem(QString sName, QString sPath, BookmarkItem *pParent)
{
    m_sName = sName;
    m_sPath = sPath;
    m_pParent = pParent;
}

BookmarkItem::~BookmarkItem()
{
    cleanup();
}

void BookmarkItem::cleanup()
{
    for (int i = 0; i < m_children.size(); i++)
    {
        BookmarkItem *pBmItem = m_children.at(i);
        delete pBmItem;
    }

    m_children.clear();

}

BookmarkItem* BookmarkItem::getChildAt(int index)
{
    if (index >= 0 && index < m_children.size())
    {
        return m_children.at(index);
    }

    return NULL;
}

void BookmarkItem::deleteChild(int index)
{
    if (index >= 0 && index < m_children.size())
    {
        BookmarkItem *pBMItem = m_children.at(index);
        m_children.removeAt(index);
        delete pBMItem;
    }
}

void BookmarkItem::addChild(QString sName, QString sPath)
{
    BookmarkItem *pChild = new BookmarkItem(sName, sPath, this);
    m_children.append(pChild);
}

int BookmarkItem::index()
{
    if (m_pParent)
    {
        return m_pParent->getChildrenList()->indexOf(this);
    }

    return -1;
}

