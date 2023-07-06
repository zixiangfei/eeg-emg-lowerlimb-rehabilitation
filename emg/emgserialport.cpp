#include "emgserialport.h"

EMGSerialPort::EMGSerialPort(QObject *parent) : QObject(parent)
{
    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &serialPortInfo, serialPortInfos) {
        qDebug() << "name" << serialPortInfo.portName() << '\n';
        qDebug() << "manufacturer" << serialPortInfo.manufacturer() << '\n';
        qDebug() << "description" << serialPortInfo.description() << '\n';
        qDebug() << "systemlocation" << serialPortInfo.systemLocation() << '\n';
    }
}
