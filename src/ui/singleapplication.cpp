#include "singleapplication.h"

#include <QSharedMemory>

SingleApplication::SingleApplication(int &argc, char **argv) : QApplication(argc, argv)
{
    _singular = new QSharedMemory("SharedMemoryForOnlyOneInstanceOfYourBeautifulApplication", this);
}

SingleApplication::~SingleApplication()
{
   if(_singular->isAttached())
       _singular->detach();

}

bool SingleApplication::lock()
{
    if (_singular->attach(QSharedMemory::ReadOnly))
    {
        _singular->detach();

        return false;
    }

    if (_singular->create(1))
        return true;

    return false;

}
