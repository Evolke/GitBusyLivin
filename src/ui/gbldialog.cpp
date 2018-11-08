#include "gbldialog.h"

#include <QDialogButtonBox>
#include <QPushButton>

GBLDialog::GBLDialog(QWidget *parent) : QDialog(parent)
{

}

void GBLDialog::init(int nWidth, int nHeight, QWidget *parent)
{
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

    m_pBtnBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    m_pOkBtn = m_pBtnBox->button(QDialogButtonBox::Ok);
    connect(m_pBtnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_pBtnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

}

