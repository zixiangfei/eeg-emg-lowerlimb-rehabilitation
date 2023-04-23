#ifndef QSERIALPORTHELPER_H
#define QSERIALPORTHELPER_H

#include "includes.h"
#include <QString>
#include <QExtSerialPort/posix_qextserialport.h>
#include <QObject>

#define YTK_F_START_STOP    0x3F
#define YTK_F_BEND          0xBE
#define YTK_F_STRETCH       0x5F

class QSerialportHelper:public QObject
{
    Q_OBJECT
public:
    QSerialportHelper(QObject *parent = 0);
    ~QSerialportHelper();

    void QSerialPortInit();
    void QSerialPortOpen();
    void QSerialPortClose();
    void QSerialPortWriteChar(char c);
    void QSerialPortReadChar(char *c);
    void QSerialPortWrite(const char *c);
    QString QSerialPortReadAll();

    void YTK_F_Start_Stop();
    void YTK_F_Bend();
    void YTK_F_Stretch();

private:
    bool QSerialPortState;
    QString QSerialPortName;
    struct PortSettings QSerialPortSettings;
    Posix_QextSerialPort *QSerialPort;
 public slots:
    void manualControlSlot(motorControlCmd mcd);
    void autoControlSlot(motorControlCmd mcd);
    void runFinishSlot(motorControlCmd mcd);

};

#endif // QSERIALPORTHELPER_H
