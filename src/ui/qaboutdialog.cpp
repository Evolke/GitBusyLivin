#include "src/gbl/gbl_version.h"
#include "qaboutdialog.h"
#include <QPainter>
#include <QTextStream>

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
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    QRectF frame(QPointF(0,0), geometry().size());
    qreal w = m_pix->width();
    qreal h = m_pix->height();
    QPointF pixpos = frame.bottomRight() - QPointF(w+10,h);

    QLinearGradient linearGradient(0, 0, 0, height());
    linearGradient.setColorAt(0.0, QColor::fromHsl(m_hue,120,120));
    linearGradient.setColorAt(1.0, QColor::fromHsl(m_hue,100,20));
    //p.setBrush(linearGradient);
    p.fillRect(0, 0, width(), height(), linearGradient);
    p.drawPixmap(pixpos, *m_pix);
    QFont f("Helvetica", 28, QFont::Bold);
    p.setFont(f);
    p.setPen(QColor(0, 0, 0));
    p.drawText(QPointF(20,40), QString("GitBusyLivin"));
 #ifdef Q_OS_WIN
    int size = 10;
 #else
    int size = 13;
 #endif
    p.setFont(QFont("Helvetica", size));
    QString quote(tr("Hope is a good thing, maybe the best of things, and no good thing ever dies."));

    p.setPen(QColor(180,180,180));
    p.drawText(QRectF(20,45, width() - 20, 100), quote);
    QString vers;
    QTextStream(&vers) << "version: " << GBL_APP_VERSION;
    p.setPen(QColor(0,0,0));
    p.drawText(QPointF(20,80), vers);
}

