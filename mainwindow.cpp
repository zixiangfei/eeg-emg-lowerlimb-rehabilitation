#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "IcoHelper/iconhelper.h"
#include "SerialPort/qserialporthelper.h"
#include <QFile>
extern bool thread_state;
extern bool startOrstop;
extern int  timeValue;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadStyleSheet("Style");
    //loadStyleSheet("emgStyle");
    Log4cplusInit();

    currentWidget = 0;
    mcd.head = 0XAA;
    mcd.cmd = 0XFF;
    mcd.control= 0X00;
    mcd.check = 0X55;



    IconHelper::Instance()->SetIcon(ui->Back_pushButton, QChar(ICO_HOME), 20);
    ui->Back_pushButton->setDisabled(true);

    systemInfo = new SystemInfo;
    connect(systemInfo,SIGNAL(CPUUsageSignal(double)),this,SLOT(CPUUsageSlot(double)));
    connect(systemInfo,SIGNAL(MEMUsageSignal(double)),this,SLOT(MEMUsageSlot(double)));

    settings = new QSettings("Settings.ini");
    if(!settings->value(SYSTEMINFO_INI).isNull())
    {
        bool b = settings->value(SYSTEMINFO_INI).toBool();
        systemInfoShowSlot(b);
    }
    delete settings;

    stateBar = new QStatusBar(ui->state_widget);
    stateBar->setSizeGripEnabled(false);
    ui->statusbar_horizontalLayout->addWidget(stateBar);
    QFont font;
    font.setPointSize(14);
    stateBar->setFont(font);
    stateShowMessageSlot(INFO_MSG,tr("System Start..."));
    LOG4CPLUS_INFO(_logger,"System Start...");

    ui->pushButton5->setDisabled(true);
    ui->pushButton6->setDisabled(true);

    qSerialportHelper = new QSerialportHelper;
    qSerialportHelper->QSerialPortOpen();

    qRegisterMetaType<MSG_TYPE>("MSG_TYPE");
    qRegisterMetaType<emokit_contact_quality>("emokit_contact_quality");
    qRegisterMetaType<emokit_signal>("emokit_signal");
    qRegisterMetaType<motorControlCmd>("motorControlCmd");

    qemokit = new QEmokit;
    connect(qemokit,SIGNAL(emokit_initMsgSignal(MSG_TYPE, QString)),this,SLOT(stateShowMessageSlot(MSG_TYPE, QString)));
    connect(qemokit,SIGNAL(lowBatterySignal(unsigned char)),this,SLOT(lowBatterySlot(unsigned char)));
    connect(qemokit,SIGNAL(autoControlSignal(motorControlCmd)),qSerialportHelper,SLOT(autoControlSlot(motorControlCmd)));
    connect(this,SIGNAL(runFinishSignal(motorControlCmd)),qSerialportHelper,SLOT(runFinishSlot(motorControlCmd)));
    runTimer = new QTimer;
    connect(runTimer,SIGNAL(timeout()),this,SLOT(timeOutRunSlot()));
    timeThread = new QThread;
    timeThread->start();
    runTimer->moveToThread(timeThread);

    ControlWidget = new Control;
    EEGWidget = new EEG;
    EMGWidget = new EMG;
    SettingWidget = new Setting;
}
MainWindow::~MainWindow()
{

    emit runFinishSignal(mcd);
    delete systemInfo;
    delete stateBar;
    delete EMGWidget;
    delete SettingWidget;
    delete EEGWidget;
    delete ControlWidget;
    delete ui;
    qSerialportHelper->QSerialPortClose();
    delete qSerialportHelper;
}

void MainWindow::CPUUsageSlot(double u)
{
    QString str="CPU:";
    str.append(QString::number(u,'g',3));
    str.append("%");

    ui->CPUUsage_label->setText(str);
}

void MainWindow::MEMUsageSlot(double u)
{
    QString str="Mem:";
    str.append(QString::number(u,'g',3));
    str.append("%");

    ui->MEMUsage_label->setText(str);
}

void MainWindow::stateShowMessageSlot(MSG_TYPE type, QString msg)
{
    QPalette pe;
    if(type == INFO_MSG)
    {
        pe.setColor(QPalette::WindowText,QColor(0x228b22));
    }

    if(type == WARING_MSG)
    {
        pe.setColor(QPalette::WindowText,QColor(0xee7600));
    }

    if(type == ERROR_MSG)
    {
        pe.setColor(QPalette::WindowText,Qt::red);
    }

    stateBar->setPalette(pe);
    stateBar->showMessage(msg,3000);
}

