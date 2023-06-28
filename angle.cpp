#include "angle.h"

/*
 *  完成pcf8574的初始化
 */
Angle::Angle() {
}

/*
 * 连续读取10次的模拟值求平均作为当前角度的模拟值
 */
double Angle::AD_work(unsigned char channel) {
    double AD_val = 0;
    unsigned char i;
    for(i = 0; i < 10; ++i) {
        AD_val += analogRead(channel);
    }
    AD_val /= 10;
    AD_val = (AD_val * POWER) / 255;
    return AD_val;
}

/*
 * 线程执行的函数获取模拟值转换为角度大小，并发射给主线程
 */
void Angle::run() {
    for(;;) {
        Angle::AD_val = Angle::AD_work(PCF8574_A0);
        double angle = (Angle::AD_val / 1000) * 64.0 - 160;
        emit Angle_val(angle);
        qDebug() << angle <<'\n';
    }
}

