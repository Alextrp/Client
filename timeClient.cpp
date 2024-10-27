#include "timeClient.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QEventLoop>
#include <QTimer>
#include <QNetworkDatagram>
#include <QDebug>

TimeClient::TimeClient(QObject *parent) : QObject(parent) {
    udpSocket = new QUdpSocket(this);

    // Соединяем сигналы с обработчиками
    connect(udpSocket, &QUdpSocket::readyRead, this, &TimeClient::processPendingDatagrams);
}

void TimeClient::requestTact(quint16 portTime) {
    // Создаем JSON-запрос
    QJsonObject jsonObject;
    jsonObject["type"] = "get_tact";

    QJsonDocument jsonDoc(jsonObject);
    udpSocket->writeDatagram(jsonDoc.toJson(), QHostAddress("127.0.0.1"), portTime);
}

void TimeClient::processPendingDatagrams() {
    bool tactReceived = false;

    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(datagram.data());
        QJsonObject jsonObject = jsonDoc.object();

        qDebug() << "Received data:" << datagram.data();

        if (jsonObject.contains("tact")) {
            quint64 syncedTact = jsonObject["tact"].toVariant().toLongLong();
            qDebug() << "Received synchronized tact:" << syncedTact;
            takt = syncedTact;
            tactReceived = true;
        }
    }

    if (tactReceived) {
        emit udpResponseReceived(true);  // Сообщаем, что ответ корректен
    } else {
        emit udpResponseReceived(false); // Сообщаем об ошибке
    }
}

bool TimeClient::checkUdpPort(quint16 portTime) {
    // Создаем тестовое сообщение для проверки доступности порта
    QJsonObject jsonObject;
    jsonObject["type"] = "check_port";

    QJsonDocument jsonDoc(jsonObject);
    udpSocket->writeDatagram(jsonDoc.toJson(), QHostAddress("127.0.0.1"), portTime);

    // Ожидаем ответа
    QEventLoop loop;
    connect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);

    QTimer timer;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(5000); // Ожидаем 5 секунд

    loop.exec();  // Блокируем выполнение до ответа или таймаута

    // Проверяем, был ли получен ответ
    return udpSocket->hasPendingDatagrams();
}
