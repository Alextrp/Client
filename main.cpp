#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include <QStandardPaths>

QJsonObject loadConfig(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл конфигурации" << filename;
        return QJsonObject();
    }

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Ошибка разбора JSON файла:" << parseError.errorString();
        return QJsonObject();
    }

    return jsonDoc.object();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("Server");
    QCoreApplication::setApplicationVersion("1.0");

    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/projector/client.json";

    QJsonObject config = loadConfig(configPath);

    QCommandLineParser parser;
    parser.setApplicationDescription("Клиент");
    parser.addHelpOption();  // Добавляем опцию -h/--help для отображения справки
    parser.addVersionOption();  // Добавляем опцию --version для отображения версии

    QString defaultPort = config.contains("port") ? QString::number(config["port"].toInt()) : "1234";
    QString defaultLogLevel = config.contains("logLevel") ? QString::number(config["logLevel"].toInt()) : "0";

    // Опция для указания порта для TimeServer
    QCommandLineOption portOption(QStringList() << "p" << "port", "Указать порт для TimeServer.", "port", defaultPort);
    parser.addOption(portOption);

    // Опция для включения отладочной печати
    QCommandLineOption debugOption(QStringList() << "D" << "debug", "Включить отладочную печать.");
    parser.addOption(debugOption);

    // Опция для задания уровня логирования
    QCommandLineOption logLevelOption(QStringList() << "L" << "log-level", "Уровень логирования.", "level", defaultLogLevel);
    parser.addOption(logLevelOption);

    // Разбор аргументов командной строки
    parser.process(a);

    // Проверяем, была ли запрошена справка или версия
    if (parser.isSet("help")) {
        parser.showHelp();  // Показать справку и завершить программу
        return 0;
    }

    if (parser.isSet("version")) {
        parser.showVersion();  // Показать версию и завершить программу
        return 0;
    }


    bool ok;
    quint16 port = parser.value(portOption).toUShort(&ok);
    if (!ok || port == 0) {
        qDebug() << "Ошибка: некорректный номер порта.";
        return 1;
    }


    MainWindow w;

    w.portTime = port;

    if (parser.isSet(debugOption)) {
        w.setLogLevel(1);  // Включаем отладочную печать
    } else {
        w.setLogLevel(parser.value(logLevelOption).toInt());
    }

    w.show();
    return a.exec();
}
