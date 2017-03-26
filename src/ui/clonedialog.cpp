#include "clonedialog.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

/**
 * @brief CloneDialog::CloneDialog
 * @param parent
 */
CloneDialog::CloneDialog(QWidget *parent) : QDialog(parent)
{
    int nWidth = 500;
    int nHeight = 200;
    if (parent != NULL)
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
            parent->y() + parent->height()/2 - nHeight/2,
            nWidth, nHeight);
    else
        resize(nWidth, nHeight);

    QLabel *pSrcLabel = new QLabel(tr("Source:"));
    m_pSrcEdit = new QLineEdit();
    QLabel *pDestLabel = new QLabel(tr("Destination:"));
    m_pDstEdit = new QLineEdit();
    QPushButton *pSrcBtn = new QPushButton(tr("..."));
    QPushButton *pDstBtn = new QPushButton(tr("..."));
    m_pBtnBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    QPushButton *pOkBtn = m_pBtnBox->button(QDialogButtonBox::Ok);
    pOkBtn->setDisabled(true);
    connect(m_pBtnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_pBtnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(pSrcBtn,&QPushButton::clicked, this, &CloneDialog::clickedSourceBrowse);
    connect(pDstBtn,&QPushButton::clicked, this, &CloneDialog::clickedDestinationBrowse);

    QGridLayout *mainLayout = new QGridLayout(this);
    //flay->addRow(pSrcLabel, pSrcEdit);
    //flay->addRow(pDestLabel, pDestEdit);
    //mainLayout->addWidget(tabWidget);
    //mainLayout->insertLayout(0,flay);
    mainLayout->addWidget(pSrcLabel,0,0);
    mainLayout->addWidget(m_pSrcEdit,0,1);
    mainLayout->addWidget(pSrcBtn,0,2);
    mainLayout->addWidget(new QLabel("<b style=\'font-size:18px\'>?</b>"),0,3);
    mainLayout->addWidget(pDestLabel,1,0);
    mainLayout->addWidget(m_pDstEdit,1,1);
    mainLayout->addWidget(pDstBtn, 1,2);
    mainLayout->addWidget(new QLabel("<b style=\'font-size:18px\'>?</b>"),1,3);
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
    }
}
