#include "branchdialog.h"

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>


BranchDialog::BranchDialog(QWidget *parent) : QDialog(parent)
{
    int nWidth = 500;
    int nHeight = 200;
    if (parent != NULL)
    {
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
            parent->y() + parent->height()/2 - nHeight/2,
            nWidth, nHeight);
    }
    else
    {
        resize(nWidth, nHeight);
    }

    QLabel *pBrnchLabel = new QLabel(tr("Branch Name:"));
    m_pBrnchEdit = new QLineEdit();
    m_pBtnBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    m_pOkBtn = m_pBtnBox->button(QDialogButtonBox::Ok);
    m_pOkBtn->setText(tr("Create"));
    m_pOkBtn->setDisabled(true);
    connect(m_pBtnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_pBtnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_pBrnchEdit, &QLineEdit::editingFinished, this, &BranchDialog::nameEdited);
    QGridLayout *mainLayout = new QGridLayout(this);
    //flay->addRow(pSrcLabel, pSrcEdit);
    //flay->addRow(pDestLabel, pDestEdit);
    //mainLayout->addWidget(tabWidget);
    //mainLayout->insertLayout(0,flay);
    mainLayout->addWidget(pBrnchLabel,0,0);
    mainLayout->addWidget(m_pBrnchEdit,0,1);
    mainLayout->addWidget(m_pBtnBox,2,1,1,2, Qt::AlignBottom);
    setLayout(mainLayout);

    setWindowTitle(tr("Create Branch"));

}

QString BranchDialog::getBranchName()
{
    return m_pBrnchEdit->text();
}

void BranchDialog::nameEdited()
{
    QString sName = m_pBrnchEdit->text();
    m_pOkBtn->setEnabled(!sName.isEmpty());

}
