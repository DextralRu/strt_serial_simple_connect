#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include "packet.h"

static const char blankString[] = "N/A";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort)
{
    ui->setupUi(this);

    connect(ui->SerialPortComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::showPortInfo);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::read_Serial);

    rData = new QByteArray;
 //   ui->descriptionLabel->hide();
//   ui->manufacturerLabel->hide();

   fillPortInfo();


 }

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showPortInfo(int idx)
{
    if (idx == -1)
        return;
    serial->close();
    const QStringList list = ui->SerialPortComboBox->itemData(idx).toStringList();
    ui->descriptionLabel->setText(ui->descriptionLabel->text().split(':').first() + QString(": %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    ui->manufacturerLabel->setText(ui->manufacturerLabel->text().split(':').first() + QString(": %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    serialPortName = ui->SerialPortComboBox->currentText();
    connectPort();
}

void MainWindow::fillPortInfo()
{
    ui->SerialPortComboBox->clear();
    ui->SerialPortComboBox->addItem("");
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString);

        ui->SerialPortComboBox->addItem(list.first(), list);
        ui->descriptionLabel->setText(ui->descriptionLabel->text().split(':').first() + QString(": %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
        ui->manufacturerLabel->setText(ui->manufacturerLabel->text().split(':').first() + QString(": %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    }
}

void MainWindow::connectPort()
{
    serial->setPortName(serialPortName);
    qDebug() << serialPortName;
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);
    ui->statusbar->showMessage(QString("????????????: ???????? %1").arg(serial->isOpen()?"????????????":"????????????"));
}

void MainWindow::on_connectButton_clicked()
{
    BLE::Packet *command = nullptr;
    command = new BLE::PacketPollUSB();
    if(command)
    {
        serial->write(((BLE::PacketOutgoing*)command)->data());
        qDebug() << "SEND: " << ((BLE::PacketOutgoing*)command)->data().toHex();
    }
}

void MainWindow::read_Serial()
{
    rData->append(serial->readAll());
    serial->clear();
    qDebug() << rData->toHex();
    rData->clear();
}
