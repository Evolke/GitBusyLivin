#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QPixmap>

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
    QPixmap *m_pix;
    int m_hue;
    int m_timer;
};

#endif // ABOUTDIALOG_H
