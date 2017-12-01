#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
class QPixmap;
QT_END_NAMESPACE

class AboutInfo : public QTextEdit
{
    Q_OBJECT
public:
    explicit AboutInfo(QWidget *parent = 0);
};

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::FramelessWindowHint|Qt::Dialog);
    ~AboutDialog();


signals:

private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    QPixmap *m_gbl_logo/*, *m_libgit_logo, *m_qt_logo*/;
    AboutInfo *m_pInfo;
    int m_hue;
    int m_timer;
};

#endif // ABOUTDIALOG_H
