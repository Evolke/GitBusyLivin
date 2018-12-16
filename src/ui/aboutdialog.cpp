#include "src/gbl/gbl_version.h"
#include "aboutdialog.h"
#include <QPainter>
#include <QTextStream>
#include <QSvgRenderer>
#include <QPixmap>
#include <QTextEdit>
#include <QFile>

//static QSvgRenderer testsvg;

AboutDialog::AboutDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    int nWidth = 500;
    int nHeight = 400;
    if (parent != Q_NULLPTR)
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
            parent->y() + parent->height()/2 - nHeight/2,
            nWidth, nHeight);
    else
        resize(nWidth, nHeight);

    m_gbl_logo = new QPixmap(":/images/andy.png");
    m_pInfo = new AboutInfo(this);
    m_pInfo->setGeometry(QRect(0,90,width(),220));
    m_pInfo->setReadOnly(true);
    //m_pInfo->palette().setColor(QPalette::Window,QColor(0,0,0));
    QFile file(":/content/about.html");
    file.open(QIODevice::ReadOnly);
    QString html = QString::fromUtf8(file.readAll());

    m_pInfo->setText(html);
//    m_qt_logo = new QPixmap(":/images/qt_logo_32.png");
//    m_libgit_logo = new QPixmap(":/images/libgit2_logo.png");
    //setAutoFillBackground(false);
    m_hue = (int)rand()%360;
    m_timer = startTimer(10000);

    //testsvg.load(QString(":/images/push_toolbar_icon.svg"));
}

AboutDialog::~AboutDialog()
{
    killTimer(m_timer);
    delete m_gbl_logo;
    delete m_pInfo;
//    delete m_qt_logo;
//    delete m_libgit_logo;
}

void AboutDialog::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void AboutDialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    close();
}

void AboutDialog::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    m_hue += 10;
    if (m_hue >= 360) {m_hue = 0; }
    update();
}

void AboutDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    p.setRenderHint(QPainter::TextAntialiasing,true);
    QColor ltTxtClr(180,180,180);
    QRectF frame(QPointF(0,0), geometry().size());
    qreal w = m_gbl_logo->width();
    qreal h = m_gbl_logo->height();
    QPointF pixpos = frame.bottomRight() - QPointF(w+10,h);

    QLinearGradient linearGradient(0, 0, 0, height());
    linearGradient.setColorAt(0.0, QColor::fromHsl(m_hue,120,120));
    linearGradient.setColorAt(1.0, QColor::fromHsl(m_hue,100,20));
    //p.setBrush(linearGradient);
    p.fillRect(0, 0, width(), height(), linearGradient);
    QFont f("Helvetica", 28, QFont::Bold);
    p.setFont(f);
    p.setPen(QColor(0, 0, 0));
    p.setOpacity(1);
    p.drawText(QPointF(20,40), QString("GitBusyLivin"));
 #ifdef Q_OS_WIN
    int size = 10;
 #else
    int size = 13;
 #endif
    p.setFont(QFont("Helvetica", size));
    QString quote(tr("Hope is a good thing, maybe the best of things, and no good thing ever dies."));

    p.setPen(ltTxtClr);
    p.drawText(QRectF(20,45, width() - 20, 100), quote);
    QString vers;
    QTextStream(&vers) << "version: " << GBL_APP_VERSION;

    p.setPen(QColor(0,0,0));
    p.drawText(QPointF(20,80), vers);
    p.setPen(ltTxtClr);
    p.setBrush(QColor(0,0,0));
    p.setOpacity(.6);
    //p.drawRect(QRect(20,80, width()-40,200));
    p.drawPixmap(pixpos, *m_gbl_logo);
//    p.setOpacity(.4);
//    QPointF qtpixpos = frame.bottomLeft() + QPointF(20,-35);
//    p.drawPixmap(qtpixpos, m_qt_logo->scaledToWidth(24));
//    QPointF libgitpos = qtpixpos + QPointF(70,0);
//    p.drawPixmap(libgitpos, m_libgit_logo->scaledToWidth(24));
//    p.drawText(qtpixpos + QPointF(0,-5), "Built with:");
//    p.drawText(qtpixpos + QPointF(30,15), "Qt");
//    p.drawText(libgitpos + QPointF(30,15), "LibGit2");
    /*QSize svg_size = testsvg.defaultSize();
    QSize pm_size = svg_size * 2;
    QPixmap pix(pm_size);

    pix.fill(Qt::transparent);
    QPainter pixPainter(&pix);
    pixPainter.setRenderHint(QPainter::TextAntialiasing, false);
    testsvg.render(&pixPainter);
    p.drawPixmap(100,100,svg_size.width(), svg_size.height(), pix);*/
}


AboutInfo::AboutInfo(QWidget *parent) : QTextEdit(parent)
{
    setStyleSheet("AboutInfo {background-color:rgba(0,0,0,150);color:#aaa;border:none;}");
    setWindowOpacity(0.5);
}
