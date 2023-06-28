#include "mainwindow.h"
#include <QApplication>
#include <pcf8591.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <motorcontrol.h>
#define ZFpin 4
#define VRpin 1
#define ELpin 5
//PCF8591默认i2c设备地址
//VCC -> 5v
//GND -> GND
//SCL -> SCL
//SDA -> SDA
#define PCF8591_ADDRESS 0x48

//模拟信号输入端
#define PCF8591_BASE 0x40

int main(int argc, char *argv[])
{
    qDebug()<< QThread::currentThreadId() << " " << "test\n";
    QApplication a(argc, argv);
    wiringPiSetup();
    pcf8591Setup(PCF8591_BASE, PCF8591_ADDRESS);
    pinMode(ZFpin, OUTPUT);
    pinMode(VRpin, OUTPUT);
    pinMode(ELpin, OUTPUT);
    softPwmCreate(VRpin, 0, 10);
    MainWindow w;
    w.show();
    //w.showFullScreen();
    //test
    return a.exec();
}
