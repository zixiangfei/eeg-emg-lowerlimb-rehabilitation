#include "FFTW.h"
#include <string.h>
#include <math.h>


/**
 * @brief ProcessEEGData::ProcessEEGData
 */
ProcessEEGData::ProcessEEGData(void)
{
    m_ProData.channelcase = 1;
    memset(m_ProData.Data,0,PROCESSMAXLENGTH*PROCSSCHANNAL*sizeof(float));
    m_ProData.channum = 14; //default
    m_ProData.datalength = FFTW_DATA_SIZE;
}

/**
 * @brief ProcessEEGData::~ProcessEEGData
 */
ProcessEEGData::~ProcessEEGData(void)
{
}

/**
 * @brief ProcessEEGData::Calculate_Amplitude
 * @param len
 * @param real
 * @param imag
 * @return
 */
float ProcessEEGData::Calculate_Amplitude( int len, float real, float imag )
{
    float result = 0.0;
    result = (real / len) * (real / len) + (imag / len )* (imag / len);
    result = sqrt(result);
    return result;
}

/**
 * @brief ProcessEEGData::Smooth
 * @param length
 * @param chdata
 */
void ProcessEEGData::Smooth( int length, float chdata[] )
{
    int i;
    for(i = 0; i < length - 2; i++)
        chdata[i] = (chdata[i] + chdata[i + 1] + chdata[i + 2]) / 3;
}

/**
 * @brief ProcessEEGData::UseClassifer
 * @return
 */
int ProcessEEGData::UseClassifer()
{
    return Classify(destion1,destion2);
}

/**
 * @brief ProcessEEGData::AverageMax
 * @param ch_pre
 * @param ch_next
 * @return
 */
float ProcessEEGData::AverageMax( float ch_pre[], float ch_next[] )
{
    int i;
    float sum_pre = 0.0;
    float sum_next = 0.0;
    int Min = 3;
    int Max = 12;
    for(i = Min; i <= Max; i++)
    {
        sum_pre += 2 * ch_pre[i];
        sum_next += 2 * ch_next[i];
    }
    return ((sum_next - sum_pre) / (Max - Min + 1));
}

/**
 * @brief ProcessEEGData::Classify
 * @param ch_pre_M
 * @param ch_next_M
 * @return
 */
int ProcessEEGData::Classify( float ch_pre_M[],float ch_next_M[] )
{
    float classify_result=0.0;
    classify_result=AverageMax(ch_pre_M, ch_next_M);
    if (classify_result<0)
        return 0;
    else return 1;
}

