#ifndef EMG_H
#define EMG_H

#include <QWidget>
#include <QFile>
#include <QDebug>
#include "includes.h"
#include <QKeyEvent>

namespace Ui {
class EMG;
}

class EMG : public QWidget
{
    Q_OBJECT

private:
    void loadStyleSheet(QString name);
    void smalleKeyBoardInit();

public:
    explicit EMG(QWidget *parent = 0);
    ~EMG();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::EMG *ui;
    int now_selected_channel;

private slots:
    void on_port_channel_tabWidget_tabBarClicked(int index);
    void set_keyboard_confirm_enable();
    void set_keyboard_confirm_disable();
    void slot_set_channel_wave_form();
};

#endif // EMG_H
