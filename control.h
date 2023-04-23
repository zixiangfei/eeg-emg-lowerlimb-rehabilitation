#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include "includes.h"
#include "SerialPort/qserialporthelper.h"
#include <QSettings>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
using namespace log4cplus;
using namespace log4cplus::helpers;

namespace Ui {
class Control;
}

class Control : public QWidget
{
    Q_OBJECT

public:
    explicit Control(QWidget *parent = 0);
    ~Control();

private slots:
    void on_auto_radioButton_clicked();

    void on_manual_radioButton_clicked();

    void on_start_auto_pushButton_clicked();

    void on_stop_auto_pushButton_clicked();

    void manualStartStopSlot();

    void manualStretchSlot();

    void manualBendSlot();

    void speendValueSlot(int arg);

    void timeValueSlot(int arg);

    void stretchValueSlot(int arg);

    void bendValueSlot(int arg);

    void autoButtonStateSlot(bool startButtonState);

    void timeSpinBoxSlot();

private:
    void Log4cplusInit();
signals:
    void ControlMsgSignal(MSG_TYPE type, QString msg);
    void startEEGSiganl();
    void stopEEGsignal();
    void manualControlSignal(motorControlCmd mcd);
    void startTimerSignal(int interval);

private:
    Ui::Control *ui;
    //int controlModel;
    Logger _logger;
    //QSerialportHelper *qSerialportHelper;
   // bool startOrstop;
    struct motorControlCmd mcd;
    QSettings *settings;

};
extern int controlModel;
extern bool startOrstop;
extern int timeValue;
#endif // CONTROL_H
