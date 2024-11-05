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

    // Метод для запроса синхронизации такта с сервером
    void requestTact(quint16 portTime);
    qint64 takt = 0;          // Текущее значение такта

signals:
    // Сигнал для оповещения об обновленном такте
    void tactUpdated(qint64 newTact);

private slots:
    // Обработка полученных данных
    void processPendingDatagrams();

    // Обработка ошибок сокета
    void handleSocketError(QAbstractSocket::SocketError socketError);

private:
    QUdpSocket *udpSocket;    // Сокет для передачи данных по UDP
    QString status;           // Статус подключения
};

#endif // TIMECLIENT_H