void MainWindow::loadStyleSheet(QString name)
{
    QFile file(QString("://QSS/%1.css").arg(name));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
    file.close();
}

void MainWindow::Log4cplusInit()
{

    SharedObjectPtr<Appender> _append(new FileAppender("log",std::ios_base::app));
    _append->setName("MainWindow_log");

    /* step 2: Instantiate a layout object */
    std::string pattern = "[%D{%Y-%m-%d %H:%M:%S}] [%t] %c %p - %m [%l]%n";
    std::unique_ptr<Layout> _layout(new PatternLayout(pattern));

    /* step 3: Attach the layout object to the appender */
    _append->setLayout(std::move(_layout));

    /* step 4:  Instantiate a logger object */
    _logger = Logger::getInstance("Logger MainWindow");
    /* step 5: Attach the appender object to the  logger  */
    _logger.addAppender(_append);

    /* step 6: Set a priority for the logger  */
    _logger.setLogLevel(ALL_LOG_LEVEL);
}

void MainWindow::on_pushButton1_clicked()
{
//    if(ControlWidget == nullptr) {
//        ControlWidget = new Control;
//    }
    ui->main_widget->hide();
    ui->MainVerticalLayout->removeWidget(ui->main_widget);
    ui->MainVerticalLayout->addWidget(ControlWidget);
    ControlWidget->show();
    currentWidget = 1;

    connect(ControlWidget,SIGNAL(ControlMsgSignal(MSG_TYPE, QString)),this,SLOT(stateShowMessageSlot(MSG_TYPE, QString)));
    connect(ControlWidget,SIGNAL(startEEGSiganl()),this,SLOT(eegStartSlot()));
    connect(ControlWidget,SIGNAL(stopEEGsignal()),this,SLOT(eegStopSlot()));
    connect(ControlWidget,SIGNAL(manualControlSignal(motorControlCmd)),qSerialportHelper,SLOT(manualControlSlot(motorControlCmd)));
    connect(ControlWidget,SIGNAL(startTimerSignal(int)),runTimer,SLOT(start(int)));
    connect(this,SIGNAL(autoButtonStateSignal(bool)),ControlWidget,SLOT(autoButtonStateSlot(bool)));
    connect(this,SIGNAL(timeSpinBoxSinganl()),ControlWidget,SLOT(timeSpinBoxSlot()));
    ui->Back_pushButton->setDisabled(false);
    IconHelper::Instance()->SetIcon(ui->Back_pushButton, QChar(ICO_BACK), 20);
    LOG4CPLUS_INFO(_logger,"currentWidget["<<currentWidget<<"]...");
}

void MainWindow::on_pushButton2_clicked()
{
//    if(EEGWidget == nullptr) {
//        EEGWidget = new EEG;
//    }
    ui->main_widget->hide();
    ui->MainVerticalLayout->removeWidget(ui->main_widget);
    ui->MainVerticalLayout->addWidget(EEGWidget);
    EEGWidget->show();
    currentWidget = 2;

    connect(qemokit,SIGNAL(batterySignal(unsigned char)),EEGWidget,SLOT(batterySlot(unsigned char)));
    connect(qemokit,SIGNAL(lowBatterySignal(unsigned char)),EEGWidget,SLOT(batterySlot(unsigned char)));
    connect(qemokit,SIGNAL(signalQualitySignal(emokit_contact_quality)),EEGWidget,SLOT(signalQualitySlot(emokit_contact_quality)));
    connect(qemokit,SIGNAL(signalValueSignal(emokit_signal)),EEGWidget,SLOT(siganlValueSlot(emokit_signal)));

    ui->Back_pushButton->setDisabled(false);
    IconHelper::Instance()->SetIcon(ui->Back_pushButton, QChar(ICO_BACK), 20);
    LOG4CPLUS_INFO(_logger,"currentWidget["<<currentWidget<<"]...");
}

