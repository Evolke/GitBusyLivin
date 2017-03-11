#include "commitdetail.h"

#include <QLabel>
#include <QGridLayout>
#include <QString>
#include <QTextStream>
#include <QDebug>

#include "src/gbl/gbl_storage.h"
#include "src/gbl/gbl_repository.h"

CommitDetail::CommitDetail(QWidget *parent) : QFrame(parent)
{
    //resize(400,200);
    m_pHistItem = NULL;
    QGridLayout *mainLayout = new QGridLayout(this);
    m_pAvatar = new QLabel(this);
    m_pDetails = new QLabel(this);
    m_pDetails->setWordWrap(true);
    m_pDetails->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    mainLayout->addWidget(m_pAvatar,0,0);
    mainLayout->addWidget(m_pDetails,0,1);
    m_pAvatar->setMaximumWidth(50);
    m_pAvatar->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
    m_pDetails->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    m_pDetails->setMinimumHeight(0);
    m_pDetails->setMinimumWidth(0);
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
    m_pDetails->setText(sHtml);
    if (pAvatar)
    {
        QPixmap avatar = pAvatar->scaledToWidth(48);
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
   setFrameStyle(QFrame::StyledPanel);
}

void CommitDetailScrollArea::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    QSize sa_size = size();
    sa_size -= QSize(20,20);
    m_pDetail->setMinimumSize(sa_size);
    m_pDetail->move(10,10);
}

void CommitDetailScrollArea::reset()
{
    m_pDetail->reset();
}

void CommitDetailScrollArea::setDetails(GBL_History_Item *pHistItem, QPixmap *pAvatar)
{
    m_pDetail->setDetails(pHistItem, pAvatar);
}
