#include "historyview.h"
#include "src/gbl/gbl_historymodel.h"
#include "mainwindow.h"

#include <QDebug>
#include <QScrollBar>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QPalette>
#include <QtMath>
#include <QHeaderView>

HistoryView::HistoryView(QWidget *parent) : QTableView(parent)
{
    setContentsMargins(0,0,0,0);
    //setMinimumWidth(300);
    setAutoFillBackground(false);

    m_pContextMenu = new QMenu(this);

    MainWindow *pMain = MainWindow::getInstance();
    m_pContextMenu->addAction(tr("Create Branch..."),pMain, &MainWindow::onCreateBranch);
    m_bAutoSizeHdr = true;
    m_bPreAutoSizeHdr = false;
    setWordWrap(false);

    connect(horizontalHeader(), &QHeaderView::sectionResized,this, &HistoryView::headerResized);

}

HistoryView::~HistoryView()
{

}

void HistoryView::reset()
{
    QTableView::reset();
    GBL_HistoryModel *pModel = dynamic_cast<GBL_HistoryModel*>(model());
    pModel->reset();

}

void HistoryView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    if (m_bAutoSizeHdr)
    {
        m_bPreAutoSizeHdr = true;

        int nWidth = width();

        QScrollBar *pSB = verticalScrollBar();
        if (pSB && pSB->isVisible()) nWidth -= pSB->width();

        setColumnWidth(0, qFloor(nWidth*.1));
        setColumnWidth(1, qFloor(nWidth*.5));
        setColumnWidth(2, qFloor(nWidth*.25));
        setColumnWidth(3, qFloor(nWidth*.148));

        GBL_HistoryModel *pModel = dynamic_cast<GBL_HistoryModel*>(model());
        pModel->layoutChanged();
        m_bPreAutoSizeHdr = false;

    }
}

void HistoryView::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    int col = columnAt(pos.x());
    int row = rowAt(pos.y());
    //qDebug() << "row:" << row;

    //if (event->button() == Qt::RightButton) return;

    if (col > 0)
    {
        QTableView::mousePressEvent(event);
    }
    else if (col == 0)
    {
        setAutoScroll(false);
        selectRow(row);
        setAutoScroll(true);
    }
}

void HistoryView::contextMenuEvent(QContextMenuEvent *event)
{
    QTableView::contextMenuEvent(event);

    m_pContextMenu->exec(event->globalPos());
}

/*void HistoryView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QTableView::selectionChanged(selected, deselected);

    //qDebug() << "selectionChanged";

    if (!selected.isEmpty())
    {
        QItemSelectionRange first = selected.first();

        //showRow(row);
    }
}*/

void HistoryView::headerResized()
{
    if (!m_bPreAutoSizeHdr)
    {
        m_bAutoSizeHdr = false;
    }
}

HistorySelectionModel::HistorySelectionModel(QAbstractItemModel *model, QObject *parent) :  QItemSelectionModel(model, parent)
{

}


HistoryDelegate::HistoryDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    m_graphColors << QColor(50,50,200/*, 225*/) << QColor(50,200,50/*,200*/) << QColor(200,50,50/*,200*/);
}

void HistoryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QStyledItemDelegate::paint(painter, option, index);
    painter->setRenderHint(QPainter::Antialiasing, true);
    if (index.column() == 0)
    {
        HistoryView *pView = dynamic_cast<HistoryView*>(parent());
        //int nRow = index.row();
        QStyle *pStyle = pView->style();
        QSize szCir(7,7);
        int nLineIndent = 14, nLineSpacing = 10;
        QRect rct = option.rect;
        QPalette pal = option.palette;

        //painter->fillRect(rct,pal.base());
        QItemSelectionModel *pSelMod = pView->selectionModel();

        int nRowCnt = pView->model()->rowCount();
        QRect rowRct = rct;

        QPolygon line;
        for (int i = 0; i < nRowCnt; i++)
        {
            int nRowHt = pView->rowHeight(i);
            //qDebug() << "rowHt:" << nRowHt;
            rowRct.setHeight(nRowHt);
            /*pal.setCurrentColorGroup(QPalette::Normal);
            QBrush rowBr = i % 2 ? pal.alternateBase() : pal.window();

            if (pSelMod->isRowSelected(i, QModelIndex()))
            {
                //painter->fillRect(rowRct, pal.highlight());
                rowBr = pal.highlight();
            }
            painter->fillRect(rowRct, rowBr);*/
            QStyleOptionViewItem newOpt = option;
            newOpt.index = pView->model()->index(i,1);
            if (i % 2)
                newOpt.features |= QStyleOptionViewItem::Alternate;
            else
                newOpt.features &= ~QStyleOptionViewItem::Alternate;
            //newOpt.state = QStyle::State_Enabled;
            if (pSelMod->isRowSelected(i, QModelIndex()))
                newOpt.state |= QStyle::State_Selected;
            else
                newOpt.state &= ~QStyle::State_Selected;

            QPalette::ColorGroup cg = QPalette::Normal;
            newOpt.rect = rowRct;
            newOpt.palette.setCurrentColorGroup(cg);
            pView->style()->drawPrimitive(QStyle::PE_PanelItemViewRow,&newOpt, painter, pView);
            pView->style()->drawPrimitive(QStyle::PE_PanelItemViewItem,&newOpt, painter, pView);

            //qDebug() << "rowRct" << rowRct;

            painter->setBrush(m_graphColors[0]);
            painter->setPen(m_graphColors[0]);
            QRect cirRct(rowRct.left()+nLineIndent - szCir.width()/2,rowRct.top()+rowRct.height()/2-szCir.height()/2, szCir.width(),szCir.height());
            line << QPoint(rowRct.left()+nLineIndent,rowRct.top()+rowRct.height()/2);
            painter->drawEllipse(cirRct);

            rowRct.setTop(rowRct.top()+nRowHt);
        }
        painter->drawPolyline(line);
        //painter->drawEllipse(rct);

            /*QPen pen(m_graphColors[0]);
            pen.setWidth(3);
            painter->setPen(pen);
            painter->drawLine(rct.left() + nLineIndent, rct.top()+rct.height()/2,rct.left()+nLineIndent,rct.bottom());

            QRect cirRct(rct.left()+nLineIndent-szCir.width()/2,rct.top()+rct.height()/2-szCir.height()/2,szCir.width(),szCir.height());
            painter->setBrush(QBrush(m_graphColors[0]));
            painter->drawEllipse(cirRct);
            */

        //painter->drawLine(rct.left() + nLineIndent,rct.top(),rct.left() + nLineIndent,rct.bottom());
        /*if (index.row() % 2 == 0)
        {
            QRect cirRct(rct.left()+nLineIndent-szCir.width()/2,rct.top()+rct.height()/2-szCir.height()/2,szCir.width(),szCir.height());
            painter->setBrush(QBrush(QColor(active_color)));
            //painter->setPen(Qt::NoPen);
            painter->drawEllipse(cirRct);
        }

        QPen pen2(second_color);
        pen.setWidth(2);
        painter->setPen(pen2);
        //painter->drawLine(rct.left() + nLineIndent+nLineSpacing,rct.top(),rct.left() + nLineIndent+nLineSpacing,rct.bottom());

        if (index.row() % 2 != 0)
        {
            QRect cirRct2(rct.left()+nLineIndent+nLineSpacing-szCir.width()/2,rct.top()+rct.height()/2-szCir.height()/2,szCir.width(),szCir.height());
            painter->setBrush(QBrush(QColor(second_color)));
            //painter->setPen(Qt::NoPen);
            painter->drawEllipse(cirRct2);
        }*/
    }
}
