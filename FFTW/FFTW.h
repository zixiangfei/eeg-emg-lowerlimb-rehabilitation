#ifndef FFTW_H
#define FFTW_H

#define FFTW_DATA_SIZE  128


#include "/usr/local/fftw3/include/fftw3.h"


/*
implement the algorithm
*/
#define PROCSSCHANNAL 14
#define PROCESSMAXLENGTH 128

/**
 *
 */
typedef struct EEGData
{
    int channelcase;          //c3/c4 or cp3/cp4
    int channum;              //16/8
    float Data[PROCSSCHANNAL][PROCESSMAXLENGTH];
    int datalength;
}ProcessingData;

/**
 * @brief The ProcessEEGData class
 */
class ProcessEEGData
{
public:
    ProcessEEGData(void);
    ~ProcessEEGData(void);
public:
    ProcessingData m_ProData;
    //void UseFilter();
    int UseClassifer();
public:
    float Calculate_Amplitude(int len, float real, float imag);
    void Smooth(int length, float chdata[]);
    float AverageMax(float ch_pre[], float ch_next[]);
    int Classify(float ch_pre_M[],float ch_next_M[]);
    float destion1[PROCESSMAXLENGTH];
    float destion2[PROCESSMAXLENGTH];
    //float Average[PROCESSMAXLENGTH];
    int m_DataBlockSize;
};


#endif // FFTW_H
