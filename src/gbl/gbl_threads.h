#ifndef GBL_THREADS_H
#define GBL_THREADS_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class GBL_HistoryThread : public QThread
{
    Q_OBJECT
public:
    GBL_HistoryThread(QObject *parent = 0);
    ~GBL_HistoryThread();

protected:
    void run() override;

    QMutex mutex;
    QWaitCondition condition;

    bool restart;
    bool abort;

};
#endif // GBL_THREADS_H
