#ifndef EMG_H
#define EMG_H

#include <QWidget>
#include <QFile>
#include "includes.h"

namespace Ui {
class EMG;
}

class EMG : public QWidget
{
    Q_OBJECT

private:
    void loadStyleSheet(QString name);

public:
    explicit EMG(QWidget *parent = 0);
    ~EMG();

private:
    Ui::EMG *ui;
};

#endif // EMG_H
