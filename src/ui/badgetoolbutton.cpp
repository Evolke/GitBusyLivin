#include "badgetoolbutton.h"

#include <QPainter>

BadgeToolButton::BadgeToolButton(QWidget *parent) : QToolButton(parent)
{

}

void BadgeToolButton::paintEvent(QPaintEvent *event)
{
    QToolButton::paintEvent(event);

    if (!m_sBatch.isEmpty())
    {
        QPainter p(this);

        QRect rct(6,7,12,12);

        QPalette pal = palette();
        QColor txtClr = pal.color(QPalette::Text);
        QColor bkClr = pal.color(QPalette::Background);
        p.setBrush(QBrush(txtClr));
        p.drawEllipse(rct);
        p.setFont(QFont("Helvetica", 8));
        p.setPen(bkClr);
        p.drawText(rct,Qt::AlignCenter, "99");
    }
}
