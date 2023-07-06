#ifndef EMGSERIALPORT_H
#define EMGSERIALPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

class EMGSerialPort : public QObject
{
    Q_OBJECT
public:
    explicit EMGSerialPort(QObject *parent = nullptr);

signals:

};

#endif // EMGSERIALPORT_H
