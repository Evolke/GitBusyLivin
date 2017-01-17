#include "src/ui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Evolke");
    QCoreApplication::setApplicationName("GitBusyLivin");
    QCoreApplication::setApplicationVersion("0.0.1");

    MainWindow w;
    w.show();

    return a.exec();
}
