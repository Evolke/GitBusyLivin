#include "src/ui/mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(gitbusylivin);

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Evolke");
    QCoreApplication::setApplicationName("GitBusyLivin");
    QCoreApplication::setApplicationVersion("0.0.1");

    QFile file(":/styles/shawshank.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    a.setStyleSheet(styleSheet);
    MainWindow w;
    w.show();

    return a.exec();
}
