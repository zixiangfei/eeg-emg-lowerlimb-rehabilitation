#include "control.h"
#include "ui_control.h"
#include <QFile>
#include <QDebug>

bool startOrstop = false;
int controlModel = AUTO_MODEL;
int timeValue = 0;
extern bool thread_state;
Control::Control(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Control)
{
    ui->setupUi(this);
    if(controlModel == AUTO_MODEL)
    {
        if(thread_state == false)
        {
            if(startOrstop)
            {
                ui->start_auto_pushButton->setDisabled(true);
                ui->stop_auto_pushButton->setDisabled(true);
            }
            else
            {
                ui->start_auto_pushButton->setDisabled(false);
                ui->stop_auto_pushButton->setDisabled(true);
            }

        }
        else
        {
            ui->start_auto_pushButton->setDisabled(true);
            ui->stop_auto_pushButton->setDisabled(false);
        }
        ui->start_stop_manual_Button->setDisabled(true);
        ui->bend_manual_Button->setDisabled(true);
        ui->stretch_manual_Button->setDisabled(true);
        ui->auto_radioButton->setChecked(true);
    }
    else
    {
         if(thread_state == false)
         {
             ui->start_stop_manual_Button->setDisabled(false);
             ui->bend_manual_Button->setDisabled(false);
             ui->stretch_manual_Button->setDisabled(false);
         }
         else
         {
             if(startOrstop ==false)
             {
                 ui->start_stop_manual_Button->setDisabled(true);
                 ui->bend_manual_Button->setDisabled(true);
                 ui->stretch_manual_Button->setDisabled(true);
             }

         }
        ui->start_auto_pushButton->setDisabled(true);
        ui->stop_auto_pushButton->setDisabled(true);



        ui->manual_radioButton->setChecked(true);
    }


    settings = new QSettings("Settings.ini");
    if(!settings->value(SPEEDINFO_INI).isNull())
    {
       int b = settings->value(SPEEDINFO_INI).toInt();
        ui->speed_spinBox->setValue(b);
    }
    if(!settings->value(TIMEINFO_INI).isNull())
    {
       int a = settings->value(TIMEINFO_INI).toInt();
        ui->time_spinBox->setValue(a);
    }
    if(!settings->value(STRETCH_INI).isNull())
    {
        int c = settings->value(STRETCH_INI).toInt();
        ui->stretch_spinBox->setValue(c);
    }
    if(!settings->value(BEND_INI).isNull())
    {
        int d = settings->value(BEND_INI).toInt();
        ui->bend_spinBox->setValue(d);
    }
    angleThread = new Angle();//创建angle线程
    motorControlThread = new motorControl(); //电机线程
    connect(angleThread, SIGNAL(Angle_val(double)), this, SLOT(getAngleSlot(double))); //绑定获取角度的槽和信号
    connect(ui->start_stop_manual_Button,SIGNAL(clicked()),this,SLOT(manualStartStopSlot()));
    connect(ui->stretch_manual_Button,SIGNAL(clicked()),this,SLOT(manualStretchSlot()));
    connect(ui->bend_manual_Button,SIGNAL(clicked()),this,SLOT(manualBendSlot()));
    connect(ui->speed_spinBox,SIGNAL(valueChanged(int)),this,SLOT(speendValueSlot(int)));
    connect(ui->time_spinBox,SIGNAL(valueChanged(int)),this,SLOT(timeValueSlot(int)));
    connect(ui->stretch_spinBox,SIGNAL(valueChanged(int)),this,SLOT(stretchValueSlot(int)));
    connect(ui->bend_spinBox,SIGNAL(valueChanged(int)),this,SLOT(bendValueSlot(int)));
    connect(this, SIGNAL(motorStateSignal(double, double, int, int)), motorControlThread, SLOT(motorStateControlSlot(double, double, int, int)));
    connect(this, SIGNAL(motorModelSignal(int)), motorControlThread, SLOT(motorModelControlSlot(int)));
    Log4cplusInit();
}
Control::~Control()
{
    delete ui;
}
void Control::on_auto_radioButton_clicked()
{
    if(ui->auto_radioButton->isChecked())
    {
        controlModel = AUTO_MODEL;
        if(thread_state == false)
        {
            if(startOrstop)
            {
                ui->start_auto_pushButton->setDisabled(true);
                ui->stop_auto_pushButton->setDisabled(true);
            }
            else
            {
                ui->start_auto_pushButton->setDisabled(false);
                ui->stop_auto_pushButton->setDisabled(true);
            }

        }
        else
        {
            ui->start_auto_pushButton->setDisabled(true);
            ui->stop_auto_pushButton->setDisabled(false);
        }

        ui->start_stop_manual_Button->setDisabled(true);
        ui->bend_manual_Button->setDisabled(true);
        ui->stretch_manual_Button->setDisabled(true);
        emit ControlMsgSignal(WARING_MSG, tr("auto model..."));
        LOG4CPLUS_INFO(_logger,"auto model...");
    }
}

