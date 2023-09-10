#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include<QtCore/QCoreApplication>
#include<QThread>
#include<QDebug>
#include<wiringPi.h>
#include<pcf8591.h>
#include<softPwm.h>
#include<qtimer.h>
#include<QTime>

#define ZFpin 1 //方向 12
#define VRpin 4 //速度 16
#define ELpin 5 //使能 18
//PCF8591默认i2c设备地址
//VCC -> 5v
//GND -> GND
//SCL -> SCL 5
//SDA -> SDA 3
#define PCF8591_ADDRESS 0x48

//模拟信号输入端
#define PCF8591_BASE 0x40
#define PCF8591_A0 0x40
#define PCF8591_A1 0x41
#define PCF8591_A2 0x42
#define PCF8591_A3 0x43

//电源电压
#define POWER 5000

//定义延时时长
#define RUNNING_DELAY 1000
#define DIRECTION_DELAY 1000

//模式选择
#define REPEAT_MODE 1
#define STRETCH_MODE 2
#define BEND_MODE 3

//方向
#define STRETCH_DIRECTION 0 //拉伸方向
#define BEND_DIRECTION 1    //弯曲方向

class motorControl : public QThread
{
    Q_OBJECT
public:
    motorControl();
    void run();
    void lanuchMotor();
    void terminateMotor();

public:
    double maxAngle;
    double minAngle;
    int speed;
    int direction;
    int runningTime;
    int enable;
    int model;

    void updateMotorState();
    double getAngle();
    double AD_work(unsigned char channel);
    void delay_msec(int msec);
    void repeatModel();
    void bendModel();
    void stretchModel();

private slots:
    void motorStateControlSlot(double maxAngle, double minAngle, int speed, int runningTime);
    void motorModelControlSlot(int model);
};

#endif // MOTORCONTROL_H
