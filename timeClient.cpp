#include "timeClient.h"

TimeClient::TimeClient(QObject *parent) : QObject(parent) {
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead, this, &TimeClient::processPendingDatagrams);
    connect(udpSocket, &QUdpSocket::errorOccurred, this, &TimeClient::handleSocketError);
}

void TimeClient::requestTact(quint16 portTime) {
    QJsonObject jsonObject;
    jsonObject["type"] = "get_tact";

    QJsonDocument jsonDoc(jsonObject);
    qint64 bytesWritten = udpSocket->writeDatagram(jsonDoc.toJson(), QHostAddress("127.0.0.1"), portTime);

    if (bytesWritten == -1) {
        qDebug() << "Failed to send request for synchronized tact. Error:" << udpSocket->errorString();
        status = "Failed";
    } else {
        qDebug() << "Sent request for synchronized tact.";

    }
}

void TimeClient::processPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(datagram.data());
        QJsonObject jsonObject = jsonDoc.object();

        if (jsonObject.contains("tact")) {
            qint64 syncedTact = jsonObject["tact"].toVariant().toLongLong();
            qDebug() << "Received synchronized tact:" << syncedTact;
            takt = syncedTact;
        } else {
            qDebug() << "Received unexpected data:" << datagram.data();
            takt = 0;
        }
    }
}

void TimeClient::handleSocketError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "Error: хост не найден.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "Error: сервер не принял запрос на подключение.";
        break;
    case QAbstractSocket::NetworkError:
        qDebug() << "Error: ошибка сети.";
        break;
    case QAbstractSocket::DatagramTooLargeError:
        qDebug() << "Error: превышен размер отправляемой информации.";
        break;
    default:
        qDebug() << "An unknown socket error occurred:" << udpSocket->errorString();
        break;
    }
}
