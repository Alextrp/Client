#ifndef TIMECLIENT_H
#define TIMECLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkDatagram>
#include <QDebug>

class TimeClient : public QObject {
    Q_OBJECT

public:
    explicit TimeClient(QObject *parent = nullptr);

    void requestTact(quint16 portTime);
    qint64 takt;

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *udpSocket;
};

#endif // TIMECLIENT_H
