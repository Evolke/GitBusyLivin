#include "historyview.h"
#include "src/gbl/gbl_historymodel.h"

#include <QDebug>
#include <QScrollBar>
#include <QPainter>

HistoryView::HistoryView(QWidget *parent) : QTableView(parent)
{
    setContentsMargins(0,0,0,0);
    //setMinimumWidth(300);
}

HistoryView::~HistoryView()
{

}

void HistoryView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    int nWidth = width();

    QScrollBar *pSB = verticalScrollBar();
    if (pSB && pSB->isVisible()) nWidth -= pSB->width();

    setColumnWidth(0, nWidth*.1);
    setColumnWidth(1, nWidth*.5);
    setColumnWidth(2, nWidth*.24);
    setColumnWidth(3, nWidth*.15);

    GBL_HistoryModel *pModel = (GBL_HistoryModel*)model();
    pModel->layoutChanged();
}

/*void HistoryView::itemSelectionChanged()
{
    qDebug() << "itemSectionChanged";
}*/


HistoryDelegate::HistoryDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void HistoryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    if (index.column() == 0)
    {
        QSize szCir(7,7);
        int nLineIndent = 10, nLineSpacing = 10;
        QColor active_color(50,50,200, 200);
        QColor second_color(50,200,50,200);

        /*if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());

        if (option.state & QStyle::State_Selected)
    //! [8] //! [9]
            painter->setBrush(option.palette.highlightedText());
        else
    //! [2]
            painter->setBrush(option.palette.text());
        */
        QRect rct = option.rect;
        //painter->drawEllipse(rct);
        QPen pen(active_color);
        pen.setWidth(2);
        painter->setPen(pen);

        painter->drawLine(rct.left() + nLineIndent,rct.top(),rct.left() + nLineIndent,rct.bottom());
        if (index.row() % 2 == 0)
        {
            QRect cirRct(rct.left()+nLineIndent-szCir.width()/2,rct.top()+rct.height()/2-szCir.height()/2,szCir.width(),szCir.height());
            painter->setBrush(QBrush(QColor(active_color)));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(cirRct);
        }

        QPen pen2(second_color);
        pen.setWidth(2);
        painter->setPen(pen2);
        painter->drawLine(rct.left() + nLineIndent+nLineSpacing,rct.top(),rct.left() + nLineIndent+nLineSpacing,rct.bottom());

        if (index.row() % 2 != 0)
        {
            QRect cirRct2(rct.left()+nLineIndent+nLineSpacing-szCir.width()/2,rct.top()+rct.height()/2-szCir.height()/2,szCir.width(),szCir.height());
            painter->setBrush(QBrush(QColor(second_color)));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(cirRct2);
        }
    }
}
