#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showPortInfo(int);
    void fillPortInfo(void);

    void on_connectButton_clicked();
    void read_Serial();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QString serialPortName;
    QByteArray *rData;
};
#endif // MAINWINDOW_H
