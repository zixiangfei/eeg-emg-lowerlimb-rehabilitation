#ifndef PORTCHANNELATTR_H
#define PORTCHANNELATTR_H
#include <QWidget>

#define CONSTANT_WAVE 1
#define SINE_WAVE 2
#define SQUARE_WAVE 3
#define TRANGULAR_WAVE 4
#define CHANNEL_SHUTDOWN 0
#define CHANNEL_RUNNING 1
#define INF_TIME 0

class portChannelAttr
{
    //Q_OBJECT
public:
    portChannelAttr();

    int getChannelState() const;
    void setChannelState(int value);

    int getChannelWaveForm() const;
    void setChannelWaveForm(int value);

    int getChannelWaveFreq() const;
    void setChannelWaveFreq(int value);

    int getChannelWaveWidth() const;
    void setChannelWaveWidth(int value);

    int getChannelWaveTime() const;
    void setChannelWaveTime(int value);

    int getChannelWaveCurrent() const;
    void setChannelWaveCurrent(int value);

private:
    int channelState; // 1 running 0 shutdown
    int channelWaveForm; // 0 unselected 1 constant 2 sine 3 square 4 trangular
    int channelWaveFreq; // 1 - 1000 hz
    int channelWaveWidth; // 50 - 9950 us
    int channelWaveTime; // -1 inf e  |  ms
    int channelWaveCurrent; // 5 - 9995 uA
};

#endif // PORTCHANNELATTR_H
