#include "commitdetail.h"

#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>
#include <QString>
#include <QTextStream>
#include <QDebug>

#include "src/gbl/gbl_storage.h"
#include "src/gbl/gbl_repository.h"

CommitDetail::CommitDetail(QWidget *parent) : QFrame(parent)
{
    //resize(400,200)
    setFrameStyle(QFrame::StyledPanel);
    m_pHistItem = NULL;
    QGridLayout *mainLayout = new QGridLayout(this);
    m_pAvatar = new QLabel(this);
    m_pDetails = new QTextEdit(this);
    m_pDetails->setReadOnly(true);
    m_pDetails->setFrameStyle(QFrame::NoFrame);
    m_pDetails->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    mainLayout->addWidget(m_pAvatar,0,0);
    mainLayout->addWidget(m_pDetails,0,1);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(2);
    m_pAvatar->setMaximumWidth(42);
    m_pAvatar->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
    //m_pDetails->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    //m_pDetails->setMinimumHeight(0);
    //m_pDetails->setMinimumWidth(0);
    setContentsMargins(0,0,0,0);
}

void CommitDetail::reset()
{
    m_pAvatar->clear();
    m_pDetails->clear();
}

void CommitDetail::setDetails(GBL_History_Item *pHistItem, QPixmap *pAvatar)
{
    m_pHistItem = pHistItem;

    QString sEmail = m_pHistItem->hist_author_email;
    //QString sUrl = GBL_Storage::getGravatarUrl(sEmail);
    QString sHtml("<table cellpadding=\'2\'><tr><td><i>Commit:</i></td><td>");
    sHtml += m_pHistItem->hist_oid;
    sHtml += "</td></tr><td><i>Parents:</i></td><td>";
    int nParentCount = m_pHistItem->hist_parents.size();
    for (int i = 0; i < nParentCount; i++)
    {
        sHtml += m_pHistItem->hist_parents.at(i).left(7);
        if (i < (nParentCount-1)) sHtml += ", ";
    }

    sHtml += "</td></tr><tr><td><i>Author:</i></td><td>";
    sHtml += m_pHistItem->hist_author;
    sHtml += "</td></tr><tr><td><i>Email:</i></td><td>";
    sHtml += m_pHistItem->hist_author_email;
    sHtml += "</td></tr><tr><td><i>Date:</i></td><td>";
    sHtml += pHistItem->hist_datetime.toString("MM/dd/yyyy hh:mm:ss ap");
    sHtml += "</td></tr><tr><td><i>Message:</i></td><td>";
    sHtml += m_pHistItem->hist_message;
    sHtml += "</td></tr>";
    sHtml += "</table>";
    //qDebug() << sHtml;
    m_pDetails->setHtml(sHtml);
    if (pAvatar)
    {
        QPixmap avatar = pAvatar->scaledToWidth(40);
        m_pAvatar->setPixmap(avatar);
    }

}


/**
 *    CommitDetailScrollArea
 *
 */
CommitDetailScrollArea::CommitDetailScrollArea(QWidget *parent) : QScrollArea(parent)
{
   m_pDetail = new CommitDetail(this);
   setContentsMargins(0,0,0,0);
   setBackgroundRole(QPalette::Base);
   setFrameStyle(QFrame::StyledPanel);
   setViewportMargins(0,0,0,0);
}

void CommitDetailScrollArea::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    QSize sa_size = size();
    //sa_size -= QSize(10,10);
    //m_pDetail->setMinimumSize(sa_size);
    m_pDetail->resize(sa_size);
    //m_pDetail->move(5,5);
}

void CommitDetailScrollArea::reset()
{
    m_pDetail->reset();
}

void CommitDetailScrollArea::setDetails(GBL_History_Item *pHistItem, QPixmap *pAvatar)
{
    m_pDetail->setDetails(pHistItem, pAvatar);
}
