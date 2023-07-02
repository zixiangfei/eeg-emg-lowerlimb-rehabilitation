#include "portchannelattr.h"

portChannelAttr::portChannelAttr()
{

}

int portChannelAttr::getChannelState() const
{
    return channelState;
}

void portChannelAttr::setChannelState(int value)
{
    channelState = value;
}

int portChannelAttr::getChannelWaveForm() const
{
    return channelWaveForm;
}

void portChannelAttr::setChannelWaveForm(int value)
{
    channelWaveForm = value;
}

int portChannelAttr::getChannelWaveFreq() const
{
    return channelWaveFreq;
}

void portChannelAttr::setChannelWaveFreq(int value)
{
    channelWaveFreq = value;
}

int portChannelAttr::getChannelWaveWidth() const
{
    return channelWaveWidth;
}

void portChannelAttr::setChannelWaveWidth(int value)
{
    channelWaveWidth = value;
}

int portChannelAttr::getChannelWaveTime() const
{
    return channelWaveTime;
}

void portChannelAttr::setChannelWaveTime(int value)
{
    channelWaveTime = value;
}

int portChannelAttr::getChannelWaveCurrent() const
{
    return channelWaveCurrent;
}

void portChannelAttr::setChannelWaveCurrent(int value)
{
    channelWaveCurrent = value;
}
