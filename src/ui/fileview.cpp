#include "fileview.h"
#include "src/gbl/gbl_filemodel.h"
#include <QScrollBar>

FileView::FileView(QWidget *parent) : QTreeView(parent)
{
    setIndentation(0);
    setAllColumnsShowFocus(true);
}

FileView::~FileView()
{
    GBL_FileModel *pFileModel = (GBL_FileModel*)model();
    delete pFileModel;
}

void FileView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    int nWidth = width();

    QScrollBar *pSB = verticalScrollBar();
    if (pSB && pSB->isVisible()) nWidth -= pSB->width();

    setColumnWidth(0, nWidth*.75);
    setColumnWidth(1, nWidth*.24);
}
