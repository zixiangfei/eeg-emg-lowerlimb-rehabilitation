#ifndef EMGDATAGET_H
#define EMGDATAGET_H

#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")
#include <QObject>
#include <QThread>
#include <QDebug>
#include "PythonGIL/pythreadstatelock.h"

#define TEST qDebug("TEST");

#define CONNECT_SUCC 0
#define CONNECT_FAIL -1
#define RESTING_POTENTIAL 0
#define BEND_POTENTIAL 1
#define STRETCH_POTENTIAL 2

class EmgDataGet : public QThread
{
    Q_OBJECT
public:
    EmgDataGet();
    void run();
    void PythonInit();

public:
    bool running;

signals:
    void BendPotentialSignal();
    void StretchPotentialSignal();
    void RestingPotentialSignal();

};

#endif // EMGDATAGET_H
