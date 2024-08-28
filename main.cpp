#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList arguments = a.arguments();

    MainWindow w;

    w.portTime = 65535;

    if (arguments.size() > 1) {
        bool ok;
        quint16 parsedPort = arguments[1].toUShort(&ok);
        if (ok) {
            w.portTime = parsedPort;
        } else {
            qDebug() << "Введите желаемый номер порта. Пример ./Server 65535";
        }
    }

    w.show();
    return a.exec();
}