void MainWindow::on_pushButton3_clicked()
{
//    if(EMGWidget == nullptr) {
//        EMGWidget = new EMG;
//    }
    ui->main_widget->hide();
    ui->MainVerticalLayout->removeWidget(ui->main_widget);
    ui->MainVerticalLayout->addWidget(EMGWidget);
    EMGWidget->show();
    currentWidget = 3;

    connect(EMGWidget, SIGNAL(EmgMsgSignal(MSG_TYPE, QString)), this, SLOT(stateShowMessageSlot(MSG_TYPE, QString)));

    ui->Back_pushButton->setDisabled(false);
    IconHelper::Instance()->SetIcon(ui->Back_pushButton, QChar(ICO_BACK), 20);
    LOG4CPLUS_INFO(_logger,"currentWidget["<<currentWidget<<"]...");
}

void MainWindow::on_pushButton4_clicked()
{
//    if(SettingWidget == nullptr) {
//        SettingWidget = new Setting;
//    }
    ui->main_widget->hide();
    ui->MainVerticalLayout->removeWidget(ui->main_widget);
    ui->MainVerticalLayout->addWidget(SettingWidget);
    SettingWidget->show();
    currentWidget = 4;

    connect(SettingWidget,SIGNAL(systemInfoSignal(bool)),this,SLOT(systemInfoShowSlot(bool)));
    connect(SettingWidget,SIGNAL(systeminfoMsgSignal(MSG_TYPE, QString)),this,SLOT(stateShowMessageSlot(MSG_TYPE, QString)));

    ui->Back_pushButton->setDisabled(false);
    IconHelper::Instance()->SetIcon(ui->Back_pushButton, QChar(ICO_BACK), 20);
    LOG4CPLUS_INFO(_logger,"currentWidget["<<currentWidget<<"]...");
}

void MainWindow::on_Back_pushButton_clicked()
{
    switch(currentWidget)
    {
    case 1:
        ControlWidget->hide();
        ui->MainVerticalLayout->removeWidget(ControlWidget);
        ui->MainVerticalLayout->addWidget(ui->main_widget);
        ui->main_widget->show();
        //delete ControlWidget;
        break;
    case 2:
        EEGWidget->hide();
        ui->MainVerticalLayout->removeWidget(EEGWidget);
        ui->MainVerticalLayout->addWidget(ui->main_widget);
        ui->main_widget->show();
        //delete EEGWidget;
        break;
    case 3:
        EMGWidget->hide();
        ui->MainVerticalLayout->removeWidget(EMGWidget);
        ui->MainVerticalLayout->addWidget(ui->main_widget);
        ui->main_widget->show();
        //delete EMGWidget;
        break;
    case 4:
        SettingWidget->hide();
        ui->MainVerticalLayout->removeWidget(SettingWidget);
        ui->MainVerticalLayout->addWidget(ui->main_widget);
        ui->main_widget->show();
        //delete SettingWidget;
        break;
    default:break;
    }

    currentWidget = 0;

    ui->Back_pushButton->setDisabled(true);
    IconHelper::Instance()->SetIcon(ui->Back_pushButton, QChar(ICO_HOME), 20);
    LOG4CPLUS_INFO(_logger,"currentWidget["<<currentWidget<<"]...");
}

void MainWindow::systemInfoShowSlot(bool s)
{
    if(s == true)
    {
        ui->CPUUsage_label->show();
        ui->MEMUsage_label->show();
        LOG4CPLUS_INFO(_logger,"System Info Show...");
    }
    else
    {
        ui->CPUUsage_label->hide();
        ui->MEMUsage_label->hide();
        LOG4CPLUS_INFO(_logger,"System Info Close...");
    }

}

void MainWindow::eegStartSlot()
{

    if(thread_state == false)
    {
        qemokit->start();
    }
    emit autoButtonStateSignal(true);
}
void MainWindow::eegStopSlot()
{
    qemokit->thread_stop();
    emit autoButtonStateSignal(false);
}

void MainWindow::timeOutRunSlot()
{
    if(thread_state == true)
       thread_state = false;
   if(startOrstop == true)
       startOrstop = false;
   if(runTimer->isActive())
      runTimer->stop();

    emit runFinishSignal(mcd);//send stop cmd to dev;
    emit timeSpinBoxSinganl();

}

void MainWindow::lowBatterySlot(unsigned char bat)
{
    int a = (int)bat;
    stateShowMessageSlot(ERROR_MSG, tr("Battery less than 15%! Please Charge"));
    LOG4CPLUS_INFO(_logger,"lowBattery["<<a<<"%]...");
}