void Control::on_manual_radioButton_clicked()
{
    if(ui->manual_radioButton->isChecked())
    {
        controlModel = MANUAL_MODEL;
        if(thread_state == true)
        {
            ui->start_stop_manual_Button->setDisabled(true);
            ui->bend_manual_Button->setDisabled(true);
            ui->stretch_manual_Button->setDisabled(true);
        }
        else
        {
            ui->start_stop_manual_Button->setDisabled(false);
            ui->bend_manual_Button->setDisabled(false);
            ui->stretch_manual_Button->setDisabled(false);
        }

        ui->start_auto_pushButton->setDisabled(true);
        ui->stop_auto_pushButton->setDisabled(true);
        emit ControlMsgSignal(WARING_MSG, tr("manual model..."));
        LOG4CPLUS_INFO(_logger,"manual model...");
    }
}
void Control::on_start_auto_pushButton_clicked()
{
    timeValue = ui->time_spinBox->value();
    emit startEEGSiganl();
    emit startTimerSignal(1000*60*timeValue);
    ui->time_spinBox->setDisabled(true);
    emit ControlMsgSignal(INFO_MSG, tr("Start Read EEG Data..."));
    LOG4CPLUS_INFO(_logger,"Start Read EEG Data...");
}

void Control::on_stop_auto_pushButton_clicked()
{
    emit stopEEGsignal();
     ui->time_spinBox->setDisabled(false);
    emit ControlMsgSignal(INFO_MSG, tr("Stop Read EEG Data..."));
    LOG4CPLUS_INFO(_logger,"Stop Read EEG Data...");
}
void Control::manualStartStopSlot() //send start or stop serial cmd
{
     char temp[3];
     qDebug() << startOrstop <<'\n';
    if(startOrstop)
    {
          //Control::angleThread->exit();
          mcd.head = 0XAA;
          mcd.cmd = 0XFF;
          mcd.control= 0X00;
          mcd.check = 0X55;
          startOrstop = false;
          emit manualControlSignal(mcd);

          ui->time_spinBox->setDisabled(false);
          emit ControlMsgSignal(INFO_MSG,tr("stop manual run device "));
          Control::motorControlThread->terminateMotor();
          emit(motorStateSignal(ui->stretch_spinBox->value(), ui->bend_spinBox->value(), 0, 0));
          //Control::motorControlThread->requestInterruption();
          Control::motorControlThread->exit();
          //qDebug()<< QThread::currentThreadId() << " " << "test\n";
    }
    else
     {
        //Control::angleThread->run();

        startOrstop = true;
        timeValue = ui->time_spinBox->value();
        qDebug()<<"timeValue"<<timeValue;
        mcd.head = 0XAA;
        mcd.cmd = 0XFF;
        mcd.control = 0X01;
        mcd.check = 0X00;
        temp[0] = 0XAA;
        temp[1] = 0XFF;
        temp[2] = 0X01;
        for (int i=0;i<=2;i++)
        mcd.check ^=temp[i];
        emit manualControlSignal(mcd);
        if(timeValue == 0)
        {
            emit startTimerSignal(1000*60*1);
        }
        else
        {
            emit startTimerSignal(1000*60*timeValue);
        }
        //emit startTimerSignal(1000*60*timeValue);
        //这句话正确性存疑 先注释掉

        emit ControlMsgSignal(INFO_MSG,tr("start manual run device "));
        ui->time_spinBox->setDisabled(true);
        //修改电机状态
        emit(motorStateSignal(ui->stretch_spinBox->value(), ui->bend_spinBox->value(), ui->speed_spinBox->value(), ui->time_spinBox->value()));
        //选择电机模式
        emit(motorModelSignal(REPEAT_MODE));
        Control::motorControlThread->start();
        Control::motorControlThread->exit();
    }
}
void Control::manualStretchSlot() //send manual stretch serial cmd
{
    Control::motorControlThread->exit();
    char temp[3];
    if (startOrstop)
    {
        mcd.head = 0XAA;
        mcd.cmd = 0X02;
        mcd.control = 0X01;
        mcd.check = 0X00;

        temp[0] = 0XAA;
        temp[1] = 0X02;
        temp[2] = 0X01;
        for (int i=0;i<=2;i++)
            mcd.check ^=temp[i];

        emit manualControlSignal(mcd);
    }
    else{
        emit ControlMsgSignal(INFO_MSG,tr("Device not start"));
    }
    emit(motorModelSignal(STRETCH_MODE));
    Control::motorControlThread->start();
    Control::motorControlThread->exit();
}
void Control::manualBendSlot() //send manual bend serial cmd
{
    Control::motorControlThread->exit();
    char temp[3];
    if (startOrstop)
    {
        mcd.head = 0XAA;
        mcd.cmd = 0X02;
        mcd.control = 0X00;
        mcd.check = 0X00;

        temp[0] = 0XAA;
        temp[1] = 0X02;
        temp[2] = 0X00;
        for (int i=0;i<=2;i++)
            mcd.check ^=temp[i];

        emit manualControlSignal(mcd);
    }
    else{
        emit ControlMsgSignal(INFO_MSG,tr("Device not start"));
    }
    emit(motorModelSignal(BEND_MODE));
    Control::motorControlThread->start();
    Control::motorControlThread->exit();
}

