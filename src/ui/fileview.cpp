#include "fileview.h"
#include "src/gbl/gbl_filemodel.h"

FileView::FileView(QWidget *parent) : QTreeView(parent)
{

}

FileView::~FileView()
{
    GBL_FileModel *pFileModel = (GBL_FileModel*)model();
    delete pFileModel;
}
