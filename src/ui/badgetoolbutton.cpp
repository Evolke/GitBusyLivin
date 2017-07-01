#include "badgetoolbutton.h"

#include <QPainter>
#include <QPolygon>
#include <QPoint>

BadgeToolButton::BadgeToolButton(QWidget *parent) : QToolButton(parent)
{
    m_nArrowType = 0;
}

void BadgeToolButton::paintEvent(QPaintEvent *event)
{
    QToolButton::paintEvent(event);

    if (!m_sBatch.isEmpty())
    {
        QPainter p(this);

        QRect rct(4,4,18,18);

        QPalette pal = palette();
        QColor txtClr = pal.color(QPalette::Text);
        QColor bkClr = pal.color(QPalette::Background);
        p.fillRect(rct, pal.background());
        p.setBrush(QBrush(txtClr));
        p.drawEllipse(rct);
        //triangle << QPoint(rct.left() + rct.width()/2,rct.top()) << rct.bottomLeft() << rct.bottomRight();
        //triangle << rct.topLeft() << rct.topRight() << QPoint(rct.left() + rct.width()/2,rct.bottom());
        //p.drawPolygon(triangle);
        p.setFont(QFont("monospace", 8));
        QPen pen(bkClr);
        //pen.setWidth(2);
        p.setPen(pen);
        p.drawText(rct,Qt::AlignCenter, m_sBatch);
        QPolygon upArrow, downArrow;
        int nArrH = 2;
        upArrow << QPoint(rct.left() + rct.width()/3,rct.top() + nArrH) << QPoint(rct.left() + rct.width()/2,rct.top()) << QPoint(rct.right() - rct.width()/3,rct.top()+nArrH);
        downArrow << QPoint(rct.left() + rct.width()/3,rct.bottom() - nArrH) << QPoint(rct.left() + rct.width()/2,rct.bottom()) << QPoint(rct.right() - rct.width()/3,rct.bottom()-nArrH);
        p.setBrush(pal.background());
        switch (m_nArrowType)
        {
            case 1:
                p.drawPolygon(upArrow);
                break;
            case 2:
                p.drawPolygon(downArrow);
                break;
            default:
                break;
        }



    }
}
