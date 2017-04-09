#include "stageddockview.h"
#include "fileview.h"
#include "src/gbl/gbl_filemodel.h"

#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

StagedDockView::StagedDockView(QWidget *parent) : QSplitter(Qt::Vertical,parent)
{
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
    m_pCommitEdit = new QTextEdit(tr("no commit message"), this);
    //m_pCommitEdit->selectAll();
    m_pCommitBtn = new QPushButton(tr("Commit"), this);
    m_pCommitBtn->setDisabled(true);
    m_pCommitBtn->setMaximumSize(100,25);
    m_pPushBtn = new QPushButton(tr("Push"), this);
    m_pPushBtn->setDisabled(true);
    m_pPushBtn->setMaximumSize(100,25);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pCommitEdit);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(5);
    btnLayout->setMargin(0);
    btnLayout->addWidget(m_pCommitBtn);
    btnLayout->addWidget(m_pPushBtn);
    mainLayout->addLayout(btnLayout);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(5);
    setLayout(mainLayout);

}
