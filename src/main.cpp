#include "src/gbl/gbl_version.h"
#include "src/ui/mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(gitbusylivin);

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName(GBL_APP_COMPANY_NAME);
    QCoreApplication::setApplicationName(GBL_APP_NAME);
    QCoreApplication::setApplicationVersion(QString(GBL_APP_VERSION));

    /**/
    MainWindow w;
    w.show();

    return a.exec();
}
