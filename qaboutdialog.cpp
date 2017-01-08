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
    m_hue = (int)rand()%360;
    m_timer = startTimer(5000);
}

QAboutDialog::~QAboutDialog()
{
    killTimer(m_timer);
    delete m_pix;
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
    m_hue += 10;
    if (m_hue >= 360) {m_hue = 0; }
    update();
}

void QAboutDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);

    QRectF frame(QPointF(0,0), geometry().size());
    qreal w = m_pix->width();
    qreal h = m_pix->height();
    QPointF pixpos = frame.bottomRight() - QPointF(w+10,h);

    QLinearGradient linearGradient(0, 0, 0, height());
    linearGradient.setColorAt(0.0, QColor::fromHsl(m_hue,100,100));
    linearGradient.setColorAt(1.0, QColor::fromHsl(m_hue,100,20));
    //p.setBrush(linearGradient);
    p.fillRect(0, 0, width(), height(), linearGradient);
    p.drawPixmap(pixpos, *m_pix);
    QFont f("Arial", 28, 75);
    p.setFont(f);
    p.setPen(QColor(0, 0, 0));
    p.drawText(QPointF(20,50), QString("GitBusyLivin"));
    p.setFont(QFont("Arial", 12));
    p.setPen(QColor(140,140,140));
    p.drawText(QPointF(20,70), tr("Hope is a good thing, maybe the best of things, and no good thing ever dies."));
}

