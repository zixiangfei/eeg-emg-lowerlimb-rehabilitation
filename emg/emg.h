#ifndef EMG_H
#define EMG_H

#include <QWidget>
#include <QFile>
#include <QDebug>
#include "includes.h"
#include <QKeyEvent>
#include <QGraphicsItem>
#include <math.h>
#include <QSettings>
#include "portchannelattr.h"

#define PI 3.1415926

namespace Ui {
class EMG;
}

class EMG : public QWidget
{
    Q_OBJECT

private:
    void loadStyleSheet(QString name);
    void smalleKeyBoardInit();
    void drawConstantWave(int channelNum);
    void drawSineWave(int channelNum);
    void drawTrangularWave(int channelNum);
    void drawSquareWave(int channelNum);
    void loadChannelSettings();
    void updateChannel1UI();
    void updateChannel2UI();
    void updateChannel3UI();
    void updateChannel4UI();



public:
    explicit EMG(QWidget *parent = 0);
    ~EMG();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::EMG *ui;
    int now_selected_channel;
    QVector<portChannelAttr> portChannels;
    QString channelUrl[5];
    QSettings *settings;
    QPixmap runningPix;
    QPixmap shutdownPix;


private slots:
    void on_port_channel_tabWidget_tabBarClicked(int index);
    void set_keyboard_confirm_enable();
    void set_keyboard_confirm_disable();
    void slot_set_channel_wave_form();
    void on_port_channel1_shoot_pushButton_clicked();
    void on_port_channel2_shoot_pushButton_clicked();
    void on_port_channel3_shoot_pushButton_clicked();
    void on_port_channel4_shoot_pushButton_clicked();
};

#endif // EMG_H
