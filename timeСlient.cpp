#include "timeСlient.h"

TimeClient::TimeClient(QObject *parent) : QObject(parent) {
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead, this, &TimeClient::processPendingDatagrams);

}

void TimeClient::requestTact(quint16 portTime) {
    QJsonObject jsonObject;
    jsonObject["type"] = "get_tact";

    QJsonDocument jsonDoc(jsonObject);
    qint64 bytesWritten = udpSocket->writeDatagram(jsonDoc.toJson(), QHostAddress("127.0.0.1"), portTime);

    if (bytesWritten == -1) {
        qDebug() << "Failed to send request for synchronized tact. Error:" << udpSocket->errorString();
    } else {
        qDebug() << "Sent request for synchronized tact.";
    }
}

void TimeClient::processPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(datagram.data());
        QJsonObject jsonObject = jsonDoc.object();

        qint64 syncedTact = jsonObject["tact"].toVariant().toLongLong();
        qDebug() << "Received synchronized tact:" << syncedTact;
        takt = syncedTact;
    }
}
