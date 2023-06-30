#ifndef PORTCHANNELATTR_H
#define PORTCHANNELATTR_H
#include <QWidget>

class portChannelAttr
{
    //Q_OBJECT
public:
    portChannelAttr();

private:
    int channelState; // 1 running 0 shutdown
    int channelWaveForm; // 0 unselected 1 constant 2 sine 3 square 4 trangular
    int channelWaveFreq; // 1 - 1000 hz
    int channelWaveWidth; // 50 - 9950 us
    int channelWaveTime; // -1 inf e  |  ms
    int channelWaveCurrent; // 5 - 9995 uA
};

#endif // PORTCHANNELATTR_H
