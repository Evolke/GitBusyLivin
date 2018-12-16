#include "badgetoolbutton.h"

#include <QPainter>
#include <QPolygon>
#include <QPoint>
#include <QSize>
#include <QDebug>

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

        QSize badgeSize(17,13);
        QRect widgetRct = rect();
        QSize iconsz = iconSize();
        QRect rct;
        int iconAreaWidth = 24;
        if (toolButtonStyle() == Qt::ToolButtonTextBesideIcon)
        {
            iconAreaWidth += 8;
        }

        int badgeLeft = 0;//iconAreaWidth/2 - badgeSize.width()/2;
        switch (m_nArrowType)
        {
            case 1:
                rct.setRect(badgeLeft,widgetRct.bottom() - badgeSize.height()-1,badgeSize.width(), badgeSize.height());
                break;
            case 2:
                rct.setRect(badgeLeft, 1,badgeSize.width(), badgeSize.height());
                break;
            default:
                break;
        }

        p.setRenderHint(QPainter::Antialiasing,true);
        p.setRenderHint(QPainter::TextAntialiasing,true);
        QPalette pal = palette();
        QColor txtClr(255,255,255);//pal.color(QPalette::Text);
        QColor bkClr(200,0,0);//pal.color(QPalette::Background);
        //p.fillRect(rct, pal.background());
        p.setBrush(QBrush(bkClr));
        p.drawRoundedRect(rct,6,6);
        //triangle << QPoint(rct.left() + rct.width()/2,rct.top()) << rct.bottomLeft() << rct.bottomRight();
        //triangle << rct.topLeft() << rct.topRight() << QPoint(rct.left() + rct.width()/2,rct.bottom());
        //p.drawPolygon(triangle);
        int pointSize = 10;
#ifdef Q_OS_WIN
        pointSize = 8;
#endif
        QFont badgeFont("Arial", pointSize);
        badgeFont.setStretch(QFont::Condensed);
        p.setFont(badgeFont);
        QPen pen(txtClr);
        pen.setWidth(1);
        p.setPen(pen);
        QRect txtRct = rct;
        int nArrH = 3;
        int nArrW = 8;
        int nCtr = rct.left() + rct.width()/2;

#ifdef Q_OS_WIN
        txtRct.adjust(0,-1,0,-1);
        //nCtr++;
#endif
        p.drawText(txtRct,Qt::AlignCenter, m_sBatch);
/*        QPolygon upArrow, downArrow;
        upArrow << QPoint(nCtr - nArrW/2,rct.top()+nArrH+1) << QPoint(nCtr,rct.top()+1) << QPoint(nCtr + nArrW/2,rct.top()+nArrH+1);
        qDebug() << "upArrow:" << upArrow;
        downArrow << QPoint(nCtr - nArrW/2,rct.bottom() - nArrH-1) << QPoint(nCtr,rct.bottom()-1) << QPoint(nCtr + nArrW/2,rct.bottom()-nArrH-1);
        p.setBrush(QBrush(txtClr));
        switch (m_nArrowType)
        {
            case 1:
                p.drawPolyline(upArrow);
                break;
            case 2:
                p.drawPolyline(downArrow);
                break;
            default:
                break;
        }
*/


    }
}
