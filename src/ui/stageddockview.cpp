#include "stageddockview.h"
#include "fileview.h"
#include "src/gbl/gbl_filemodel.h"

#include <QTextEdit>
#include <QGridLayout>
#include <QDebug>
#include <QScrollBar>

#define COMMIT_MIN_WIDTH 330
#define COMMIT_MIN_HEIGHT 60

StagedDockView::StagedDockView(QWidget *parent) : QSplitter(Qt::Vertical,parent)
{
    setContentsMargins(0,0,0,0);
    m_pFileView = new FileView(this);
    m_pFileView->setModel(new GBL_FileModel(m_pFileView));
    addWidget(m_pFileView);
    m_pCommitView = new StagedCommitView(this);
    addWidget(m_pCommitView);
}

void StagedDockView::setFileArray(GBL_File_Array *pArr)
{
    GBL_FileModel *pMod = (GBL_FileModel*)m_pFileView->model();
    pMod->setFileArray(pArr);
}


StagedCommitView::StagedCommitView(QWidget *parent) : QScrollArea(parent)
{
    setViewportMargins(0,0,0,0);
    m_pWrap = new QWidget(this);
    setGeometry(0,0,200,100);
    m_pWrap->setGeometry(0,0,COMMIT_MIN_WIDTH,COMMIT_MIN_HEIGHT);
    m_pWrap->setMinimumSize(COMMIT_MIN_WIDTH,COMMIT_MIN_HEIGHT);
    setWidget(m_pWrap);
    //setWidgetResizable(true);
    QGridLayout *mainLayout = new QGridLayout(m_pWrap);
    mainLayout->setSpacing(4);
    mainLayout->setMargin(2);
    m_pCommitEdit = new QTextEdit(tr("no commit message"), m_pWrap);
    m_pCommitEdit->setGeometry(0,0,COMMIT_MIN_WIDTH,30);
    mainLayout->addWidget(m_pCommitEdit,0,0,1,4);
    //m_pCommitEdit->selectAll();
    //setMinimumWidth(350);
    m_pCommitBtn = new StagedButton(tr("Commit"), m_pWrap);
    m_pCommitBtn->setDisabled(true);
    m_pCommitBtn->setMaximumSize(100,20);
    m_pCommitBtn->setMinimumSize(60,20);
    mainLayout->addWidget(m_pCommitBtn,1,0,1,1,Qt::AlignBottom);
    m_pPushBtn = new StagedButton(tr("Push"), m_pWrap);
    m_pPushBtn->setDisabled(true);
    m_pPushBtn->setMaximumSize(80,20);
    m_pPushBtn->setMinimumSize(50,20);
    mainLayout->addWidget(m_pPushBtn,1,1,1,1,Qt::AlignBottom);
    m_pUnstageAllBtn = new StagedButton(tr("Unstage All"), m_pWrap);
    m_pUnstageAllBtn->setDisabled(true);
    m_pUnstageAllBtn->setMaximumSize(100,20);
    m_pUnstageAllBtn->setMinimumSize(80,20);
    mainLayout->addWidget(m_pUnstageAllBtn,1,2,1,1,Qt::AlignBottom);
    m_pUnstageSelBtn = new StagedButton(tr("Unstage Selected"), m_pWrap);
    m_pUnstageSelBtn->setDisabled(true);
    m_pUnstageSelBtn->setMaximumSize(120,20);
    m_pUnstageSelBtn->setMinimumSize(120,20);
    mainLayout->addWidget(m_pUnstageSelBtn,1,3,1,1,Qt::AlignBottom);
    mainLayout->setRowStretch(0,1);
    mainLayout->setRowStretch(1,0);


    m_pWrap->setLayout(mainLayout);

}

void StagedCommitView::resizeEvent(QResizeEvent *event)
{
    QSize scv_size = size();
    qDebug() << "commitview::resize: " << scv_size;
    QScrollBar *pVSB = verticalScrollBar();
    QScrollBar *pHSB = horizontalScrollBar();

    QSize subSize(pVSB && pVSB->isVisible() ? pVSB->width() : 0, pHSB && pHSB->isVisible() ? pHSB->height() : 0);
    scv_size -= subSize;
    qDebug() << "commitview::resize-minus-sb: " << scv_size;

    if (scv_size.width() > COMMIT_MIN_WIDTH || scv_size.height() > COMMIT_MIN_HEIGHT)
    {
        if (scv_size.width() < COMMIT_MIN_WIDTH) scv_size.setWidth(COMMIT_MIN_WIDTH);
        if (scv_size.height() < COMMIT_MIN_HEIGHT) scv_size.setHeight(COMMIT_MIN_HEIGHT);
        m_pWrap->resize(scv_size);
    }

}
