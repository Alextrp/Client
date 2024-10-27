#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <timeClient.h>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setLogLevel(int level);          // Установить уровень логирования
    void setupComboBox(QComboBox* comboBox);
    quint16 portTime;


private slots:
    void on_pushButton_clicked();         // Подключение/Отключение от TCP сервера
    void on_pushButton_2_clicked();       // Отправка данных на TCP сервер
    void updateTaktLabel();               // Обновление метки с текущим тактом
    //void handleUdpResponse(bool success); // Обработка ответа от UDP сервера

    void slotReadyRead();                 // Обработка данных от TCP сервера
    void handleServerDisconnected();      // Обработка отключения от сервера
    void handleConnected();               // Обработка успешного подключения
    void handleError(QAbstractSocket::SocketError socketError); // Обработка ошибок

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;                   // TCP сокет для соединения
    QByteArray Data;                      // Данные для отправки на сервер
    quint16 nextBlockSize;                // Размер блока для чтения данных
    QTimer *timer;                        // Таймер для обновления тактов
    TimeClient *client;                   // Клиент для синхронизации времени через UDP
    int logLevel;                         // Уровень логирования
    void SendToServer();                  // Функция отправки данных на TCP сервер
};

#endif // MAINWINDOW_H
