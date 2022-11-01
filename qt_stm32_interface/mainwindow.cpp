#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDialog>
#include "qcustomplot.h"    // add chart

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //-------------- setting speed ------------//
    ui->comboBox_speed->addItem("9600", QSerialPort::Baud9600);
    ui->comboBox_speed->addItem("115200", QSerialPort::Baud115200);

    //-------------- setting databit ----------//
    ui->comboBox_databit->addItem("8", QSerialPort::Data8);
    ui->comboBox_databit->addItem("7", QSerialPort::Data7);

    //-------------- setting parity bits ------//
    ui->comboBox_parity->addItem("None", QSerialPort::NoParity);
    ui->comboBox_parity->addItem("Even", QSerialPort::EvenParity);
    ui->comboBox_parity->addItem("Odd", QSerialPort::OddParity);
    //-------------- setting dht11 control-----//
//    ui->comboBox_dht11->addItem("dht11time1500");
//    ui->comboBox_dht11->addItem("dht11time2000");
//    ui->comboBox_dht11->addItem("dht11time3000");


    port = new QSerialPort(this);
    QObject::connect(port, SIGNAL(readyRead()), this, SLOT(text_reading()));
    on_pushButton_scanagain_clicked();

    //--------- Add chart start
    ui->plot1->setInteraction(QCP::iRangeDrag, true);
    ui->plot1->setInteraction(QCP::iRangeZoom, true);
    ui->plot1->addGraph();
    ui->plot1->yAxis->setLabel("온도");
    ui->plot1->yAxis->setRange(0.0, 40.0);   // 0~40 온도범위
    ui->plot1->xAxis->setLabel("시간");
    ui->plot1->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle); // 점 스타일 원으로
    ui->plot1->graph(0)->setLineStyle(QCPGraph::lsLine); // 라인 스타일로
    timeTemp=0;

    ui->plot2->setInteraction(QCP::iRangeDrag, true);
    ui->plot2->setInteraction(QCP::iRangeZoom, true);
    ui->plot2->addGraph();
    ui->plot2->yAxis->setLabel("습도");
    ui->plot2->yAxis->setRange(0.0, 100.0);   // 0~100 습도범위
    ui->plot2->xAxis->setLabel("시간");
    ui->plot2->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle); // 점 스타일 원으로
    ui->plot2->graph(0)->setLineStyle(QCPGraph::lsLine); // 라인 스타일로
    timeHumid=0;

    readState=0;
    //--------- Add chart end

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::text_reading()
{
#if 1
    QList <QByteArray> receivedList;  //  QByteArray를 list 자료구조로 만든다.
    char tmp_ch[6];
    char wet_ch[6];
    char msg[40];
    readState++;
    switch (readState%4)
    {
    case 0:
        ui->label_Alive->setText("--");
        break;
    case 1:
        ui->label_Alive->setText("\\");
        break;
    case 2:
        ui->label_Alive->setText("|");
        break;
    case 3:
        ui->label_Alive->setText("/");
        break;
    }

    while (port->canReadLine())        //  \n이 존재하면
    {
        receivedList.append(port->readLine());   //  [Tmp]\n까지 읽어서 receive
    }

    for(QByteArray received : receivedList) {
        sprintf(msg, "%s", received.data());
        if ( !strncmp(msg, "[Tmp]", strlen("[Tmp]")))
        {
            strcpy(tmp_ch, msg+5);
            int tmp = atoi(tmp_ch);
            ui->lcdNumber_temp->display(tmp);
            //---- chart start
            AddPoint((double) timeTemp, (double) tmp, 1);
            timeTemp += 1;
            ui->plot1->yAxis->setRange(0.0, 40.0);   // y축 범위
            ui->plot1->xAxis->setRange(0.0, timeTemp+2); // x축 확장
            plot(0, 1);
            ui->lcdNumber_temp_current->display((double) tmp);

            if ((double) tmp >= MaxTemp)
            {
                MaxTemp = (double) tmp;
            }
            ui->lcdNumber_temp_max->display(MaxTemp);

            if ((double) tmp <= MinTemp)
            {
                MinTemp = (double) tmp;
            }
            ui->lcdNumber_temp_min->display(MinTemp);
            //---- chart end
        }
        else if ( !strncmp(msg, "[Wet]", strlen("[Wet]")))
        {
            strcpy(wet_ch, msg+5);
            int wet = atoi(wet_ch);
            ui->lcdNumber_wet->display(wet);
            //---- chart start
            AddPoint((double) timeHumid, (double) wet, 2);
            timeHumid += 1;
            ui->plot2->yAxis->setRange(0.0, 100.0);   // y축 범위
            ui->plot2->xAxis->setRange(0.0, timeHumid+2); // x축 확장
            plot(0, 2);
            ui->lcdNumber_humid_current->display((double) wet);

            if ((double) wet >= MaxHumid)
            {
                MaxHumid = (double) wet;
            }
            ui->lcdNumber_humid_max->display(MaxHumid);

            if ((double) wet <= MinHumid)
            {
                MinHumid = (double) wet;
            }
            ui->lcdNumber_humid_min->display(MinHumid);
            //---- chart end
        }

        ui->textEdit_receive->append(received);  //  1개씩 추가
        ui->textEdit_receive->show();      //  display

        qDebug() << received;
    }
#endif
#if 0
    QByteArray received;

    while (port->canReadLine())        //  \n이 존재하면
    {
        received = port->readLine();   //  \n까지 읽어서 receive
    }
    ui->textEdit_receive->append(received);  //  1개씩 추가
    ui->textEdit_receive->show();      //  display

    qDebug() << received;
#endif
}