void Control::Log4cplusInit()
{

    SharedObjectPtr<Appender> _append(new FileAppender("log",std::ios_base::app));
    _append->setName("Control_log");

    /* step 2: Instantiate a layout object */
    std::string pattern = "[%D{%Y-%m-%d %H:%M:%S}] [%t] %c %p - %m [%l]%n";
    std::unique_ptr<Layout> _layout(new PatternLayout(pattern));

    /* step 3: Attach the layout object to the appender */
    _append->setLayout(std::move(_layout));

    /* step 4:  Instantiate a logger object */
    _logger = Logger::getInstance("Logger Control");
    /* step 5: Attach the appender object to the  logger  */
    _logger.addAppender(_append);

    /* step 6: Set a priority for the logger  */
    _logger.setLogLevel(ALL_LOG_LEVEL);
}

void Control::speendValueSlot(int arg)
{

    emit(motorStateSignal(ui->stretch_spinBox->value(), ui->bend_spinBox->value(), ui->speed_spinBox->value(), ui->time_spinBox->value()));
    char temp[3];
    mcd.head = 0XAA;
    mcd.cmd = 0X01;
    mcd.control = (char)arg;
    mcd.check = 0X00;

    temp[0] = 0XAA;
    temp[1] = (char)arg;
    temp[2] = 0X01;
    for (int i=0;i<=2;i++)
        mcd.check ^=temp[i];
    settings->setValue(SPEEDINFO_INI,arg);
    emit manualControlSignal(mcd);
}

void Control::stretchValueSlot(int arg)
{
    emit(motorStateSignal(ui->stretch_spinBox->value(), ui->bend_spinBox->value(), ui->speed_spinBox->value(), ui->time_spinBox->value()));
    char temp[3];
    mcd.head = 0XAA;
    mcd.cmd = 0X03;
    mcd.control = (char)arg;
    mcd.check = 0X00;

    temp[0] = 0XAA;
    temp[1] = (char)arg;
    temp[2] = 0X03;
    for (int i=0;i<=2;i++)
        mcd.check ^=temp[i];
    settings->setValue(STRETCH_INI,arg);
    emit manualControlSignal(mcd);
}

void Control::bendValueSlot(int arg)
{

    emit(motorStateSignal(ui->stretch_spinBox->value(), ui->bend_spinBox->value(), ui->speed_spinBox->value(), ui->time_spinBox->value()));
    char temp[3];
    mcd.head = 0XAA;
    mcd.cmd = 0X04;
    mcd.control = (char)arg;
    mcd.check = 0X00;

    temp[0] = 0XAA;
    temp[1] = (char)arg;
    temp[2] = 0X04;
    for (int i=0;i<=2;i++)
        mcd.check ^=temp[i];
    settings->setValue(BEND_INI,arg);
    qDebug()<<"bendValue="<<arg;
    emit manualControlSignal(mcd);
}

void Control::timeValueSlot(int arg)
{
    emit(motorStateSignal(ui->stretch_spinBox->value(), ui->bend_spinBox->value(), ui->speed_spinBox->value(), ui->time_spinBox->value()));
    settings->setValue(TIMEINFO_INI,arg);

}

void Control::autoButtonStateSlot(bool startButtonState)
{
    if(startButtonState)
    {
        ui->start_auto_pushButton->setDisabled(startButtonState);
        ui->stop_auto_pushButton->setDisabled(false);
    }else{
        ui->start_auto_pushButton->setDisabled(startButtonState);
        ui->stop_auto_pushButton->setDisabled(true);
    }
}
void Control::timeSpinBoxSlot()
{
    ui->time_spinBox->setDisabled(false);
}

void Control::getAngleSlot(double angle) {
    Control::angle = angle;
}
