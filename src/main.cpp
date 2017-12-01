#include "src/gbl/gbl_version.h"
#include "src/ui/mainwindow.h"
#include "src/ui/singleapplication.h"
#include <QFile>

#include <QThread>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(gitbusylivin);

    SingleApplication a(argc, argv);

    if (!a.lock())
        return -42;

    QCoreApplication::setOrganizationName(GBL_APP_COMPANY_NAME);
    QCoreApplication::setApplicationName(GBL_APP_NAME);
    QCoreApplication::setApplicationVersion(QString(GBL_APP_VERSION));

    //a.setWindowIcon(QIcon(":/images/git_busy_livin_logo_16.png"));

    /**/
    MainWindow w;
    QThread::connect(&a, &SingleApplication::applicationStateChanged, &w, &MainWindow::applicationStateChanged);

    w.show();

    return a.exec();
}
