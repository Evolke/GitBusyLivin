#ifndef STATUSPROGRESSBAR_H
#define STATUSPROGRESSBAR_H

#include <QProgressBar>

class StatusProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    explicit StatusProgressBar(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // STATUSPROGRESSBAR_H
