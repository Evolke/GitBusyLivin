#ifndef QABOUTDIALOG_H
#define QABOUTDIALOG_H

#include <QDialog>
#include <QPixmap>

class QAboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QAboutDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::FramelessWindowHint|Qt::Dialog);
    ~QAboutDialog();


signals:

private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    QPixmap *m_pix;
    int m_hue;
    int m_timer;
};

#endif // QABOUTDIALOG_H
