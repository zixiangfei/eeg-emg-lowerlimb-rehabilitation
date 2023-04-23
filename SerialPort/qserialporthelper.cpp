/*************************************************************************************************
Copyright (C),  2015,  HangZhou DianZi University.
File name:  qserialporthelper.cpp  qserialporthelper.h
Author:  zlj 	  Version: V1.0    Date: 2015-11-10
Description: qserialporthelper


Others:

History:
      Date:        Author:       Version：         Modification:
1.  2015-11-10      zlj			   V1.0        		  建库
*************************************************************************************************/
#include "qserialporthelper.h"
#include <QDebug>


/**
 * @brief QSerialportHelper::QSerialportHelper
 */
QSerialportHelper::QSerialportHelper(QObject *parent) :
    QObject(parent)
{
    QSerialPortState = false;
    QSerialPortSettings.BaudRate = BAUD115200;
    QSerialPortSettings.DataBits = DATA_8;
    QSerialPortSettings.Parity = PAR_NONE;
    QSerialPortSettings.StopBits = STOP_1;
    QSerialPortSettings.FlowControl = FLOW_OFF;
    QSerialPortSettings.Timeout_Millisec = 20;

    QSerialPortInit();
}

/**
 * @brief QSerialportHelper::~QSerialportHelper
 */
QSerialportHelper::~QSerialportHelper()
{
    QSerialPortClose();
    delete QSerialPort;
}

/**
 * @brief QSerialportHelper::QSerialPortInit
 */
void QSerialportHelper::QSerialPortInit()
{
    QSerialPortName = "/dev/ttyAMA0";
    QSerialPort = new Posix_QextSerialPort(QSerialPortName, QextSerialBase::Polling);
}

/**
 * @brief QSerialportHelper::QSerialPortOpen
 */
void QSerialportHelper::QSerialPortOpen()
{
    if(QSerialPortState == false)
    {
        if(QSerialPort->open(QIODevice::ReadWrite))
        {
            QSerialPortState = true;
            QSerialPort->setBaudRate(QSerialPortSettings.BaudRate);
            QSerialPort->setParity(QSerialPortSettings.Parity);
            QSerialPort->setStopBits(QSerialPortSettings.StopBits);
            QSerialPort->setDataBits(QSerialPortSettings.DataBits);
            QSerialPort->setFlowControl(QSerialPortSettings.FlowControl);
            QSerialPort->setTimeout(QSerialPortSettings.Timeout_Millisec);
        }
    }
}

/**
 * @brief QSerialportHelper::QSerialPortClose
 */
void QSerialportHelper::QSerialPortClose()
{
    if(QSerialPortState == true)
    {
        QSerialPort->close();
        QSerialPortState = false;
    }
}

/**
 * @brief QSerialportHelper::QSerialPortWriteChar
 * @param c
 */
void QSerialportHelper::QSerialPortWriteChar(char c)
{
    if(QSerialPortState == true)
    {
        QSerialPort->putChar(c);

    }
}

/**
 * @brief QSerialportHelper::QSerialPortReadChar
 * @return
 */
void QSerialportHelper::QSerialPortReadChar(char *c)
{
    if(QSerialPortState == true)
    {
        QSerialPort->getChar(c);
    }
    else
    {
        *c = 0;
    }
}

void QSerialportHelper::QSerialPortWrite(const char *c)
{
    QSerialPort->write(c);
    QSerialPort->flush();
}

QString QSerialportHelper::QSerialPortReadAll()
{
    QByteArray recv = QSerialPort->readAll();
    QString string = QString(recv);
    return string;
}

void QSerialportHelper::manualControlSlot(motorControlCmd mcd)
{

    QSerialPort->write((char *)&mcd,sizeof(mcd));
    QSerialPort->putChar('\n');
}

void QSerialportHelper::autoControlSlot(motorControlCmd mcd)
{
        QSerialPort->write((char *)&mcd,sizeof(mcd));
        //QSerialPort->write((char *)&mcd,sizeof(mcd));
        QSerialPort->putChar('\n');
}
void QSerialportHelper::runFinishSlot(motorControlCmd mcd)
{
    QSerialPort->write((char *)&mcd,sizeof(mcd));
    QSerialPort->putChar('\n');
}

/**
 * @brief QSerialportHelper::YTK_F_Start_Stop
 */
void QSerialportHelper::YTK_F_Start_Stop()
{
    for(int i=0; i<3; i++)
    {
        QSerialPort->putChar(YTK_F_START_STOP);
        usleep(15000);
    }
}

/**
 * @brief QSerialportHelper::YTK_F_Bend
 */
void QSerialportHelper::YTK_F_Bend()
{
    for(int i=0; i<3; i++)
    {
        QSerialPort->putChar(YTK_F_BEND);
        usleep(15000);
    }
}

/**
 * @brief QSerialportHelper::YTK_F_Stretch
 */
void QSerialportHelper::YTK_F_Stretch()
{

    for(int i=0; i<3; i++)
    {
        QSerialPort->putChar(YTK_F_STRETCH);
        usleep(15000);
    }
}
