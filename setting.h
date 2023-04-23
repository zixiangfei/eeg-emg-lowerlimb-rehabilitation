#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QSettings>
#include "includes.h"

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = 0);
    ~Setting();

private slots:
    void on_systeminfo_radioButton_clicked();

    void on_exit_pushButton_clicked();

signals:
    void systemInfoSignal(bool s);
    void systeminfoMsgSignal(MSG_TYPE type, QString msg);

private:
    Ui::Setting *ui;
    QSettings *settings;

};

#endif // SETTING_H
