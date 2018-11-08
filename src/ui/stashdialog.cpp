#include "stashdialog.h"

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>

StashDialog::StashDialog(QWidget *parent) : GBLDialog(parent)
{
    init(500,100,parent);

    QLabel *pStashLabel = new QLabel(tr("Stash Message:"));
    m_pStashEdit = new QLineEdit();

    m_pOkBtn->setText(tr("Create"));
    m_pOkBtn->setDisabled(true);
    connect(m_pStashEdit, &QLineEdit::editingFinished, this, &StashDialog::messageEdited);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(pStashLabel,0,0);
    mainLayout->addWidget(m_pStashEdit,0,1);
    mainLayout->addWidget(m_pBtnBox,2,1,1,2, Qt::AlignBottom);
    setLayout(mainLayout);

    setWindowTitle(tr("Create Stash"));

}

QString StashDialog::getStashMessage()
{
    return m_pStashEdit->text();
}

void StashDialog::messageEdited()
{
    QString sMsg = m_pStashEdit->text();
    m_pOkBtn->setEnabled(!sMsg.isEmpty());

}
