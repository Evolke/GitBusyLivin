#include "unstageddockview.h"
#include "fileview.h"
#include "src/gbl/gbl_filemodel.h"

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
    mainLayout->addWidget(m_pBtnBar, Qt::AlignVCenter);
    setLayout(mainLayout);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(2);
}

void UnstagedDockView::setFileArray(GBL_File_Array *pArr)
{
    GBL_FileModel *pMod = (GBL_FileModel*)m_pFileView->model();
    pMod->setFileArray(pArr);
    UnstagedButton *pBtn = m_pBtnBar->getButton(ADD_ALL_BTN);
    pBtn->setDisabled(pArr->size() == 0);
}


UnstagedButtonBar::UnstagedButtonBar(QWidget *parent) : QFrame(parent)
{
    setContentsMargins(0,0,0,0);
    m_pAddAllBtn = new UnstagedButton(tr("Add All"),this);
    m_pAddSelBtn = new UnstagedButton(tr("Add Selected"),this);
    setMaximumHeight(30);
    m_pAddAllBtn->setDisabled(true);
    m_pAddAllBtn->setMaximumSize(100,20);
    m_pAddSelBtn->setDisabled(true);
    m_pAddSelBtn->setMaximumSize(120,20);
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_pAddAllBtn, Qt::AlignVCenter);
    mainLayout->addWidget(m_pAddSelBtn, Qt::AlignVCenter);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setMargin(3);
    mainLayout->setSpacing(2);
    setLayout(mainLayout);
}

UnstagedButton* UnstagedButtonBar::getButton(int nBtnID)
{
    UnstagedButton *pRet = NULL;

    switch (nBtnID)
    {
        case ADD_ALL_BTN:
            pRet = m_pAddAllBtn;
            break;
        case ADD_SELECTED_BTN:
            pRet = m_pAddSelBtn;
            break;
    }

    return pRet;
}
