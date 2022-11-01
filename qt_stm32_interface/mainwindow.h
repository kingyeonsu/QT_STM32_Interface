#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort> // Add
#include <qserialportinfo.h>        // Add
#include <stdio.h>                  // Add
#include <QMessageBox>              // Add
#include <QDebug>                   // Add

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSerialPort *port;  //Add
    //---- chart start
    void AddPoint(double x, double y, int modeSelect);
    void plot(int graphSelect, int plotSelect);
    //---- chart end
private slots:
    void text_reading();

    void on_pushButton_scanagain_clicked();

    void on_pushButton_open_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_send_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_dht11_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_clear_temp_clicked();

    void on_pushButton_clear_humid_clicked();

private:
    Ui::MainWindow *ui;
    //---- chart start
    QVector <double> qv_x_temp, qv_y_temp;    // 온도 x,y 축 좌표값
    QVector <double> qv_x_humid, qv_y_humid;    // 습도 x,y 축 좌표값
    QString temp;   // 온도
    QString humid;   // 습도
    double MaxTemp=0;
    double MinTemp=40;
    double timeTemp=0;  // 시간축을(qv_x) 가리킨다.
    double MaxHumid=0;
    double MinHumid=100;
    double timeHumid=0;  // 시간축을(qv_x) 가리킨다.
    int readState=0;


    //  QVector <int> vector (1,2,3,...);   동적 메모리 할당
    //---- chart end

};
#endif // MAINWINDOW_H
