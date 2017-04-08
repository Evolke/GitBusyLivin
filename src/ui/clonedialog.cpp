#include "clonedialog.h"
#include "src/gbl/gbl_repository.h"
#include "mainwindow.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <QDir>

#define EMPTY_FORMAT "<b style=\'font-size:18px\'>?</b>"
#define ERROR_FORMAT "<b style=\'font-size:18px;color:#f00\'>X</b>"
#define CORRECT_FORMAT "<b style=\'font-size:18px;color:#0F0\'>&#x2713;</b>"
/**
 * @brief CloneDialog::CloneDialog
 * @param parent
 */
CloneDialog::CloneDialog(QWidget *parent) : QDialog(parent)
{
    int nWidth = 500;
    int nHeight = 200;
    if (parent != NULL)
    {
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
            parent->y() + parent->height()/2 - nHeight/2,
            nWidth, nHeight);
        m_pRepo = ((MainWindow*)parent)->getRepo();
    }
    else
    {
        m_pRepo = NULL;
        resize(nWidth, nHeight);
    }

    QLabel *pSrcLabel = new QLabel(tr("Source:"));
    m_pSrcEdit = new QLineEdit();
    QLabel *pDestLabel = new QLabel(tr("Destination:"));
    m_pDstEdit = new QLineEdit();
    QPushButton *pSrcBtn = new QPushButton(tr("..."));
    QPushButton *pDstBtn = new QPushButton(tr("..."));
    m_pBtnBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    m_pOkBtn = m_pBtnBox->button(QDialogButtonBox::Ok);
    m_pOkBtn->setDisabled(true);
    connect(m_pBtnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_pBtnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(pSrcBtn,&QPushButton::clicked, this, &CloneDialog::clickedSourceBrowse);
    connect(pDstBtn,&QPushButton::clicked, this, &CloneDialog::clickedDestinationBrowse);
    connect(m_pSrcEdit, &QLineEdit::editingFinished, this, &CloneDialog::sourceEdited);
    connect(m_pDstEdit, &QLineEdit::editingFinished, this, &CloneDialog::destEdited);
    QGridLayout *mainLayout = new QGridLayout(this);
    //flay->addRow(pSrcLabel, pSrcEdit);
    //flay->addRow(pDestLabel, pDestEdit);
    //mainLayout->addWidget(tabWidget);
    //mainLayout->insertLayout(0,flay);
    mainLayout->addWidget(pSrcLabel,0,0);
    mainLayout->addWidget(m_pSrcEdit,0,1);
    mainLayout->addWidget(pSrcBtn,0,2);
    //m_pSrcValidateLabel = new QLabel(EMPTY_FORMAT);
    //mainLayout->addWidget(m_pSrcValidateLabel,0,3);
    mainLayout->addWidget(pDestLabel,1,0);
    mainLayout->addWidget(m_pDstEdit,1,1);
    mainLayout->addWidget(pDstBtn, 1,2);
    //m_pDstValidateLabel = new QLabel(EMPTY_FORMAT);
    //mainLayout->addWidget(m_pDstValidateLabel,1,3);
    mainLayout->addWidget(m_pBtnBox,2,1,1,2, Qt::AlignBottom);
    setLayout(mainLayout);

    setWindowTitle(tr("Clone"));
}

/**
 * @brief CloneDialog::getSource
 * @return
 */
QString CloneDialog::getSource()
{
    return m_pSrcEdit->text();
}

/**
 * @brief CloneDialog::getDestination
 * @return
 */
QString CloneDialog::getDestination()
{
    return m_pDstEdit->text();
}

/**
 * @brief CloneDialog::clickedSourceBrowse
 */
void CloneDialog::clickedSourceBrowse()
{
    QString dirName = QFileDialog::getExistingDirectory(this);
    if (!dirName.isEmpty())
    {
        m_pSrcEdit->setText(dirName);
        validate();
    }
}

/**
 * @brief CloneDialog::clickedDestinationBrowse
 */
void CloneDialog::clickedDestinationBrowse()
{
    QString dirName = QFileDialog::getExistingDirectory(this);
    if (!dirName.isEmpty())
    {
        m_pDstEdit->setText(dirName);
        validate();
    }
}

void CloneDialog::sourceEdited()
{
    validate();
}

void CloneDialog::destEdited()
{
    validate();
}

void CloneDialog::validate()
{
    QString sSrc = m_pSrcEdit->text();
    QString sDst = m_pDstEdit->text();
    QDir dstDir;

    bool bDst = !sDst.isEmpty() && dstDir.exists(sDst);
    m_pOkBtn->setDisabled(sSrc.isEmpty() || !bDst);

}
