#include "historyview.h"
#include <QDebug>
#include <QScrollBar>

HistoryView::HistoryView(QWidget *parent) : QTableView(parent)
{
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

    setColumnWidth(0, nWidth*.6);
    setColumnWidth(1, nWidth*.24);
    setColumnWidth(2, nWidth*.15);
}

/*void HistoryView::itemSelectionChanged()
{
    qDebug() << "itemSectionChanged";
}*/
