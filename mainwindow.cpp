#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "help.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QComboBox>
#include <QStringList>
#include "timeСlient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setWindowTitle("Client");
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead,this,&MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected,socket,&QTcpSocket::deleteLater);
    nextBlockSize = 0;

    //setupComboBox(ui->);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->lineEdit_2->text() == "")
    {
        socket->connectToHost("127.0.0.1",2323);
    }
    else
    {
        socket->connectToHost(ui->lineEdit_2->text(),2323);
    }

    client = new TimeClient(this);

    // Создаем и настраиваем таймер
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, client, &TimeClient::requestTact);
    timer->start(1000);

}

void MainWindow::SendToServer(QString str)
{
    QJsonObject json;
    json["id"] = 1;  // Пример заполнения
    json["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    json["priority"] = 5;  // Пример заполнения
    json["min_count"] = 10;  // Пример заполнения
    json["tact_number"] = str.toInt();  // Пример, где номер такта из поля ввода

    QJsonDocument doc(json);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << jsonString;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
    ui->lineEdit->clear();
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
            QString timestamp = json["timestamp"].toString();
            int priority = json["priority"].toInt();
            int minCount = json["min_count"].toInt();
            int tactNumber = json["tact_number"].toInt();

            qDebug() << "Received JSON:";
            qDebug() << "ID:" << id;
            qDebug() << "Timestamp:" << timestamp;
            qDebug() << "Priority:" << priority;
            qDebug() << "Min Count:" << minCount;
            qDebug() << "Tact Number:" << tactNumber;

            ui->textBrowser->append("ID: " + QString::number(id));
            ui->textBrowser->append("Timestamp: " + timestamp);
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
    SendToServer(ui->lineEdit->text());
}


void MainWindow::on_pushButton_2_clicked()
{
    SendToServer(ui->lineEdit->text());
}


void MainWindow::on_pushButton_3_clicked()
{
    MyDialog *dialog = new MyDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}


void setupComboBox(QComboBox* comboBox) {
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
