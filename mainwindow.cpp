#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "help.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QComboBox>
#include <QStringList>
#include "timeClient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setWindowTitle("Client");
    ui->setupUi(this);
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::handleServerDisconnected);
    connect(socket, &QTcpSocket::connected, this, &MainWindow::handleConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::handleError);

    nextBlockSize = 0;
    setupComboBox(ui->comboBox);
    client = new TimeClient(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTaktLabel);

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if (ui->pushButton->isChecked()) {
        // Подключение к серверу
        QString hostAddress = ui->lineEdit_2->text().isEmpty() ? "127.0.0.1" : ui->lineEdit_2->text();
        socket->connectToHost(hostAddress, 2323);

        client->requestTact(portTime);
        timer->start(1000);

        ui->pushButton->setText("Disconnect");
        if(logLevel>0)
            qDebug() << "Attempting to connect to server...";
    } else {
        // Отключение от сервера
        socket->disconnectFromHost();
        if (socket->state() != QAbstractSocket::UnconnectedState) {
            socket->waitForDisconnected(3000);
        }
        timer->stop();
        ui->pushButton->setText("Connect");

        if(logLevel>0)
            qDebug() << "Disconnected from server.";
    }
}

void MainWindow::handleConnected()
{

    qDebug() << "Успешное подключение к серверу!";
    ui->textBrowser->append("Успешное подключение к серверу!");
}

void MainWindow::handleServerDisconnected()
{
    qDebug() << "Сервер выключен!";
    ui->textBrowser->append("Сервер выключен!");
}

void MainWindow::handleError(QAbstractSocket::SocketError socketError)
{
    QString errorString;

    switch(socketError)
    {
    case QAbstractSocket::ConnectionRefusedError:
        errorString = "Сервер отказал в подключении.";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorString = "Сервер закрыл соединение.";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorString = "Хост не найден.";
        break;
    case QAbstractSocket::NetworkError:
        errorString = "Произошла ошибка сети.";
        break;
    default:
        errorString = socket->errorString();
        break;
    }

    qDebug() << "Socket error:" << errorString;
    ui->textBrowser->append("Socket error: " + errorString);

}

void MainWindow::updateTaktLabel()
{
    client->takt += 1;
    ui->label->setText(QString::number(client->takt));
}

void MainWindow::setLogLevel(int level) {
    logLevel = level;
}

void MainWindow::SendToServer()
{
    if(socket->state() == QAbstractSocket::ConnectedState) {
        timer->stop();
        int tmp = client->takt;
        client->requestTact(portTime);

        int tmp2 = tmp - client->takt;

        if(tmp2 != 0)
        {
            ui->textBrowser->append("Время было откорректировано на " + QString::number(tmp2));
        }

        QJsonObject json;
        json["id"] = QRandomGenerator::global()->bounded(1, QDateTime::currentSecsSinceEpoch());
        json["timestamp"] = client->takt;
        json["config"] = ui->comboBox->currentText();
        json["priority"] = ui->comboBox_2->currentText().toInt();
        json["min_count"] = 10;
        json["tact_number"] = 0;

        QJsonDocument doc(json);
        QString jsonString = doc.toJson(QJsonDocument::Compact);

        Data.clear();
        QDataStream out(&Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_2);
        out << quint16(0) << jsonString;
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
        socket->write(Data);
        timer->start(1000);
    } else {
        ui->textBrowser->append("Вы не подключены к серверу.");
    }
}


void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);
    if(in.status() == QDataStream::Ok)
    {
        for(;;)
        {
            if(nextBlockSize == 0)
            {
                if(socket->bytesAvailable() < 2)
                {
                    break;
                }
                in >> nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize)
            {
                break;
            }

            QString jsonString;
            in >> jsonString;
            nextBlockSize = 0;

            QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
            QJsonObject json = doc.object();

            int id = json["id"].toInt();
            quint64 timestamp = json["timestamp"].toInt();
            QString config = json["config"].toString();
            int priority = json["priority"].toInt();
            int minCount = json["min_count"].toInt();
            int tactNumber = json["tact_number"].toInt();


            if(logLevel > 0)
            {
                qDebug() << "Received JSON:";
                qDebug() << "ID:" << id;
                qDebug() << "Timestamp:" << timestamp;
                qDebug() << "Priority:" << priority;
                qDebug() << "Min Count:" << minCount;
                qDebug() << "Tact Number:" << tactNumber;
            }

            ui->textBrowser->append("ID: " + QString::number(id));
            ui->textBrowser->append("Timestamp: " + QString::number(timestamp));
            ui->textBrowser->append("Config: " + config);
            ui->textBrowser->append("Priority: " + QString::number(priority));
            ui->textBrowser->append("Min Count: " + QString::number(minCount));
            ui->textBrowser->append("Tact Number: " + QString::number(tactNumber));
        }
    }
    else
    {
        ui->textBrowser->append("read error");
    }
}



void MainWindow::on_lineEdit_returnPressed()
{
    SendToServer();
}


void MainWindow::on_pushButton_2_clicked()
{
    SendToServer();
}


void MainWindow::setupComboBox(QComboBox* comboBox) {
    QStringList items;
    items << "одна строка"
          << "две строки"
          << "три строки"
          << "четыре строки"
          << "один столбец"
          << "два столбца"
          << "три столбца"
          << "четыре столбца"
          << "один прожектор"
          << "прямоугольник 1x2"
          << "прямоугольник 1x3"
          << "прямоугольник 2x3"
          << "квадрат 2x2"
          << "квадрат 3x3"
          << "квадрат 4x4"
          << "квадрат 8x8";

    comboBox->addItems(items);
}
