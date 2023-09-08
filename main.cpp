#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")
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

//python只需要初始化一次
void PythonInit() {
    if(!Py_IsInitialized()) {
        //1.初始化python解释器，调用操作的第一步
        Py_Initialize();
        if(!Py_IsInitialized()) {
            qDebug("Python初始化失败");
        }
        else {
            // 初始化线程支持
            PyEval_InitThreads();
            // 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁
            PyEval_ReleaseThread(PyThreadState_Get());
            qDebug("初始化Python解释器成功");
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PythonInit();
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
