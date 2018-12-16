#include "scandialog.h"
#include "optionsmenubutton.h"
#include "src/gbl/gbl_threads.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QMenu>
#include <QActionGroup>

ScanDialog::ScanDialog(QWidget *parent) : QDialog(parent)
{
    int nWidth = 500;
    int nHeight = 200;
    if (parent != Q_NULLPTR)
    {
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
            parent->y() + parent->height()/2 - nHeight/2,
            nWidth, nHeight);
    }
    else
    {
        resize(nWidth, nHeight);
    }

    QLabel *pRootLabel = new QLabel(tr("Root Path:"));
    m_pRootEdit = new QLineEdit();
    QLabel *pSearchLabel = new QLabel(tr("Search:"));
    m_pSearchEdit = new QLineEdit();
    QPushButton *pRootBtn = new QPushButton(tr("..."));
    m_pBtnBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    QPushButton *pSrchBtn = m_pBtnBox->button(QDialogButtonBox::Ok);
    pSrchBtn->setText(tr("Search"));
    pSrchBtn->setDisabled(true);
    m_pSearchOptionsBtn = new OptionsMenuButton(this);
    QMenu *pMenu = m_pSearchOptionsBtn->getMenu();
    QAction *pCIAct = new QAction(tr("Case Insensitive"),this);
    pCIAct->setCheckable(true);
    QAction *pCSAct = new QAction(tr("Case Sensitive"),this);
    pCSAct->setCheckable(true);
    QAction *pREAct = new QAction(tr("Regular Expression"),this);
    pREAct->setCheckable(true);
    connect(pCIAct, &QAction::triggered, this, &ScanDialog::searchTypeCaseInsensitive);
    connect(pCSAct, &QAction::triggered, this, &ScanDialog::searchTypeCaseSensitive);
    connect(pREAct, &QAction::triggered, this, &ScanDialog::searchTypeRegex);
    m_pActGrp = new QActionGroup(this);
    m_pActGrp->addAction(pCIAct);
    m_pActGrp->addAction(pCSAct);
    m_pActGrp->addAction(pREAct);
    pCIAct->setChecked(true);
    pMenu->addAction(pCIAct);
    pMenu->addAction(pCSAct);
    pMenu->addAction(pREAct);

    connect(m_pBtnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_pBtnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(pRootBtn,&QPushButton::clicked, this, &ScanDialog::clickedRootBrowse);
    connect(m_pRootEdit, &QLineEdit::editingFinished, this, &ScanDialog::rootEdited);
    connect(m_pSearchEdit, &QLineEdit::editingFinished, this, &ScanDialog::searchEdited);
    QGridLayout *mainLayout = new QGridLayout(this);
    //flay->addRow(pSrcLabel, pSrcEdit);
    //flay->addRow(pDestLabel, pDestEdit);
    //mainLayout->addWidget(tabWidget);
    //mainLayout->insertLayout(0,flay);
    mainLayout->addWidget(pRootLabel,0,0);
    mainLayout->addWidget(m_pRootEdit,0,1);
    mainLayout->addWidget(pRootBtn,0,2);
    //m_pSrcValidateLabel = new QLabel(EMPTY_FORMAT);
    //mainLayout->addWidget(m_pSrcValidateLabel,0,3);
    mainLayout->addWidget(pSearchLabel,1,0);
    mainLayout->addWidget(m_pSearchEdit,1,1);
    mainLayout->addWidget(m_pSearchOptionsBtn,1,2);
    //mainLayout->addWidget(pDstBtn, 1,2);
    //m_pDstValidateLabel = new QLabel(EMPTY_FORMAT);
    //mainLayout->addWidget(m_pDstValidateLabel,1,3);
    mainLayout->addWidget(m_pBtnBox,2,1,1,2, Qt::AlignBottom);
    setLayout(mainLayout);

    setWindowTitle(tr("Scan"));

    m_nSearchType = SCAN_THREAD_SEARCH_TYPE_INSENSITIVE;
}


void ScanDialog::clickedRootBrowse()
{
    QString dirName = QFileDialog::getExistingDirectory(this);
    if (!dirName.isEmpty())
    {
        m_pRootEdit->setText(dirName);
        validate();
    }

}

void ScanDialog::rootEdited()
{
    validate();
}

void ScanDialog::searchEdited()
{
    validate();
}

void ScanDialog::searchTypeCaseInsensitive()
{
    m_nSearchType = SCAN_THREAD_SEARCH_TYPE_INSENSITIVE;
}

void ScanDialog::searchTypeCaseSensitive()
{
    m_nSearchType = SCAN_THREAD_SEARCH_TYPE_SENSITIVE;
}

void ScanDialog::searchTypeRegex()
{
    m_nSearchType = SCAN_THREAD_SEARCH_TYPE_REGEX;
}

void ScanDialog::validate()
{
    QString sRoot = m_pRootEdit->text();
    QString sSearch = m_pSearchEdit->text();
    QDir rootDir;

    bool bRoot = !sRoot.isEmpty() && rootDir.exists(sRoot);

    QPushButton *pSrchBtn = m_pBtnBox->button(QDialogButtonBox::Ok);
    pSrchBtn->setDisabled(sSearch.isEmpty() || !bRoot);

}

QString ScanDialog::getRootPath()
{
   return m_pRootEdit->text();
}

QString ScanDialog::getSearch()
{
    return m_pSearchEdit->text();
}

