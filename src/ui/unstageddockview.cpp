#include "unstageddockview.h"
#include "fileview.h"
#include "src/gbl/gbl_filemodel.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

UnstagedDockView::UnstagedDockView(QWidget *parent) : QScrollArea(parent)
{
    setViewportMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
    m_pFileView = new FileView(this);
    m_pFileView->setModel(new GBL_FileModel(m_pFileView));

    m_pBtnBar = new UnstagedButtonBar(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pFileView);
    mainLayout->addWidget(m_pBtnBar);
    setLayout(mainLayout);
    mainLayout->setMargin(0);
}

void UnstagedDockView::setFileArray(GBL_File_Array *pArr)
{
    GBL_FileModel *pMod = (GBL_FileModel*)m_pFileView->model();
    pMod->setFileArray(pArr);
}


UnstagedButtonBar::UnstagedButtonBar(QWidget *parent) : QFrame(parent)
{
    setContentsMargins(0,0,0,0);
    m_pAddAllBtn = new QPushButton(tr("Add All"),this);
    m_pAddSelBtn = new QPushButton(tr("Add Selected"),this);
    setMaximumHeight(25);
    m_pAddAllBtn->setDisabled(true);
    m_pAddAllBtn->setMaximumSize(100,25);
    m_pAddSelBtn->setDisabled(true);
    m_pAddSelBtn->setMaximumSize(100,25);
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_pAddAllBtn);
    mainLayout->addWidget(m_pAddSelBtn);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(2);
    setLayout(mainLayout);
}
