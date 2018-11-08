#include "branchdialog.h"

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>


BranchDialog::BranchDialog(QWidget *parent) : GBLDialog(parent)
{
    init(500,100, parent);

    QLabel *pBrnchLabel = new QLabel(tr("Branch Name:"));
    m_pBrnchEdit = new QLineEdit();

    m_pOkBtn->setText(tr("Create"));
    m_pOkBtn->setDisabled(true);
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
