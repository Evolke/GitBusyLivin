#include "gbl_threads.h"

GBL_HistoryThread::GBL_HistoryThread(QObject *parent) : QThread(parent)
{
    restart = false;
    abort = false;

}

GBL_HistoryThread::~GBL_HistoryThread()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();

    wait();

}

void GBL_HistoryThread::run()
{
}
