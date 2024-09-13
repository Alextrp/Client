#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <timeClient.h>
#include <QComboBox>
#include <QRandomGenerator>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void setupComboBox(QComboBox* comboBox);
    ~MainWindow();
    qint64 portTime;

private slots:
    void on_pushButton_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_2_clicked();

    void updateTaktLabel();

    //void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QByteArray Data;
    void SendToServer();
    quint16 nextBlockSize;
    QTimer *timer;
    TimeClient *client;


public slots:
    void slotReadyRead();
    void handleServerDisconnected();
    void handleConnected();
    void handleError(QAbstractSocket::SocketError socketError);
};
#endif // MAINWINDOW_H
