#include "qemokit.h"
#include "FFTW/FFTW.h"
#include <QDebug>
bool thread_state = false;
QEmokit::QEmokit(QObject *parent) :
    QThread(parent)
{
    emokit_dev = NULL;
    emokit_count = 0;
    //thread_state = false;
    lowBatteryFlag = false;

    Log4cplusInit();

    qtimer = new QTimer();
    qtimer->setInterval(500);
    qtimer->start();
    connect(qtimer,SIGNAL(timeout()),this,SLOT(timerOutSlot()));

    qtimer_Graph = new QTimer();
    qtimer_Graph->setInterval(20);
    qtimer_Graph->start();
    connect(qtimer_Graph,SIGNAL(timeout()),this,SLOT(timerOutGraphSlot()));
}

QEmokit::~QEmokit()
{
    qtimer->stop();
    delete qtimer;
    qtimer_Graph->stop();
    delete qtimer_Graph;
}

int QEmokit::emokit_init()
{
    emokit_dev = emokit_create();
    emokit_count = emokit_get_count(emokit_dev,EMOKIT_VID,EMOKIT_PID);
    LOG4CPLUS_INFO(_logger,"emokit_count["<<emokit_count<<"]...");

    int r = emokit_open(emokit_dev,EMOKIT_VID,EMOKIT_PID,1);
    if(r != 0)
    {
        emokit_close(emokit_dev);
        emokit_delete(emokit_dev);
        emokit_dev = emokit_create();
        r = emokit_open(emokit_dev,EMOKIT_VID,EMOKIT_PID,0);
        if(r != 0)
        {
            LOG4CPLUS_INFO(_logger,"CANNOT CONNECT["<<r<<"]...");
            return E_EMOKIT_INT_ERROR;
        }
    }

    return E_EMOKIT;
}

