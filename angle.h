#ifndef ANGLE_H
#define ANGLE_H

//使用pcf8574 AD转换器获取角度传感器的值
#include<pcf8574.h>
#include<wiringPi.h>
#include<QThread>
#include<QtCore/QCoreApplication>
#include<QDebug>

#define ZFpin 4
#define VRpin 1
//PCF8591默认i2c设备地址
//VCC -> 5v
//GND -> GND
//SCL -> SCL
//SDA -> SDA
#define PCF8574_ADDRESS 0x48

//模拟信号输入端
#define PCF8574_BASE 0x40
#define PCF8574_A0 0x40
#define PCF8574_A1 0x41
#define PCF8574_A2 0x42
#define PCF8574_A3 0x43

//供电5v
#define POWER 5000

class Angle : public QThread {
    Q_OBJECT
public:
    Angle();
    void run();

private:
    double AD_work(unsigned char channel);
    double AD_val;
signals:
    void Angle_val(double);
};

#endif // ANGLE_H
