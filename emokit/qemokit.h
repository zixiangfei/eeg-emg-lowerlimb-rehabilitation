#ifndef QEMOKIT_H
#define QEMOKIT_H

#include <QTimer>
#include <QThread>
#include <QDebug>
#include "emokit.h"
#include "emokit_decode.h"
#include "includes.h"

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#define DECISION_NUM 3
using namespace log4cplus;
using namespace log4cplus::helpers;


class QEmokit : public QThread
{
    Q_OBJECT
public:
    explicit QEmokit(QObject *parent = 0);

    ~QEmokit();

    int emokit_init();
    void thread_stop();

signals:
    void batterySignal(unsigned char bat);
    void lowBatterySignal(unsigned char bat);
    void emokit_initMsgSignal(MSG_TYPE type, QString msg);
    void signalQualitySignal(emokit_contact_quality cq);
    void signalValueSignal(emokit_signal es);
    void autoControlSignal(motorControlCmd mcd);


public slots:
    void timerOutSlot();
    void timerOutGraphSlot();

//public:
    //bool
    //thread_state;

private:
    void Log4cplusInit();
private:
    struct emokit_device *emokit_dev;
    int emokit_count;
    struct emokit_signal es;
    bool lowBatteryFlag;

    QTimer *qtimer;
    QTimer *qtimer_Graph;

    Logger _logger;
    struct motorControlCmd mcd;

protected:
    void run();
};
extern bool thread_state;
#endif // QEMOKIT_H
