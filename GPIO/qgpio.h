#ifndef QGPIO_H
#define QGPIO_H

#include <QObject>
//#include "wiringPi.h"
//#include "softPwm.h"

/*
 * gpio好像没有涉及到功能都没有实现
*/

class QGPIO : public QObject
{
    Q_OBJECT
public:
    explicit QGPIO(QObject *parent = 0);

signals:

public slots:

};

#endif // QGPIO_H