void QEmokit::run()
{
    int FF_Dat_num = 0;
    ProcessEEGData processEEGData;
    EEGData m_ProData;
    int index = 0;
    int i = 0;
    float Average = 0;
    int istretch=0;
    int ibend =0;
    char temp[3];

    int m_fftLen = FFTW_DATA_SIZE;
    fftw_complex *ch_pre_in, *ch_pre_out;
    fftw_complex *ch_next_in, *ch_next_out;
    fftw_plan p;
    ch_pre_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_fftLen);
    ch_pre_out= (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_fftLen);
    ch_next_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_fftLen);
    ch_next_out= (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_fftLen);

    float ChannelAvgDat[2][64] = {0.0};
    int flag = 0;
    if(emokit_init() != E_EMOKIT)
    {
        emit emokit_initMsgSignal(ERROR_MSG,tr("Emotiv Init Failed!"));
        return;
    }
    thread_state = true;
    mcd.head = 0XAA;
    mcd.cmd  = 0XFF;
    mcd.control = 0X01;
    mcd.check = 0X00;
    temp[0] = 0XAA;
    temp[1] = 0XFF;
    temp[2] = 0X01;
    for (int i=0;i<=2;i++)
    mcd.check ^=temp[i];
    emit autoControlSignal(mcd);
    emokit_dev->current_frame.battery = 100;
    lowBatteryFlag = false;

    LOG4CPLUS_INFO(_logger,"emokit thread running...");
    while (thread_state)
    {

        if( emokit_read_data(emokit_dev) == 32)
        {
            emokit_dev->current_frame = emokit_get_next_frame(emokit_dev);
            es.AF3 = emokit_dev->current_frame.AF3;
            es.AF4 = emokit_dev->current_frame.AF4;
            es.F3 = emokit_dev->current_frame.F3;
            es.F4 = emokit_dev->current_frame.F4;
            es.F7 = emokit_dev->current_frame.F7;
            es.F8 = emokit_dev->current_frame.F8;
            es.FC5 = emokit_dev->current_frame.FC5;
            es.FC6 = emokit_dev->current_frame.FC6;
            es.O1 = emokit_dev->current_frame.O1;
            es.O2 = emokit_dev->current_frame.O2;
            es.P7 = emokit_dev->current_frame.P7;
            es.P8 = emokit_dev->current_frame.P8;
            es.T7 = emokit_dev->current_frame.T7;
            es.T8 = emokit_dev->current_frame.T8;
            /****FFTW****/
            m_ProData.Data[0][FF_Dat_num] = emokit_dev->current_frame.F3;
            m_ProData.Data[1][FF_Dat_num] = emokit_dev->current_frame.FC6;
            m_ProData.Data[2][FF_Dat_num] = emokit_dev->current_frame.P7;
            m_ProData.Data[3][FF_Dat_num] = emokit_dev->current_frame.T8;
            m_ProData.Data[4][FF_Dat_num] = emokit_dev->current_frame.F7;
            m_ProData.Data[5][FF_Dat_num] = emokit_dev->current_frame.F8;
            m_ProData.Data[6][FF_Dat_num] = emokit_dev->current_frame.T7;
            m_ProData.Data[7][FF_Dat_num] = emokit_dev->current_frame.P8;
            m_ProData.Data[8][FF_Dat_num] = emokit_dev->current_frame.AF4;
            m_ProData.Data[9][FF_Dat_num] = emokit_dev->current_frame.F4;
            m_ProData.Data[10][FF_Dat_num] = emokit_dev->current_frame.AF3;
            m_ProData.Data[11][FF_Dat_num] = emokit_dev->current_frame.O2;
            m_ProData.Data[12][FF_Dat_num] = emokit_dev->current_frame.O1;
            m_ProData.Data[13][FF_Dat_num] = emokit_dev->current_frame.FC5;

            Average = (m_ProData.Data[0][FF_Dat_num]+m_ProData.Data[1][FF_Dat_num]+m_ProData.Data[2][FF_Dat_num]
                        +m_ProData.Data[3][FF_Dat_num]+m_ProData.Data[4][FF_Dat_num]+m_ProData.Data[5][FF_Dat_num]+m_ProData.Data[6][FF_Dat_num]
                        +m_ProData.Data[7][FF_Dat_num]+m_ProData.Data[8][FF_Dat_num]+m_ProData.Data[9][FF_Dat_num]+m_ProData.Data[10][FF_Dat_num]
                        +m_ProData.Data[11][FF_Dat_num]+m_ProData.Data[12][FF_Dat_num]+m_ProData.Data[13][FF_Dat_num])/14;

            ch_pre_in[FF_Dat_num][0] = m_ProData.Data[1][FF_Dat_num]-Average;
            ch_next_in[FF_Dat_num][0] = m_ProData.Data[13][FF_Dat_num]-Average;
            ch_pre_in[FF_Dat_num][1] = 0.0;
            ch_next_in[FF_Dat_num][1] = 0.0;


            if(FF_Dat_num > 63 && flag == 0)
            {
                ChannelAvgDat[0][FF_Dat_num-64] = m_ProData.Data[0][FF_Dat_num]-Average;
                ChannelAvgDat[1][FF_Dat_num-64] = m_ProData.Data[9][FF_Dat_num]-Average;
            }
            FF_Dat_num ++;
        }
        if(FF_Dat_num >= FFTW_DATA_SIZE)
        {

            if(flag == 1)
            {
                for(i=0;i<64;i++)
                {
                    ch_pre_in[i][0] = ChannelAvgDat[0][i];
                    ch_next_in[i][0] = ChannelAvgDat[1][i];
                }
                for(i=0;i<64;i++)
                {
                     ChannelAvgDat[0][i] = ch_pre_in[i+64][0];
                     ChannelAvgDat[1][i] = ch_next_in[i+64][0];
                }

            }
            flag = 1;

            FF_Dat_num = 64;
            p = fftw_plan_dft_1d(m_fftLen ,ch_pre_in,ch_pre_out, FFTW_FORWARD, FFTW_ESTIMATE);
            fftw_execute(p); /* repeat as needed*/
            p = fftw_plan_dft_1d(m_fftLen ,ch_next_in,ch_next_out, FFTW_FORWARD, FFTW_ESTIMATE);
            fftw_execute(p);

            //calculate the amplitude;
            for(i = 0; i < m_fftLen; i++)
            {
                processEEGData.destion1[i] = processEEGData.Calculate_Amplitude(m_fftLen,ch_pre_out[i][0],ch_pre_out[i][1]);
                processEEGData.destion2[i] = processEEGData.Calculate_Amplitude(m_fftLen,ch_next_out[i][0],ch_next_out[i][1]);
            }

            processEEGData.Smooth(m_fftLen,processEEGData.destion1);
            processEEGData.Smooth(m_fftLen,processEEGData.destion2);

            index = processEEGData.UseClassifer();
            if(index){
                ibend++;
            }else
            {
                istretch++;
            }

            if(ibend > DECISION_NUM)
            {
                ibend=0;
                istretch=0;

                mcd.head = 0XAA;
                mcd.cmd = 0X02;
                mcd.control = 0X00;
                mcd.check = 0X00;

                temp[0] = 0XAA;
                temp[1] = 0X02;
                temp[2] = 0X00;
                for (int i=0;i<=2;i++)
                   mcd.check ^=temp[i];
                emit autoControlSignal(mcd);
                qDebug()<<"bend";

            }else if(istretch > DECISION_NUM)
            {
                ibend=0;
                istretch=0;

                mcd.head = 0XAA;
                mcd.cmd = 0X02;
                mcd.control = 0X01;
                mcd.check = 0X00;

                temp[0] = 0XAA;
                temp[1] = 0X02;
                temp[2] = 0X01;
                for (int i=0;i<=2;i++)
                   mcd.check ^=temp[i];
                emit autoControlSignal(mcd);
                qDebug()<<"stretch";

            }

        }



 //       PrinHex(emokit_dev->raw_unenc_frame,32);
 //       sleep(2);
    }
    LOG4CPLUS_INFO(_logger,"emokit thread stop...");

}
void QEmokit::thread_stop()
{
    if(thread_state == true)
    {
         mcd.head = 0XAA;
         mcd.cmd  = 0XFF;
         mcd.control = 0X00;
         mcd.check = 0X55;
         emit autoControlSignal(mcd);
        thread_state = false;
     }
}
void QEmokit::timerOutSlot()
{
    if(thread_state == true)
    {
        emit batterySignal(emokit_dev->current_frame.battery);
        emit signalQualitySignal(emokit_dev->current_frame.cq);
         //qDebug()<<"AF3="<<emokit_dev->current_frame.cq.AF3;
        qDebug()<<"F4="<<emokit_dev->current_frame.cq.F4;
         qDebug()<<"F3  = "<<emokit_dev->current_frame.cq.F3;
        emit signalValueSignal(es);

        if(emokit_dev->current_frame.battery <= 15 && emokit_dev->current_frame.battery > 0 && lowBatteryFlag == false)
        {
            emit lowBatterySignal(emokit_dev->current_frame.battery);
            lowBatteryFlag = true;
        }
        else
            if(emokit_dev->current_frame.battery > 15)
            {
                lowBatteryFlag = false;
            }
    }
}
void QEmokit::timerOutGraphSlot()
{
    if(thread_state == true)
    {
        emit signalValueSignal(es);
    }
}
void QEmokit::Log4cplusInit()
{

    SharedObjectPtr<Appender> _append(new FileAppender("log",std::ios_base::app));
    _append->setName("qemokit_log");

    /* step 2: Instantiate a layout object */
    std::string pattern = "[%D{%Y-%m-%d %H:%M:%S}] [%t] %c %p - %m [%l]%n";
    std::unique_ptr<Layout> _layout(new PatternLayout(pattern));

    /* step 3: Attach the layout object to the appender */
    _append->setLayout(std::move(_layout));

    /* step 4:  Instantiate a logger object */
    _logger = Logger::getInstance("Logger QEmokit");
    /* step 5: Attach the appender object to the  logger  */
    _logger.addAppender(_append);

    /* step 6: Set a priority for the logger  */
    _logger.setLogLevel(ALL_LOG_LEVEL);
}
