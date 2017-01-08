#include "qaboutdialog.h"
#include <QPainter>

QAboutDialog::QAboutDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    int nWidth = 500;
    int nHeight = 400;
    if (parent != NULL)
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
            parent->y() + parent->height()/2 - nHeight/2,
            nWidth, nHeight);
    else
        resize(nWidth, nHeight);

    m_pix = new QPixmap(QLatin1String(":/images/andy.png"));
    //setAutoFillBackground(false);
    m_endClr = new QColor(100,100,100);
    m_timer = startTimer(200);
}

QAboutDialog::~QAboutDialog()
{
    killTimer(m_timer);
    delete m_pix;
    delete m_endClr;
}

void QAboutDialog::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void QAboutDialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    close();
}

void QAboutDialog::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    QColor darker = m_endClr->darker(101);
    m_endClr->setRgb(darker.red(),darker.green(),darker.blue());
    repaint(0,0,width(),height());
}

void QAboutDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);

    QRectF frame(QPointF(0,0), geometry().size());
    qreal h = m_pix->height();
    QPointF pixpos = frame.bottomLeft() - QPointF(0,h);
    QFont f("Arial", 32, 700);
    p.setFont(f);
    p.setPen(QColor(0, 0, 0));

    QLinearGradient linearGradient(0, 0, 0, height());
    linearGradient.setColorAt(0.0, Qt::yellow);
    linearGradient.setColorAt(1.0, *m_endClr);
    p.setBrush(linearGradient);
    p.drawRect(0, 0, width(), height());
    p.drawPixmap(pixpos, *m_pix);
    p.drawText(QPointF(20,50), QString("GitBusyLivin"));
    p.setFont(QFont("Arial", 12));
    p.setPen(QColor(140,140,140));
    p.drawText(QPointF(20,70), tr("Hope is a good thing, maybe the best of things, and no good thing ever dies."));
}

