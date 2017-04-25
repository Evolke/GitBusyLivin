#include "historyview.h"
#include "src/gbl/gbl_historymodel.h"

#include <QDebug>
#include <QScrollBar>

HistoryView::HistoryView(QWidget *parent) : QTableView(parent)
{
    setContentsMargins(0,0,0,0);
    setMinimumWidth(300);
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

    GBL_HistoryModel *pModel = (GBL_HistoryModel*)model();
    pModel->layoutChanged();
}

/*void HistoryView::itemSelectionChanged()
{
    qDebug() << "itemSectionChanged";
}*/
