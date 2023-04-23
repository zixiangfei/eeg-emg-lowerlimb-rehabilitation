#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SystemInfo/systeminfo.h"
#include <QStatusBar>
#include <QSettings>
#include <QTimer>
#include <QThread>
#include "control.h"
#include "eeg.h"
#include "emg.h"
#include "setting.h"
#include "includes.h"
#include "emokit/qemokit.h"
#include "SerialPort/qserialporthelper.h"

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
using namespace log4cplus;
using namespace log4cplus::helpers;



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void loadStyleSheet(QString name);
    void Log4cplusInit();

private slots:
    void CPUUsageSlot(double u);
    void MEMUsageSlot(double u);

    void on_pushButton1_clicked();
    void on_pushButton2_clicked();
    void on_pushButton3_clicked();
    void on_pushButton4_clicked();
    void on_Back_pushButton_clicked();

    void stateShowMessageSlot(MSG_TYPE type,QString msg);
    void systemInfoShowSlot(bool s);
    void eegStartSlot();
    void eegStopSlot();
    void lowBatterySlot(unsigned char bat);
    void timeOutRunSlot();
  signals:
    void autoButtonStateSignal(bool startButtonState);
    void runFinishSignal(motorControlCmd mcd);
    void timeSpinBoxSinganl();
private:
    Ui::MainWindow *ui;
    SystemInfo *systemInfo;
    QStatusBar *stateBar;
    Logger _logger;

    Control *ControlWidget;
    EEG *EEGWidget;
    EMG *EMGWidget;
    Setting *SettingWidget;
    int currentWidget;

    QSettings *settings;
    QEmokit *qemokit;

    QSerialportHelper *qSerialportHelper;
    QTimer *runTimer;
    QThread *timeThread;
    struct motorControlCmd mcd;

};
#endif // MAINWINDOW_H