//---- chart start
void MainWindow::AddPoint(double x, double y, int modeSelect)
{
    switch (modeSelect)
    {
    case 1:
        qv_x_temp.append(x);
        qv_y_temp.append(y);
        break;
    case 2:
        qv_x_humid.append(x);
        qv_y_humid.append(y);
        break;
    }
}

void MainWindow::plot(int graphSelect, int plotSelect)   // 그래프 그리기
{
    switch (plotSelect)
    {
    case 1:
        ui->plot1->graph(graphSelect)->setData(qv_x_temp, qv_y_temp);
        ui->plot1->replot();
        ui->plot1->update();
        break;
    case 2:
        ui->plot2->graph(graphSelect)->setData(qv_x_humid, qv_y_humid);
        ui->plot2->replot();
        ui->plot2->update();
        break;
    }
}


//---- chart end

void MainWindow::on_pushButton_scanagain_clicked()
{
    ui->comboBox_port->clear();  //  combobox clear
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        ui->comboBox_port->addItem(serialPortInfo.portName());
    }
}

void MainWindow::on_pushButton_open_clicked()
{
    port->setPortName(ui->comboBox_port->currentText());

    if (ui->comboBox_speed->currentIndex() == 0)
        port->setBaudRate(QSerialPort::Baud9600);
    else if (ui->comboBox_speed->currentIndex() == 1)
        port->setBaudRate(QSerialPort::Baud115200);

    if (ui->comboBox_databit->currentIndex() == 0)
        port->setDataBits(QSerialPort::Data8);
    else if (ui->comboBox_databit->currentIndex() == 1)
        port->setDataBits(QSerialPort::Data7);

    if (ui->comboBox_parity->currentIndex() == 0)
        port->setParity(QSerialPort::NoParity);
    else if (ui->comboBox_parity->currentIndex() == 1)
        port->setParity(QSerialPort::EvenParity);
    else if (ui->comboBox_parity->currentIndex() == 2)
        port->setParity(QSerialPort::OddParity);

    if (!port->open(QIODevice::ReadWrite))  //  ==0
    {
        qDebug() << "\n Serial port open error! \n";
    }
    else
    {
        ui->progressBar->setValue(100);
        qDebug() << "\n Serial port open success! \n";
    }
}

void MainWindow::on_pushButton_close_clicked()
{
    port->close();
    ui->progressBar->setValue(0);
}

void MainWindow::on_pushButton_send_clicked()
{
    QByteArray sendData;

    sendData = QString(ui->lineEdit_send->text() + "\n").toUtf8();  //  한글 컨버젼, 한글 아닐때는 관계x
    port->write(sendData.data());

    qDebug() << sendData;
}

void MainWindow::on_pushButton_clear_clicked()
{
    ui->lineEdit_send->setText("");
    ui->textEdit_receive->setText("");
}

void MainWindow::on_pushButton_dht11_clicked()
{
    QByteArray sendData1;

    if (ui->comboBox_dht11->currentIndex() == 0)
    {
    sendData1 = QString(ui->lineEdit_send->text() + "dht11time1500\n").toUtf8();
    port->write(sendData1.data());
    qDebug() << sendData1;
    }
    else if (ui->comboBox_dht11->currentIndex() == 1)
    {
    sendData1 = QString(ui->lineEdit_send->text() + "dht11time2000\n").toUtf8();
    port->write(sendData1.data());
    qDebug() << sendData1;
    }
    else if (ui->comboBox_dht11->currentIndex() == 2)
    {
    sendData1 = QString(ui->lineEdit_send->text() + "dht11time2500\n").toUtf8();
    port->write(sendData1.data());
    qDebug() << sendData1;
    }
    else if (ui->comboBox_dht11->currentIndex() == 3)
    {
    sendData1 = QString(ui->lineEdit_send->text() + "dht11time3000\n").toUtf8();
    port->write(sendData1.data());
    qDebug() << sendData1;
    }
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    QByteArray sendData2;
    switch(arg1)
    {
        case Qt::Unchecked :
            sendData2 = QString("led2\n").toUtf8();
            port->write(sendData2.data());
            ui->progressBar_led->setValue(0);
            break;

        case Qt::Checked :
            sendData2 = QString("led2\n").toUtf8();
            port->write(sendData2.data());
            ui->progressBar_led->setValue(100);
            break;
    }
    qDebug() << sendData2;
}

void MainWindow::on_pushButton_clear_temp_clicked()
{
    qv_x_temp.clear();
    qv_y_temp.clear();
    timeTemp=0;
}

void MainWindow::on_pushButton_clear_humid_clicked()
{
    qv_x_humid.clear();
    qv_y_humid.clear();
    timeHumid=0;
}
