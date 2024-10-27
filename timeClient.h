#ifndef TIMECLIENT_H
#define TIMECLIENT_H

#include <QObject>
#include <QUdpSocket>

class TimeClient : public QObject {
    Q_OBJECT

public:
    explicit TimeClient(QObject *parent = nullptr);

    void requestTact(quint16 portTime);   // Запрос такта с сервера
    bool checkUdpPort(quint16 portTime);  // Проверка доступности UDP порта
    quint64 takt;                         // Синхронизированный такт
    QString status;                       // Статус соединения

private slots:
    void processPendingDatagrams();       // Обработка входящих данных
    //void handleSocketError(QAbstractSocket::SocketError socketError); // Обработка ошибок сокета

private:
    QUdpSocket *udpSocket;                // UDP сокет для работы

signals:
    void udpResponseReceived(bool success);  // Сигнал для уведомления о результате
};

#endif // TIMECLIENT_H
