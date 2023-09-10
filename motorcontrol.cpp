#include "motorcontrol.h"

/*
 * 初始化电机参数
 */
motorControl::motorControl() {
    motorControl::maxAngle = 130;
    motorControl::minAngle = 8;
    motorControl::speed = 1;
    motorControl::direction = STRETCH_DIRECTION;
    motorControl::runningTime = 1;
    motorControl::enable = false;
}


/*
 * 定时器延时
 */
void motorControl::delay_msec(int msec) {
//    QEventLoop loop;
//    qDebug() << "12345 " << QThread::currentThread() << '\n';
//    QTimer::singleShot(msec, &loop, SLOT(quit()));
//    qDebug() << QThread::currentThreadId() << '\n';
//    loop.exec();
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime() < dieTime) {
        //QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

/*
 * 控制电机运动速度和方向
 */
void motorControl::updateMotorState() {
    digitalWrite(ZFpin, motorControl::direction);
    softPwmWrite(VRpin, motorControl::speed);
    //qDebug() <<"speed:"<< motorControl::speed << "\n";
}

/*
 * AD转换获取输入值
 */
double motorControl::AD_work(unsigned char channel) {
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
 * 获取角度值
 */
double motorControl::getAngle() {
    double AD_value = motorControl::AD_work(PCF8591_A0);
    double angle = (AD_value / 1000) * 64.0 - 190;
    return angle;
}

/*
 * 启动电机
 */
void motorControl::lanuchMotor() {
    digitalWrite(ELpin, HIGH);
}

/*
 * 停止电机
 */
void motorControl::terminateMotor() {
    digitalWrite(ELpin, LOW);
}

/*
 * 电机运行中
 */
void motorControl::run() {

    switch (motorControl::model) {
        case REPEAT_MODE:
        motorControl::repeatModel();
        break;
        case STRETCH_MODE:
        motorControl::stretchModel();
        break;
        case BEND_MODE:
        motorControl::bendModel();
        break;
    }
}

/*
 * 从主线程获取到的控制信息，并更新
 */
void motorControl::motorStateControlSlot(double maxAngle, double minAngle, int speed, int runningTime) {
    motorControl::maxAngle = maxAngle;
    motorControl::minAngle = minAngle;
    motorControl::speed = speed;
    motorControl::runningTime = runningTime;
    motorControl::updateMotorState();
}

/*
 * 选择电机模式
 */
void motorControl::motorModelControlSlot(int model) {
    motorControl::model = model;
}

/*
 * 重复模式
 */
void motorControl::repeatModel() {
    bool running = true;
    bool moving = false; //调头
    int nowTime = 0;
    motorControl::lanuchMotor();
    while(running) {
        //qDebug()<< QThread::currentThread() << " " << "test\n";
        double angle = motorControl::getAngle();
        //角度超过设定的阈值变更方向
        qDebug() << angle <<" "<<motorControl::maxAngle << " " << motorControl::minAngle<<'\n';
        qDebug() << moving <<'\n';
        if((angle < motorControl::minAngle + 1 || angle > motorControl::maxAngle + 1)) {
            if(!moving) {
                moving = true;
                motorControl::terminateMotor();
                if(angle < motorControl::minAngle + 1) {
                    motorControl::direction = STRETCH_DIRECTION;
                }
                if(angle > motorControl::maxAngle - 1) {
                    motorControl::direction = BEND_DIRECTION;
                }
                motorControl::updateMotorState();
                motorControl::delay_msec(DIRECTION_DELAY);
                nowTime += 1000;
                motorControl::lanuchMotor();
            }
        }
        else {
            moving = false;
            motorControl::delay_msec(RUNNING_DELAY);
            nowTime += 1000;
        }
        //超过设定时间
        //qDebug() << nowTime << " " << motorControl::runningTime * 60 * 1000;
        if(nowTime >= motorControl::runningTime * 60 * 1000) {
            running = false;
            motorControl::terminateMotor();
        }
        //QCoreApplication::processEvents();
        //qDebug() << motorControl::direction <<'\n';
        //qDebug() << nowTime << " " << motorControl::maxAngle << " " << motorControl::minAngle << " " << motorControl::speed << " " <<motorControl::runningTime << "\n";
    }
    motorControl::speed = 0;
    motorControl::updateMotorState();
    motorControl::terminateMotor();
}

/*
 * 伸长模式
 */
void motorControl::stretchModel() {
    motorControl::direction = STRETCH_DIRECTION;
    motorControl::updateMotorState();
    motorControl::lanuchMotor();
    while(motorControl::getAngle() < motorControl::maxAngle) {
        //qDebug() << motorControl::direction << " " <<motorControl::getAngle() << '\n';
        //QCoreApplication::processEvents();
        motorControl::delay_msec(RUNNING_DELAY);
    }
    motorControl::terminateMotor();

}

/*
 * 弯曲模式
 */
void motorControl::bendModel() {
    motorControl::direction = BEND_DIRECTION;
    motorControl::updateMotorState();
    motorControl::lanuchMotor();
    while(motorControl::getAngle() > motorControl::minAngle) {
        //qDebug() << motorControl::direction << " " <<motorControl::getAngle() << '\n';
        //QCoreApplication::processEvents();
        motorControl::delay_msec(RUNNING_DELAY);
    }
    motorControl::terminateMotor();
}
