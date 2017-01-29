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

    QFile file(":/styles/shawshank.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    a.setStyleSheet(styleSheet);
    MainWindow w;
    w.show();

    return a.exec();
}
