#include "timeСlient.h"

TimeClient::TimeClient(QObject *parent) : QObject(parent) {
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead, this, &TimeClient::processPendingDatagrams);

    requestTact();
}

void TimeClient::requestTact() {
    QJsonObject jsonObject;
    jsonObject["type"] = "get_tact";

    QJsonDocument jsonDoc(jsonObject);
    udpSocket->writeDatagram(jsonDoc.toJson(), QHostAddress("127.0.0.1"), 45454);

    qDebug() << "Sent request for synchronized tact.";
}

void TimeClient::processPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(datagram.data());
        QJsonObject jsonObject = jsonDoc.object();

        qint64 syncedTact = jsonObject["tact"].toVariant().toLongLong();
        qDebug() << "Received synchronized tact:" << syncedTact;
    }
}
