#include "emg.h"
#include "ui_emg.h"

EMG::EMG(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EMG)
{
    ui->setupUi(this);
    //loadStyleSheet("emgStyle");
    EMG::settings = new QSettings("Settings.ini");
    EMG::channel1Qtimer = new QTimer(this);
    EMG::channel2Qtimer = new QTimer(this);
    EMG::channel3Qtimer = new QTimer(this);
    EMG::channel4Qtimer = new QTimer(this);
    EMG::channelUrl[1] = "/channel1";
    EMG::channelUrl[2] = "/channel2";
    EMG::channelUrl[3] = "/channel3";
    EMG::channelUrl[4] = "/channel4";
    EMG::runningPix.load(":/Image/channel_running.png");
    EMG::shutdownPix.load(":/Image/channel_shutdown.png");
    EMG::now_selected_channel = ui->port_channel_tabWidget->currentIndex() + 1;
    int label_width = ui->port_channel_state_label_1->width(), label_height = ui->port_channel_state_label_1->height();
    EMG::runningPix = EMG::runningPix.scaled(label_width, label_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);//按比例缩放
    EMG::shutdownPix = EMG::shutdownPix.scaled(label_width, label_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);//按比例缩放
    //加载数据
    EMG::loadChannelSettings();
    EMG::runningChannel = 0;
    EMG::portAllChannelModel = RUNNING_MODEL;
    //更新UI
    EMG::updateChannel1UI();
    EMG::updateChannel2UI();
    EMG::updateChannel3UI();
    EMG::updateChannel4UI();

    //connect(ui->port_channel_tabWidget, SIGNAL(tabBarClicked(int)), this, SLOT(on_port_channel_tabWidget_tabBarClicked(int))); //绑定切换通道事件
    //使用connectSlotByName就不用在代码中connect否则会触发两次
    //小键盘
    EMG::smalleKeyBoardInit();

    //绑定事件监听器
    EMG::ui->keyboard_confirm_pushButton->setEnabled(true);
    EMG::ui->port_channel1_time_textEdit->installEventFilter(this);
    EMG::ui->port_channel2_time_textEdit->installEventFilter(this);
    EMG::ui->port_channel3_time_textEdit->installEventFilter(this);
    EMG::ui->port_channel4_time_textEdit->installEventFilter(this);
    EMG::ui->port_channel1_freq_textEdit->installEventFilter(this);
    EMG::ui->port_channel2_freq_textEdit->installEventFilter(this);
    EMG::ui->port_channel3_freq_textEdit->installEventFilter(this);
    EMG::ui->port_channel4_freq_textEdit->installEventFilter(this);
    EMG::ui->port_channel1_current_textEdit->installEventFilter(this);
    EMG::ui->port_channel2_current_textEdit->installEventFilter(this);
    EMG::ui->port_channel3_current_textEdit->installEventFilter(this);
    EMG::ui->port_channel4_current_textEdit->installEventFilter(this);
    EMG::ui->port_channel1_width_textEdit->installEventFilter(this);
    EMG::ui->port_channel2_width_textEdit->installEventFilter(this);
    EMG::ui->port_channel3_width_textEdit->installEventFilter(this);
    EMG::ui->port_channel4_width_textEdit->installEventFilter(this);
    //单选
    for(auto radioBtn : EMG::ui->port_channel_wave_choose_groupBox->findChildren<QRadioButton*>()) {
        connect(radioBtn, SIGNAL(clicked(bool)), this, SLOT(slot_set_channel_wave_form()));
    }
    //连接定时器的信号与槽
    connect(EMG::channel1Qtimer, SIGNAL(timeout()), this, SLOT(slot_channel1QtimerTimeout()));
    connect(EMG::channel2Qtimer, SIGNAL(timeout()), this, SLOT(slot_channel2QtimerTimeout()));
    connect(EMG::channel3Qtimer, SIGNAL(timeout()), this, SLOT(slot_channel3QtimerTimeout()));
    connect(EMG::channel4Qtimer, SIGNAL(timeout()), this, SLOT(slot_channel4QtimerTimeout()));
    connect(EMG::ui->port_channel1_pause_pushButton, SIGNAL(clicked()), this, SLOT(slot_port_channel1_pause_pushButtonClicked()));
    connect(EMG::ui->port_channel2_pause_pushButton, SIGNAL(clicked()), this, SLOT(slot_port_channel2_pause_pushButtonClicked()));
    connect(EMG::ui->port_channel3_pause_pushButton, SIGNAL(clicked()), this, SLOT(slot_port_channel3_pause_pushButtonClicked()));
    connect(EMG::ui->port_channel4_pause_pushButton, SIGNAL(clicked()), this, SLOT(slot_port_channel4_pause_pushButtonClicked()));
    connect(EMG::ui->port_channel1_save_pushButton, SIGNAL(clicked()), this, SLOT(on_port_channe11_save_pushButton_clicked()));
    connect(EMG::ui->port_channel2_save_pushButton, SIGNAL(clicked()), this, SLOT(on_port_channe12_save_pushButton_clicked()));
    connect(EMG::ui->port_channel3_save_pushButton, SIGNAL(clicked()), this, SLOT(on_port_channe13_save_pushButton_clicked()));
    connect(EMG::ui->port_channel4_save_pushButton, SIGNAL(clicked()), this, SLOT(on_port_channe14_save_pushButton_clicked()));
    connect(EMG::ui->port_all_channel_open_close_pushButton, SIGNAL(clicked()), this, SLOT(slot_port_all_channel_open_close_pushButtonClicked()));

    //串口初始化

    EMG::initEMGSerialPort();

    EMGSerialPort testPort;
}

EMG::~EMG()
{
    delete ui;
}

/*
 * 读取css文件
 */
void EMG::loadStyleSheet(QString name)
{
    QFile file(QString("://QSS/%1.css").arg(name));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
}

/*
 * 切换通道时修改当前选中的通道是那一个
 */
void EMG::on_port_channel_tabWidget_tabBarClicked(int index) {
    EMG::now_selected_channel = index + 1;
    //qDebug() << "now selected tab: " << index << '\n';
    int channelNum = EMG::now_selected_channel;
    EMG::ui->constant_radioButton->setChecked(false);
    EMG::ui->sine_radioButton->setChecked(false);
    EMG::ui->square_radioButton->setChecked(false);
    EMG::ui->trangular_radioButton->setChecked(false);
    switch(EMG::portChannels[channelNum].getChannelWaveForm()) {
        case SINE_WAVE:
            EMG::ui->sine_radioButton->setChecked(true);
            break;
        case TRANGULAR_WAVE:
            EMG::ui->trangular_radioButton->setChecked(true);
            break;
        case SQUARE_WAVE:
            EMG::ui->square_radioButton->setChecked(true);
            break;
        case CONSTANT_WAVE:
            EMG::ui->constant_radioButton->setChecked(true);
            break;
        default:
            break;
    }
}


/*
 * 小键盘模块初始化
 */
void EMG::smalleKeyBoardInit() {
    //qDebug() << curFocusTextEdit->
    //lambda表达式 清空键
    connect(ui->keyboard_clear_or_inf_pushButton, &QPushButton::clicked, [=](){
        if(focusWidget() != nullptr) {
            QTextEdit *curFocusTextEdit = qobject_cast<QTextEdit *>(EMG::ui->port_channel_tabWidget->focusWidget());
            if(curFocusTextEdit) {
                curFocusTextEdit->clear();
            }
        }
    });

    //设定时间或次数为持续
    connect(ui->keyboard_confirm_pushButton, &QPushButton::clicked, [=](){
       if(focusWidget() != nullptr) {
           QTextEdit *curFocusTextEdit = qobject_cast<QTextEdit *>(EMG::ui->port_channel_tabWidget->focusWidget());
           if(curFocusTextEdit) {
               curFocusTextEdit->setText("持续");
           }
       }
    });

    //数字键
    for(auto btn:ui->small_keyboard_groupBox->findChildren<QPushButton *>()) {
        if(btn == ui->keyboard_confirm_pushButton || btn == ui->keyboard_clear_or_inf_pushButton) {
            continue;
        }
        connect(btn, &QPushButton::clicked, [=](){
            if(ui->port_channel_tabWidget->focusWidget() != nullptr) {
                QTextEdit *curFocusTextEdit = qobject_cast<QTextEdit *>(EMG::ui->port_channel_tabWidget->focusWidget());
                if(curFocusTextEdit) {
                    QString text = btn->text();
                    QChar c = text.at(0);
                    int num = c.unicode();
                    QKeyEvent evt(QEvent::KeyPress, num, Qt::KeyboardModifier::NoModifier, text); //模拟键盘输入
                    qApp->sendEvent(curFocusTextEdit, &evt);
                }
            }
        });
    }
}

/*
 * 设定持续按钮有效
 */
void EMG::set_keyboard_confirm_enable() {
    EMG::ui->keyboard_confirm_pushButton->setEnabled(true);
}

/*
 * 设定持续按钮无效
 */
void EMG::set_keyboard_confirm_disable() {
    EMG::ui->keyboard_confirm_pushButton->setEnabled(false);
}

/*
 * 事件过滤器
 */
bool EMG::eventFilter(QObject *watched, QEvent *event) {
    //只有时间、次数能选择持续输出
    if(watched == EMG::ui->port_channel1_time_textEdit ||
       watched == EMG::ui->port_channel2_time_textEdit ||
       watched == EMG::ui->port_channel3_time_textEdit ||
       watched == EMG::ui->port_channel4_time_textEdit ) {
        if(event->type() == QEvent::FocusIn) {
            EMG::ui->keyboard_confirm_pushButton->setEnabled(true);
        }
    }
    if(watched == EMG::ui->port_channel1_freq_textEdit ||
       watched == EMG::ui->port_channel2_freq_textEdit ||
       watched == EMG::ui->port_channel3_freq_textEdit ||
       watched == EMG::ui->port_channel4_freq_textEdit ||
       watched == EMG::ui->port_channel1_width_textEdit ||
       watched == EMG::ui->port_channel2_width_textEdit ||
       watched == EMG::ui->port_channel3_width_textEdit ||
       watched == EMG::ui->port_channel4_width_textEdit ||
       watched == EMG::ui->port_channel1_current_textEdit ||
       watched == EMG::ui->port_channel2_current_textEdit ||
       watched == EMG::ui->port_channel3_current_textEdit ||
       watched == EMG::ui->port_channel4_current_textEdit) {
        if(event->type() == QEvent::FocusIn) {
            EMG::ui->keyboard_confirm_pushButton->setEnabled(false);
        }
    }
    return QWidget::eventFilter(watched, event);
}

/*
 * 通道波形的选择
 */
void EMG::slot_set_channel_wave_form() {
    //qDebug() << 1 << '\n';
    int channelNum = EMG::now_selected_channel;
    QString settingURL = channelUrl[channelNum] + "/FormInfo";
    if(EMG::ui->sine_radioButton->isChecked()) {
        qDebug() << "正弦波" << '\n';
        EMG::drawSineWave(channelNum);
        EMG::settings->setValue(settingURL, SINE_WAVE);
        EMG::portChannels[channelNum].setChannelWaveForm(SINE_WAVE);
        switch(channelNum) {
            case 1:
                EMG::ui->port_channel1_wave_label_1->setText("正弦波");
                EMG::ui->port_channel1_freq_textEdit->setText("");
                EMG::ui->port_channel1_width_textEdit->setText("");
                EMG::ui->port_channel1_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel1_width_textEdit->setEnabled(true);
                break;
            case 2:
                EMG::ui->port_channel2_wave_label->setText("正弦波");
                EMG::ui->port_channel2_freq_textEdit->setText("");
                EMG::ui->port_channel2_width_textEdit->setText("");
                EMG::ui->port_channel2_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel2_width_textEdit->setEnabled(true);
                break;
            case 3:
                EMG::ui->port_channel3_wave_label->setText("正弦波");
                EMG::ui->port_channel3_freq_textEdit->setText("");
                EMG::ui->port_channel3_width_textEdit->setText("");
                EMG::ui->port_channel3_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel3_width_textEdit->setEnabled(true);
                break;
            case 4:
                EMG::ui->port_channel4_wave_label->setText("正弦波");
                EMG::ui->port_channel4_freq_textEdit->setText("");
                EMG::ui->port_channel4_width_textEdit->setText("");
                EMG::ui->port_channel4_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel4_width_textEdit->setEnabled(true);
                break;
            default:
                break;
        }
    }
    if(EMG::ui->trangular_radioButton->isChecked()) {
        qDebug() << "三角波" << '\n';
        EMG::drawTrangularWave(channelNum);
        EMG::settings->setValue(settingURL, TRANGULAR_WAVE);
        EMG::portChannels[channelNum].setChannelWaveForm(TRANGULAR_WAVE);
        switch(channelNum) {
            case 1:
                EMG::ui->port_channel1_wave_label_1->setText("三角波");
                EMG::ui->port_channel1_freq_textEdit->setText("");
                EMG::ui->port_channel1_width_textEdit->setText("");
                EMG::ui->port_channel1_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel1_width_textEdit->setEnabled(true);
                break;
            case 2:
                EMG::ui->port_channel2_wave_label->setText("三角波");
                EMG::ui->port_channel2_freq_textEdit->setText("");
                EMG::ui->port_channel2_width_textEdit->setText("");
                EMG::ui->port_channel2_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel2_width_textEdit->setEnabled(true);
                break;
            case 3:
                EMG::ui->port_channel3_wave_label->setText("三角波");
                EMG::ui->port_channel3_freq_textEdit->setText("");
                EMG::ui->port_channel3_width_textEdit->setText("");
                EMG::ui->port_channel3_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel3_width_textEdit->setEnabled(true);
                break;
            case 4:
                EMG::ui->port_channel4_wave_label->setText("三角波");
                EMG::ui->port_channel4_freq_textEdit->setText("");
                EMG::ui->port_channel4_width_textEdit->setText("");
                EMG::ui->port_channel4_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel4_width_textEdit->setEnabled(true);
                break;
            default:
                break;
        }
    }
    if(EMG::ui->constant_radioButton->isChecked()) {
        qDebug() << "恒波" << '\n';
        EMG::drawConstantWave(channelNum);
        EMG::settings->setValue(settingURL, CONSTANT_WAVE);
        EMG::portChannels[channelNum].setChannelWaveForm(CONSTANT_WAVE);
        switch(channelNum) {
            case 1:
                EMG::ui->port_channel1_wave_label_1->setText("恒波");
                EMG::ui->port_channel1_freq_textEdit->setText("无需设置");
                EMG::ui->port_channel1_width_textEdit->setText("无需设置");
                EMG::ui->port_channel1_freq_textEdit->setEnabled(false);
                EMG::ui->port_channel1_width_textEdit->setEnabled(false);
                break;
            case 2:
                EMG::ui->port_channel2_wave_label->setText("恒波");
                EMG::ui->port_channel2_freq_textEdit->setText("无需设置");
                EMG::ui->port_channel2_width_textEdit->setText("无需设置");
                EMG::ui->port_channel2_freq_textEdit->setEnabled(false);
                EMG::ui->port_channel2_width_textEdit->setEnabled(false);
                break;
            case 3:
                EMG::ui->port_channel3_wave_label->setText("恒波");
                EMG::ui->port_channel3_freq_textEdit->setText("无需设置");
                EMG::ui->port_channel3_width_textEdit->setText("无需设置");
                EMG::ui->port_channel3_freq_textEdit->setEnabled(false);
                EMG::ui->port_channel3_width_textEdit->setEnabled(false);
                break;
            case 4:
                EMG::ui->port_channel4_wave_label->setText("恒波");
                EMG::ui->port_channel4_freq_textEdit->setText("无需设置");
                EMG::ui->port_channel4_width_textEdit->setText("无需设置");
                EMG::ui->port_channel4_freq_textEdit->setEnabled(false);
                EMG::ui->port_channel4_width_textEdit->setEnabled(false);
                break;
            default:
                break;
        }
    }
    if(EMG::ui->square_radioButton->isChecked()) {
        qDebug() << "方波" << '\n';
        EMG::drawSquareWave(channelNum);
        EMG::settings->setValue(settingURL, SQUARE_WAVE);
        EMG::portChannels[channelNum].setChannelWaveForm(SQUARE_WAVE);
        switch(channelNum) {
            case 1:
                EMG::ui->port_channel1_wave_label_1->setText("方波");
                EMG::ui->port_channel1_freq_textEdit->setText("");
                EMG::ui->port_channel1_width_textEdit->setText("");
                EMG::ui->port_channel1_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel1_width_textEdit->setEnabled(true);
                break;
            case 2:
                EMG::ui->port_channel2_wave_label->setText("方波");
                EMG::ui->port_channel2_freq_textEdit->setText("");
                EMG::ui->port_channel2_width_textEdit->setText("");
                EMG::ui->port_channel2_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel2_width_textEdit->setEnabled(true);
                break;
            case 3:
                EMG::ui->port_channel3_wave_label->setText("方波");
                EMG::ui->port_channel3_freq_textEdit->setText("");
                EMG::ui->port_channel3_width_textEdit->setText("");
                EMG::ui->port_channel3_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel3_width_textEdit->setEnabled(true);
                break;
            case 4:
                EMG::ui->port_channel4_wave_label->setText("方波");
                EMG::ui->port_channel4_freq_textEdit->setText("");
                EMG::ui->port_channel4_width_textEdit->setText("");
                EMG::ui->port_channel4_freq_textEdit->setEnabled(true);
                EMG::ui->port_channel4_width_textEdit->setEnabled(true);
                break;
            default:
                break;
        }
    }
}

/*
 * 显示恒波
 */
void EMG::drawConstantWave(int channelNum) {
    QGraphicsView *view = nullptr;
    switch(channelNum) {
    case 1:
        view = EMG::ui->port_channel1_wave_graphicsView_1;
        break;
    case 2:
        view = EMG::ui->port_channel2_wave_graphicsView;
        break;
    case 3:
        view = EMG::ui->port_channel3_wave_graphicsView;
        break;
    case 4:
        view = EMG::ui->port_channel4_wave_graphicsView;
        break;
    default:
        break;
    }
    if(view == nullptr) {
        return;
    }
    QGraphicsScene *scene;
    if(view->scene()) {
        scene = view->scene();
        scene->clear();
    }
    else {
        scene = new QGraphicsScene;
        view->setScene(scene);
    }
    QGraphicsLineItem* item = new QGraphicsLineItem;
    item->setLine(20, 65, 120, 65);
    scene->addItem(item);
    view->show();
}

/*
 * 显示方波
 */
void EMG::drawSquareWave(int channelNum) {
    QGraphicsView *view = nullptr;
    switch(channelNum) {
    case 1:
        view = EMG::ui->port_channel1_wave_graphicsView_1;
        break;
    case 2:
        view = EMG::ui->port_channel2_wave_graphicsView;
        break;
    case 3:
        view = EMG::ui->port_channel3_wave_graphicsView;
        break;
    case 4:
        view = EMG::ui->port_channel4_wave_graphicsView;
        break;
    default:
        break;
    }
    if(view == nullptr) {
        return;
    }
    QGraphicsScene *scene;
    if(view->scene()) {
        scene = view->scene();
        scene->clear();
    }
    else {
        scene = new QGraphicsScene;
        view->setScene(scene);
    }
    QGraphicsPathItem *pItem = new QGraphicsPathItem;
    QPainterPath path;
    path.moveTo(20, 65);
    path.lineTo(40, 65);
    path.lineTo(40, 45);
    path.lineTo(60, 45);
    path.lineTo(60, 65);
    path.lineTo(80, 65);
    path.lineTo(80, 45);
    path.lineTo(100, 45);
    path.lineTo(100, 65);
    path.lineTo(120, 65);
    pItem->setPath(path);
    pItem->pen().setWidth(2);
    scene->addItem(pItem);
    view->show();
}

/*
 * 显示三角波
 */
void EMG::drawTrangularWave(int channelNum) {
    QGraphicsView *view = nullptr;
    switch(channelNum) {
    case 1:
        view = EMG::ui->port_channel1_wave_graphicsView_1;
        break;
    case 2:
        view = EMG::ui->port_channel2_wave_graphicsView;
        break;
    case 3:
        view = EMG::ui->port_channel3_wave_graphicsView;
        break;
    case 4:
        view = EMG::ui->port_channel4_wave_graphicsView;
        break;
    default:
        break;
    }
    if(view == nullptr) {
        return;
    }
    QGraphicsScene *scene;
    if(view->scene()) {
        scene = view->scene();
        scene->clear();
    }
    else {
        scene = new QGraphicsScene;
        view->setScene(scene);
    }
    QGraphicsPathItem *pItem = new QGraphicsPathItem;
    QPainterPath path;
    path.moveTo(20, 65);
    path.lineTo(50, 65);
    path.lineTo(60, 45);
    path.lineTo(80, 85);
    path.lineTo(90, 65);
    path.lineTo(120, 65);
    pItem->setPath(path);
    pItem->pen().setWidth(2);
    scene->addItem(pItem);
    view->show();
}

/*
 * 显示正弦波
 */
void EMG::drawSineWave(int channelNum) {
    QGraphicsView *view = nullptr;
    switch(channelNum) {
    case 1:
        view = EMG::ui->port_channel1_wave_graphicsView_1;
        break;
    case 2:
        view = EMG::ui->port_channel2_wave_graphicsView;
        break;
    case 3:
        view = EMG::ui->port_channel3_wave_graphicsView;
        break;
    case 4:
        view = EMG::ui->port_channel4_wave_graphicsView;
        break;
    default:
        break;
    }
    if(view == nullptr) {
        return;
    }
    QGraphicsScene *scene;
    if(view->scene()) {
        scene = view->scene();
        scene->clear();
    }
    else {
        scene = new QGraphicsScene;
        view->setScene(scene);
    }
    QGraphicsPathItem *pItem = new QGraphicsPathItem;
    QPainterPath path;
    path.moveTo(20, 65);
    path.lineTo(50, 65);
    double x = 50, y = 65;
    for (int i = 1; i <= 360; ++i) {
        double rad = i * PI / 180.0;
        double dy = sin(rad) * 20;
        double dx = 40 * i / 360;
        path.lineTo(x + dx, y + dy);
    }
    path.lineTo(120, 65);
    pItem->setPath(path);
    pItem->pen().setWidth(2);
    scene->addItem(pItem);
    view->show();
}

/*
 * 读取保存的通道属性
 */
void EMG::loadChannelSettings() {
    EMG::portChannels.resize(5);
    for(int channelNum = 1; channelNum <= 4; ++channelNum) {
        EMG::portChannels[channelNum].setChannelState(CHANNEL_SHUTDOWN);
        if(!EMG::settings->value(channelUrl[channelNum] + "/FormInfo").isNull()) {
            EMG::portChannels[channelNum].setChannelWaveForm(EMG::settings->value(channelUrl[channelNum] + "/FormInfo").toInt());
        }
        else {
            EMG::portChannels[channelNum].setChannelWaveForm(0);
        }
        if(!EMG::settings->value(channelUrl[channelNum] + "/FreqInfo").isNull()) {
            EMG::portChannels[channelNum].setChannelWaveFreq(EMG::settings->value(channelUrl[channelNum] + "/FreqInfo").toInt());
        }
        else {
            EMG::portChannels[channelNum].setChannelWaveFreq(0);
        }
        if(!EMG::settings->value(channelUrl[channelNum] + "/WidthInfo").isNull()) {
            EMG::portChannels[channelNum].setChannelWaveWidth(EMG::settings->value(channelUrl[channelNum] + "/WidthInfo").toInt());
        }
        else {
            EMG::portChannels[channelNum].setChannelWaveWidth(0);
        }
        if(!EMG::settings->value(channelUrl[channelNum] + "/TimeInfo").isNull()) {
            EMG::portChannels[channelNum].setChannelWaveTime(EMG::settings->value(channelUrl[channelNum] + "/TimeInfo").toInt());
        }
        else {
            EMG::portChannels[channelNum].setChannelWaveTime(0);
        }
        if(!EMG::settings->value(channelUrl[channelNum] + "/CurrentInfo").isNull()) {
            EMG::portChannels[channelNum].setChannelWaveCurrent(EMG::settings->value(channelUrl[channelNum] + "/CurrentInfo").toInt());
        }
        else {
            EMG::portChannels[channelNum].setChannelWaveCurrent(0);
        }
    }
}

/*
 * 更新通道一的UI
 */
void EMG::updateChannel1UI() {
    EMG::ui->constant_radioButton->setChecked(false);
    EMG::ui->sine_radioButton->setChecked(false);
    EMG::ui->square_radioButton->setChecked(false);
    EMG::ui->trangular_radioButton->setChecked(false);
    if(EMG::portChannels[1].getChannelState()) {
        EMG::ui->port_channel_state_label_1->setPixmap(EMG::runningPix);
    }
    else {
        EMG::ui->port_channel_state_label_1->setPixmap(EMG::shutdownPix);
    }
    switch(EMG::portChannels[1].getChannelWaveForm()) {
    case SINE_WAVE:
        EMG::drawSineWave(1);
        EMG::ui->sine_radioButton->setChecked(true);
        EMG::ui->port_channel1_wave_label_1->setText("正弦波");
        EMG::ui->port_channel1_freq_textEdit->setText("");
        EMG::ui->port_channel1_width_textEdit->setText("");
        EMG::ui->port_channel1_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel1_width_textEdit->setEnabled(true);
        break;
    case TRANGULAR_WAVE:
        EMG::drawTrangularWave(1);
        EMG::ui->trangular_radioButton->setChecked(true);
        EMG::ui->port_channel1_wave_label_1->setText("三角波");
        EMG::ui->port_channel1_freq_textEdit->setText("");
        EMG::ui->port_channel1_width_textEdit->setText("");
        EMG::ui->port_channel1_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel1_width_textEdit->setEnabled(true);
        break;
    case SQUARE_WAVE:
        EMG::drawSquareWave(1);
        EMG::ui->square_radioButton->setChecked(true);
        EMG::ui->port_channel1_wave_label_1->setText("方波");
        EMG::ui->port_channel1_freq_textEdit->setText("");
        EMG::ui->port_channel1_width_textEdit->setText("");
        EMG::ui->port_channel1_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel1_width_textEdit->setEnabled(true);
        break;
    case CONSTANT_WAVE:
        EMG::drawConstantWave(1);
        EMG::ui->constant_radioButton->setChecked(true);
        EMG::ui->port_channel1_wave_label_1->setText("恒波");
        EMG::ui->port_channel1_freq_textEdit->setText("无需设置");
        EMG::ui->port_channel1_width_textEdit->setText("无需设置");
        EMG::ui->port_channel1_freq_textEdit->setEnabled(false);
        EMG::ui->port_channel1_width_textEdit->setEnabled(false);
        break;
    default:
        break;
    }
    EMG::ui->port_channel1_current_textEdit->setText(QString::number(EMG::portChannels[1].getChannelWaveCurrent()));
    if(EMG::portChannels[1].getChannelWaveTime() == INF_TIME) {
        EMG::ui->port_channel1_time_textEdit->setText("持续");
    }
    else {
        EMG::ui->port_channel1_time_textEdit->setText(QString::number(EMG::portChannels[1].getChannelWaveTime()));
    }
    if(EMG::portChannels[1].getChannelWaveForm() != CONSTANT_WAVE) {
        EMG::ui->port_channel1_width_textEdit->setText(QString::number(EMG::portChannels[1].getChannelWaveWidth()));
        EMG::ui->port_channel1_freq_textEdit->setText(QString::number(EMG::portChannels[1].getChannelWaveFreq()));
    }
    EMG::ui->port_channel1_pause_pushButton->setEnabled(false);
}

/*
 * 更新通道二的UI
 */
void EMG::updateChannel2UI() {
    if(EMG::portChannels[2].getChannelState()) {
        EMG::ui->port_channel_state_label_2->setPixmap(EMG::runningPix);
    }
    else {
        EMG::ui->port_channel_state_label_2->setPixmap(EMG::shutdownPix);
    }
    switch(EMG::portChannels[2].getChannelWaveForm()) {
    case SINE_WAVE:
        EMG::drawSineWave(2);
        EMG::ui->port_channel2_wave_label->setText("正弦波");
        EMG::ui->port_channel2_freq_textEdit->setText("");
        EMG::ui->port_channel2_width_textEdit->setText("");
        EMG::ui->port_channel2_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel2_width_textEdit->setEnabled(true);
        break;
    case TRANGULAR_WAVE:
        EMG::drawTrangularWave(2);
        EMG::ui->port_channel2_wave_label->setText("三角波");
        EMG::ui->port_channel2_freq_textEdit->setText("");
        EMG::ui->port_channel2_width_textEdit->setText("");
        EMG::ui->port_channel2_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel2_width_textEdit->setEnabled(true);
        break;
    case SQUARE_WAVE:
        EMG::drawSquareWave(2);
        EMG::ui->port_channel2_wave_label->setText("方波");
        EMG::ui->port_channel2_freq_textEdit->setText("");
        EMG::ui->port_channel2_width_textEdit->setText("");
        EMG::ui->port_channel2_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel2_width_textEdit->setEnabled(true);
        break;
    case CONSTANT_WAVE:
        EMG::drawConstantWave(2);
        EMG::ui->port_channel2_wave_label->setText("恒波");
        EMG::ui->port_channel2_freq_textEdit->setText("无需设置");
        EMG::ui->port_channel2_width_textEdit->setText("无需设置");
        EMG::ui->port_channel2_freq_textEdit->setEnabled(false);
        EMG::ui->port_channel2_width_textEdit->setEnabled(false);
        break;
    default:
        break;
    }
    EMG::ui->port_channel2_current_textEdit->setText(QString::number(EMG::portChannels[2].getChannelWaveCurrent()));
    if(EMG::portChannels[2].getChannelWaveTime() == INF_TIME) {
        EMG::ui->port_channel2_time_textEdit->setText("持续");
    }
    else {
        EMG::ui->port_channel2_time_textEdit->setText(QString::number(EMG::portChannels[2].getChannelWaveTime()));
    }
    if(EMG::portChannels[2].getChannelWaveForm() != CONSTANT_WAVE) {
        EMG::ui->port_channel2_width_textEdit->setText(QString::number(EMG::portChannels[2].getChannelWaveWidth()));
        EMG::ui->port_channel2_freq_textEdit->setText(QString::number(EMG::portChannels[2].getChannelWaveFreq()));
    }
    EMG::ui->port_channel2_pause_pushButton->setEnabled(false);
}

/*
 * 更新通道三的UI
 */
void EMG::updateChannel3UI() {
    if(EMG::portChannels[3].getChannelState()) {
        EMG::ui->port_channel_state_label_3->setPixmap(EMG::runningPix);
    }
    else {
        EMG::ui->port_channel_state_label_3->setPixmap(EMG::shutdownPix);
    }
    switch(EMG::portChannels[3].getChannelWaveForm()) {
    case SINE_WAVE:
        EMG::drawSineWave(3);
        EMG::ui->port_channel3_wave_label->setText("正弦波");
        EMG::ui->port_channel3_freq_textEdit->setText("");
        EMG::ui->port_channel3_width_textEdit->setText("");
        EMG::ui->port_channel3_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel3_width_textEdit->setEnabled(true);
        break;
    case TRANGULAR_WAVE:
        EMG::drawTrangularWave(3);
        EMG::ui->port_channel3_wave_label->setText("三角波");
        EMG::ui->port_channel3_freq_textEdit->setText("");
        EMG::ui->port_channel3_width_textEdit->setText("");
        EMG::ui->port_channel3_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel3_width_textEdit->setEnabled(true);
        break;
    case SQUARE_WAVE:
        EMG::drawSquareWave(3);
        EMG::ui->port_channel3_wave_label->setText("方波");
        EMG::ui->port_channel3_freq_textEdit->setText("");
        EMG::ui->port_channel3_width_textEdit->setText("");
        EMG::ui->port_channel3_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel3_width_textEdit->setEnabled(true);
        break;
    case CONSTANT_WAVE:
        EMG::drawConstantWave(3);
        EMG::ui->port_channel3_wave_label->setText("恒波");
        EMG::ui->port_channel3_freq_textEdit->setText("无需设置");
        EMG::ui->port_channel3_width_textEdit->setText("无需设置");
        EMG::ui->port_channel3_freq_textEdit->setEnabled(false);
        EMG::ui->port_channel3_width_textEdit->setEnabled(false);
        break;
    default:
        break;
    }
    EMG::ui->port_channel3_current_textEdit->setText(QString::number(EMG::portChannels[3].getChannelWaveCurrent()));
    if(EMG::portChannels[3].getChannelWaveTime() == INF_TIME) {
        EMG::ui->port_channel3_time_textEdit->setText("持续");
    }
    else {
        EMG::ui->port_channel3_time_textEdit->setText(QString::number(EMG::portChannels[3].getChannelWaveTime()));
    }
    if(EMG::portChannels[3].getChannelWaveForm() != CONSTANT_WAVE) {
        EMG::ui->port_channel3_width_textEdit->setText(QString::number(EMG::portChannels[3].getChannelWaveWidth()));
        EMG::ui->port_channel3_freq_textEdit->setText(QString::number(EMG::portChannels[3].getChannelWaveFreq()));
    }
    EMG::ui->port_channel3_pause_pushButton->setEnabled(false);
}

/*
 * 更新通道四的UI
 */
void EMG::updateChannel4UI() {
    if(EMG::portChannels[4].getChannelState()) {
        EMG::ui->port_channel_state_label_4->setPixmap(EMG::runningPix);
    }
    else {
        EMG::ui->port_channel_state_label_4->setPixmap(EMG::shutdownPix);
    }
    switch(EMG::portChannels[4].getChannelWaveForm()) {
    case SINE_WAVE:
        EMG::drawSineWave(4);
        EMG::ui->port_channel4_wave_label->setText("正弦波");
        EMG::ui->port_channel4_freq_textEdit->setText("");
        EMG::ui->port_channel4_width_textEdit->setText("");
        EMG::ui->port_channel4_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel4_width_textEdit->setEnabled(true);
        break;
    case TRANGULAR_WAVE:
        EMG::drawTrangularWave(4);
        EMG::ui->port_channel4_wave_label->setText("三角波");
        EMG::ui->port_channel4_freq_textEdit->setText("");
        EMG::ui->port_channel4_width_textEdit->setText("");
        EMG::ui->port_channel4_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel4_width_textEdit->setEnabled(true);
        break;
    case SQUARE_WAVE:
        EMG::drawSquareWave(4);
        EMG::ui->port_channel4_wave_label->setText("方波");
        EMG::ui->port_channel4_freq_textEdit->setText("");
        EMG::ui->port_channel4_width_textEdit->setText("");
        EMG::ui->port_channel4_freq_textEdit->setEnabled(true);
        EMG::ui->port_channel4_width_textEdit->setEnabled(true);
        break;
    case CONSTANT_WAVE:
        EMG::drawConstantWave(4);
        EMG::ui->port_channel4_wave_label->setText("恒波");
        EMG::ui->port_channel4_freq_textEdit->setText("无需设置");
        EMG::ui->port_channel4_width_textEdit->setText("无需设置");
        EMG::ui->port_channel4_freq_textEdit->setEnabled(false);
        EMG::ui->port_channel4_width_textEdit->setEnabled(false);
        break;
    default:
        break;
    }
    EMG::ui->port_channel4_current_textEdit->setText(QString::number(EMG::portChannels[4].getChannelWaveCurrent()));
    if(EMG::portChannels[4].getChannelWaveTime() == INF_TIME) {
        EMG::ui->port_channel4_time_textEdit->setText("持续");
    }
    else {
        EMG::ui->port_channel4_time_textEdit->setText(QString::number(EMG::portChannels[4].getChannelWaveTime()));
    }
    if(EMG::portChannels[4].getChannelWaveForm() != CONSTANT_WAVE) {
        EMG::ui->port_channel4_width_textEdit->setText(QString::number(EMG::portChannels[4].getChannelWaveWidth()));
        EMG::ui->port_channel4_freq_textEdit->setText(QString::number(EMG::portChannels[4].getChannelWaveFreq()));
    }
    EMG::ui->port_channel4_pause_pushButton->setEnabled(false);
}

/*
 * 保存通道一的信息并且发送串口数据
 */
void EMG::on_port_channel1_shoot_pushButton_clicked() {
    int channelNum = 1;
    sendSerialTriggerMsg(channelNum);
    int waveTime = EMG::ui->port_channel1_time_textEdit->toPlainText().toInt();
    int waveForm = EMG::portChannels[channelNum].getChannelWaveForm();
    int waveFreq = EMG::portChannels[channelNum].getChannelWaveFreq();
    EMG::ui->port_channel1_shoot_pushButton->setEnabled(false);
    EMG::ui->port_channel1_pause_pushButton->setEnabled(true);
    EMG::ui->port_channel_state_label_1->setPixmap(EMG::runningPix);
    emit EmgMsgSignal(INFO_MSG, "通道一启动");
    if(waveTime != INF_TIME) {
        if(waveForm == CONSTANT_WAVE) {
            EMG::channel1Qtimer->setInterval(waveTime);
        }
        else {
            qDebug() << waveTime * 1000 / waveFreq << '\n';
            EMG::channel1Qtimer->setInterval(waveTime * 1000 / waveFreq);
        }
        EMG::channel1Qtimer->start();
    }
    EMG::runningChannel++;
    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = SHUTDOWN_MODEL;
        EMG::setPushButtonShutdownModel();
    }
}

/*
 * 保存通道二的信息并且发送串口数据
 */
void EMG::on_port_channel2_shoot_pushButton_clicked() {
    int channelNum = 2;
    sendSerialTriggerMsg(channelNum);
    int waveTime = EMG::ui->port_channel2_time_textEdit->toPlainText().toInt();
    int waveForm = EMG::portChannels[channelNum].getChannelWaveForm();
    int waveFreq = EMG::portChannels[channelNum].getChannelWaveFreq();
    EMG::ui->port_channel2_shoot_pushButton->setEnabled(false);
    EMG::ui->port_channel2_pause_pushButton->setEnabled(true);
    EMG::ui->port_channel_state_label_2->setPixmap(EMG::runningPix);
    emit EmgMsgSignal(INFO_MSG, "通道二启动");
    if(waveTime != INF_TIME) {
        if(waveForm == CONSTANT_WAVE) {
            EMG::channel2Qtimer->setInterval(waveTime);
        }
        else {
            qDebug() << waveTime * 1000 / waveFreq << '\n';
            EMG::channel2Qtimer->setInterval(waveTime * 1000 / waveFreq);
        }
        EMG::channel2Qtimer->start();
    }
    EMG::runningChannel++;
    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = SHUTDOWN_MODEL;
        EMG::setPushButtonShutdownModel();
    }
}

/*
 * 保存通道三的信息并且发送串口数据
 */
void EMG::on_port_channel3_shoot_pushButton_clicked() {
    int channelNum = 3;
    sendSerialTriggerMsg(channelNum);
    int waveTime = EMG::ui->port_channel3_time_textEdit->toPlainText().toInt();
    int waveForm = EMG::portChannels[channelNum].getChannelWaveForm();
    int waveFreq = EMG::portChannels[channelNum].getChannelWaveFreq();
    EMG::ui->port_channel3_shoot_pushButton->setEnabled(false);
    EMG::ui->port_channel3_pause_pushButton->setEnabled(true);
    EMG::ui->port_channel_state_label_3->setPixmap(EMG::runningPix);
    emit EmgMsgSignal(INFO_MSG, "通道三启动");
    if(waveTime != INF_TIME) {
        if(waveForm == CONSTANT_WAVE) {
            EMG::channel3Qtimer->setInterval(waveTime);
        }
        else {
            qDebug() << waveTime * 1000 / waveFreq << '\n';
            EMG::channel3Qtimer->setInterval(waveTime * 1000 / waveFreq);
        }
        EMG::channel3Qtimer->start();
    }
    EMG::runningChannel++;
    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = SHUTDOWN_MODEL;
        EMG::setPushButtonShutdownModel();
    }
}

/*
 * 保存通道四的信息并且发送串口数据
 */
void EMG::on_port_channel4_shoot_pushButton_clicked() {
    int channelNum = 4;
    sendSerialTriggerMsg(channelNum);
    int waveTime = EMG::ui->port_channel4_time_textEdit->toPlainText().toInt();
    int waveForm = EMG::portChannels[channelNum].getChannelWaveForm();
    int waveFreq = EMG::portChannels[channelNum].getChannelWaveFreq();
    EMG::ui->port_channel4_shoot_pushButton->setEnabled(false);
    EMG::ui->port_channel4_pause_pushButton->setEnabled(true);
    EMG::ui->port_channel_state_label_4->setPixmap(EMG::runningPix);
    emit EmgMsgSignal(INFO_MSG, "通道四启动");
    if(waveTime != INF_TIME) {
        if(waveForm == CONSTANT_WAVE) {
            EMG::channel4Qtimer->setInterval(waveTime);
        }
        else {
            qDebug() << waveTime * 1000 / waveFreq << '\n';
            EMG::channel4Qtimer->setInterval(waveTime * 1000 / waveFreq);
        }
        EMG::channel4Qtimer->start();
    }
    EMG::runningChannel++;
    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = SHUTDOWN_MODEL;
        EMG::setPushButtonShutdownModel();
    }
}

/*
 * 通道运行状态定时器1
 */
void EMG::slot_channel1QtimerTimeout() {
    EMG::ui->port_channel1_shoot_pushButton->setEnabled(true);
    EMG::ui->port_channel1_pause_pushButton->setEnabled(false);
    EMG::ui->port_channel_state_label_1->setPixmap(EMG::shutdownPix);
    if(EMG::channel1Qtimer->isActive()) {
        EMG::channel1Qtimer->stop();
    }

    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = RUNNING_MODEL;
        EMG::setPushButtonRunningModel();
    }
    EMG::runningChannel--;
}

/*
 * 通道运行状态定时器2
 */
void EMG::slot_channel2QtimerTimeout() {
    EMG::ui->port_channel2_shoot_pushButton->setEnabled(true);
    EMG::ui->port_channel2_pause_pushButton->setEnabled(false);
    EMG::ui->port_channel_state_label_2->setPixmap(EMG::shutdownPix);
    if(EMG::channel2Qtimer->isActive()) {
        EMG::channel2Qtimer->stop();
    }


    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = RUNNING_MODEL;
        EMG::setPushButtonRunningModel();
    }
    EMG::runningChannel--;
}

/*
 * 通道运行状态定时器3
 */
void EMG::slot_channel3QtimerTimeout() {
    EMG::ui->port_channel3_shoot_pushButton->setEnabled(true);
    EMG::ui->port_channel3_pause_pushButton->setEnabled(false);
    EMG::ui->port_channel_state_label_3->setPixmap(EMG::shutdownPix);
    if(EMG::channel3Qtimer->isActive()) {
        EMG::channel3Qtimer->stop();
    }


    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = RUNNING_MODEL;
        EMG::setPushButtonRunningModel();
    }
    EMG::runningChannel--;
}

/*
 * 通道运行状态定时器4
 */
void EMG::slot_channel4QtimerTimeout() {
    EMG::ui->port_channel4_shoot_pushButton->setEnabled(true);
    EMG::ui->port_channel4_pause_pushButton->setEnabled(false);
    EMG::ui->port_channel_state_label_4->setPixmap(EMG::shutdownPix);
    if(EMG::channel4Qtimer->isActive()) {
        EMG::channel4Qtimer->stop();
    }


    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = RUNNING_MODEL;
        EMG::setPushButtonRunningModel();
    }
    EMG::runningChannel--;
}

/*
 * 暂停通道一的输出
 */
void EMG::slot_port_channel1_pause_pushButtonClicked() {
    //留出空白供串口使用
    QByteArray sendData = "$A1P;c1;#";
    qint64 bytesWritten = EMG::emgSerialPort->write(sendData);
    qDebug() << sendData << "\n字数" << bytesWritten << "\n";
    EMG::ui->port_channel1_shoot_pushButton->setEnabled(true);
    EMG::ui->port_channel1_pause_pushButton->setEnabled(false);
    EMG::ui->port_channel_state_label_1->setPixmap(EMG::shutdownPix);
    if(EMG::channel1Qtimer->isActive()) {
        EMG::channel1Qtimer->stop();
    }


    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = RUNNING_MODEL;
        EMG::setPushButtonRunningModel();
    }
    EMG::runningChannel--;
}

/*
 * 暂停通道二的输出
 */
void EMG::slot_port_channel2_pause_pushButtonClicked() {
    QByteArray sendData = "$A1P;c2;#";
    qint64 bytesWritten = EMG::emgSerialPort->write(sendData);
    qDebug() << sendData << "\n字数" << bytesWritten << "\n";
    EMG::ui->port_channel2_shoot_pushButton->setEnabled(true);
    EMG::ui->port_channel2_pause_pushButton->setEnabled(false);
    EMG::ui->port_channel_state_label_2->setPixmap(EMG::shutdownPix);
    if(EMG::channel2Qtimer->isActive()) {
        EMG::channel2Qtimer->stop();
    }


    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = RUNNING_MODEL;
        EMG::setPushButtonRunningModel();
    }
    EMG::runningChannel--;
}

/*
 * 暂停通道三的输出
 */
void EMG::slot_port_channel3_pause_pushButtonClicked() {
    QByteArray sendData = "$A1P;c3;#";
    qint64 bytesWritten = EMG::emgSerialPort->write(sendData);
    qDebug() << sendData << "\n字数" << bytesWritten << "\n";
    EMG::ui->port_channel3_shoot_pushButton->setEnabled(true);
    EMG::ui->port_channel3_pause_pushButton->setEnabled(false);
    EMG::ui->port_channel_state_label_3->setPixmap(EMG::shutdownPix);
    if(EMG::channel3Qtimer->isActive()) {
        EMG::channel3Qtimer->stop();
    }


    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = RUNNING_MODEL;
        EMG::setPushButtonRunningModel();
    }
    EMG::runningChannel--;
}

/*
 * 暂停通道四的输出
 */
void EMG::slot_port_channel4_pause_pushButtonClicked() {
    QByteArray sendData = "$A1P;c4;#";
    qint64 bytesWritten = EMG::emgSerialPort->write(sendData);
    qDebug() << sendData << "\n字数" << bytesWritten << "\n";
    EMG::ui->port_channel4_shoot_pushButton->setEnabled(true);
    EMG::ui->port_channel4_pause_pushButton->setEnabled(false);
    EMG::ui->port_channel_state_label_4->setPixmap(EMG::shutdownPix);
    if(EMG::channel4Qtimer->isActive()) {
        EMG::channel4Qtimer->stop();
    }


    if(EMG::runningChannel == 4) {
        EMG::portAllChannelModel = RUNNING_MODEL;
        EMG::setPushButtonRunningModel();
    }
    EMG::runningChannel--;
}

/*
 * 控制四个通道启动、暂停
 */
void EMG::slot_port_all_channel_open_close_pushButtonClicked() {
    if(EMG::portAllChannelModel == RUNNING_MODEL) {
        // 串口通信
        QByteArray sendData = "$A1T;c1234;#";
        qint64 bytesWritten = EMG::emgSerialPort->write(sendData);
        qDebug() << sendData << "\n字数" << bytesWritten << "\n";


        EMG::portAllChannelModel = RUNNING_MODEL;
        //EMG::setPushButtonShutdownModel();

        int waveTime1 = EMG::ui->port_channel1_time_textEdit->toPlainText().toInt();
        int waveForm1 = EMG::portChannels[1].getChannelWaveForm();
        int waveFreq1 = EMG::portChannels[1].getChannelWaveFreq();
        EMG::ui->port_channel1_shoot_pushButton->setEnabled(false);
        EMG::ui->port_channel1_pause_pushButton->setEnabled(true);
        EMG::ui->port_channel_state_label_1->setPixmap(EMG::runningPix);
        emit EmgMsgSignal(INFO_MSG, "通道一启动");
        if(waveTime1 != INF_TIME) {
            if(waveForm1 == CONSTANT_WAVE) {
                EMG::channel1Qtimer->setInterval(waveTime1);
            }
            else {
                EMG::channel1Qtimer->setInterval(waveTime1 * 1000 / waveFreq1);
            }
            EMG::channel1Qtimer->start();
        }
        int waveTime2 = EMG::ui->port_channel2_time_textEdit->toPlainText().toInt();
        int waveForm2 = EMG::portChannels[2].getChannelWaveForm();
        int waveFreq2 = EMG::portChannels[2].getChannelWaveFreq();
        EMG::ui->port_channel2_shoot_pushButton->setEnabled(false);
        EMG::ui->port_channel2_pause_pushButton->setEnabled(true);
        EMG::ui->port_channel_state_label_2->setPixmap(EMG::runningPix);
        emit EmgMsgSignal(INFO_MSG, "通道二启动");
        if(waveTime2 != INF_TIME) {
            if(waveForm2 == CONSTANT_WAVE) {
                EMG::channel2Qtimer->setInterval(waveTime2);
            }
            else {
                EMG::channel2Qtimer->setInterval(waveTime2 * 1000 / waveFreq2);
            }
            EMG::channel2Qtimer->start();
        }
        int waveTime3 = EMG::ui->port_channel3_time_textEdit->toPlainText().toInt();
        int waveForm3 = EMG::portChannels[3].getChannelWaveForm();
        int waveFreq3 = EMG::portChannels[3].getChannelWaveFreq();
        EMG::ui->port_channel3_shoot_pushButton->setEnabled(false);
        EMG::ui->port_channel3_pause_pushButton->setEnabled(true);
        EMG::ui->port_channel_state_label_3->setPixmap(EMG::runningPix);
        emit EmgMsgSignal(INFO_MSG, "通道三启动");
        if(waveTime3 != INF_TIME) {
            if(waveForm3 == CONSTANT_WAVE) {
                EMG::channel3Qtimer->setInterval(waveTime3);
            }
            else {
                EMG::channel3Qtimer->setInterval(waveTime3 * 1000 / waveFreq3);
            }
            EMG::channel3Qtimer->start();
        }
        int waveTime4 = EMG::ui->port_channel4_time_textEdit->toPlainText().toInt();
        int waveForm4 = EMG::portChannels[4].getChannelWaveForm();
        int waveFreq4 = EMG::portChannels[4].getChannelWaveFreq();
        EMG::ui->port_channel4_shoot_pushButton->setEnabled(false);
        EMG::ui->port_channel4_pause_pushButton->setEnabled(true);
        EMG::ui->port_channel_state_label_4->setPixmap(EMG::runningPix);
        emit EmgMsgSignal(INFO_MSG, "通道四启动");
        if(waveTime4 != INF_TIME) {
            if(waveForm4 == CONSTANT_WAVE) {
                EMG::channel4Qtimer->setInterval(waveTime4);
            }
            else {
                EMG::channel4Qtimer->setInterval(waveTime4 * 1000 / waveFreq4);
            }
            EMG::channel4Qtimer->start();
        }

        EMG::runningChannel = 4;

    }
    else {
        //串口通信
        QByteArray sendData = "$A1P;c1234;#";
        qint64 bytesWritten = EMG::emgSerialPort->write(sendData);
        qDebug() << sendData << "\n字数" << bytesWritten << "\n";


        EMG::portAllChannelModel = RUNNING_MODEL;
        EMG::setPushButtonRunningModel();
        EMG::ui->port_channel1_shoot_pushButton->setEnabled(true);
        EMG::ui->port_channel1_pause_pushButton->setEnabled(false);
        EMG::ui->port_channel_state_label_1->setPixmap(EMG::shutdownPix);
        if(EMG::channel1Qtimer->isActive()) {
            EMG::channel1Qtimer->stop();
        }
        EMG::ui->port_channel2_shoot_pushButton->setEnabled(true);
        EMG::ui->port_channel2_pause_pushButton->setEnabled(false);
        EMG::ui->port_channel_state_label_2->setPixmap(EMG::shutdownPix);
        if(EMG::channel2Qtimer->isActive()) {
            EMG::channel2Qtimer->stop();
        }
        EMG::ui->port_channel3_shoot_pushButton->setEnabled(true);
        EMG::ui->port_channel3_pause_pushButton->setEnabled(false);
        EMG::ui->port_channel_state_label_3->setPixmap(EMG::shutdownPix);
        if(EMG::channel3Qtimer->isActive()) {
            EMG::channel3Qtimer->stop();
        }
        EMG::ui->port_channel4_shoot_pushButton->setEnabled(true);
        EMG::ui->port_channel4_pause_pushButton->setEnabled(false);
        EMG::ui->port_channel_state_label_4->setPixmap(EMG::shutdownPix);
        if(EMG::channel4Qtimer->isActive()) {
            EMG::channel4Qtimer->stop();
        }


        if(EMG::runningChannel == 4) {
            EMG::portAllChannelModel = RUNNING_MODEL;
            EMG::setPushButtonRunningModel();
        }
        EMG::runningChannel = 0;
    }
}

/*
 * 修改按钮port_all_channel_open_close_pushButton为运行模式
 */
void EMG::setPushButtonRunningModel() {
    EMG::ui->port_all_channel_open_close_pushButton->setStyleSheet("QPushButton{"
    "border-radius:8px;"
    "background-color:#54B345;"
    "border-style:none;"
    "font-size:20px;"
    "color:white;"
    "}"
    "QPushButton:hover{"
    "    background-color:#63E398;"
    "}"
    );
    EMG::ui->port_all_channel_open_close_pushButton->setText("一键触发");
}

/*
 * 修改按钮port_all_channel_open_close_pushButton为暂停模式
 */
void EMG::setPushButtonShutdownModel() {
    EMG::ui->port_all_channel_open_close_pushButton->setStyleSheet("QPushButton{"
    "border-radius:8px;"
    "background-color:#D76364;"
    "border-style:none;"
    "font-size:20px;"
    "color:white;"
    "}"
    "QPushButton:hover{"
    "    background-color:#EF7A6D;"
    "}"
    );
    EMG::ui->port_all_channel_open_close_pushButton->setText("一键停止");
}

/*
 * 串口初始化
 */
void EMG::initEMGSerialPort() {
    EMG::emgSerialPort = new QSerialPort("/dev/ttyUSB0");
    //EMG::emgSerialPort->setPortName("ttyUSB0");
    EMG::emgSerialPort->setBaudRate(QSerialPort::Baud115200);
    EMG::emgSerialPort->setDataBits(QSerialPort::Data8);
    EMG::emgSerialPort->setParity(QSerialPort::NoParity);
    EMG::emgSerialPort->setStopBits(QSerialPort::OneStop);
    if(EMG::emgSerialPort->open(QIODevice::WriteOnly)) {
        emit EmgMsgSignal(INFO_MSG, "电刺激设备已连接");
        qDebug() << "电刺激设备已连接\n";
    }
    else {
        emit EmgMsgSignal(ERROR_MSG, "电刺激设备初始化失败");
        qDebug() << "电刺激设备初始化失败\n";
    }
}

/*
 * 串口发送通道设置信息
 */
void EMG::sendSerialSetMsg(portChannelAttr channel, int channelNum) {
    QByteArray sendData = "$A1S;"; // 数据头 A1 机器码; S 设置参数
    sendData += "c" + QByteArray::number(channelNum) + ";"; // 选择通道
    sendData += "w" + QByteArray::number(channel.getChannelWaveForm()) + ";"; //波形
    if(channel.getChannelWaveForm() != CONSTANT_WAVE) {
        sendData += "f" + QByteArray::number(channel.getChannelWaveFreq()) + ";"; //频率
        sendData += "p" + QByteArray::number(channel.getChannelWaveWidth()) + ";"; //波宽
        sendData += "r";
    }
    else {
        sendData += "t";
    }
    //时间
    if(channel.getChannelWaveTime() == INF_TIME) {
        sendData += "e;";
    }
    else {
        sendData += QByteArray::number(channel.getChannelWaveTime()) + ";";
    }
    sendData += "o1;"; //方向默认 不能修改
    sendData += "i" + QByteArray::number(channel.getChannelWaveCurrent()) + ";";
    sendData += "#";
    qint64 bytesWritten = EMG::emgSerialPort->write(sendData);
    qDebug() << sendData << "\n字数" << bytesWritten << "\n";
}

/*
 * 通道一数据保存
 */
void EMG::on_port_channe11_save_pushButton_clicked() {
    int channelNum = 1;
    int waveFreq = 0, waveWidth = 0, waveTime = 0, waveCurrent = 0;
    int waveForm = EMG::portChannels[channelNum].getChannelWaveForm();
    if(waveForm != CONSTANT_WAVE) {
        waveFreq = EMG::ui->port_channel1_freq_textEdit->toPlainText().toInt();
        waveWidth = EMG::ui->port_channel1_width_textEdit->toPlainText().toInt();
        if(waveFreq < 1 || waveFreq > 1000) {
            emit EmgMsgSignal(ERROR_MSG, "频率应在1～1000之间，请输入有效数字");
            return;
        }
        if(waveWidth < 50 || waveWidth > 9950) {
            emit EmgMsgSignal(ERROR_MSG, "波宽应在50～9950之间，请输入有效数字");
            return;
        }
    }
    if(EMG::ui->port_channel1_time_textEdit->toPlainText() == "持续") {
        waveTime = INF_TIME;
    }
    else {
        waveTime = EMG::ui->port_channel1_time_textEdit->toPlainText().toInt();
    }
    waveCurrent = EMG::ui->port_channel1_current_textEdit->toPlainText().toInt();
    if(waveCurrent < 5 || waveCurrent > 9995) {
        emit EmgMsgSignal(ERROR_MSG, "电流强度应在5～9995之间，请输入有效数字");
        return;
    }
    EMG::portChannels[channelNum].setChannelWaveCurrent(waveCurrent);
    EMG::portChannels[channelNum].setChannelWaveFreq(waveFreq);
    EMG::portChannels[channelNum].setChannelWaveTime(waveTime);
    EMG::portChannels[channelNum].setChannelWaveWidth(waveWidth);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/CurrentInfo", waveCurrent);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/FreqInfo", waveFreq);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/TimeInfo", waveTime);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/WidthInfo", waveWidth);
    //emit EmgMsgSignal(ERROR_MSG, "按钮1按下");
    //留出空白给串口使用
    EMG::sendSerialSetMsg(EMG::portChannels[channelNum], channelNum);
}

/*
 * 通道二数据保存
 */
void EMG::on_port_channe12_save_pushButton_clicked() {
    int channelNum = 2;
    int waveFreq = 0, waveWidth = 0, waveTime = 0, waveCurrent = 0;
    int waveForm = EMG::portChannels[channelNum].getChannelWaveForm();
    if(waveForm != CONSTANT_WAVE) {
        waveFreq = EMG::ui->port_channel2_freq_textEdit->toPlainText().toInt();
        waveWidth = EMG::ui->port_channel2_width_textEdit->toPlainText().toInt();
        if(waveFreq < 1 || waveFreq > 1000) {
            emit EmgMsgSignal(ERROR_MSG, "频率应在1～1000之间，请输入有效数字");
            return;
        }
        if(waveWidth < 50 || waveWidth > 9950) {
            emit EmgMsgSignal(ERROR_MSG, "波宽应在50～9950之间，请输入有效数字");
            return;
        }
    }
    if(EMG::ui->port_channel2_time_textEdit->toPlainText() == "持续") {
        waveTime = INF_TIME;
    }
    else {
        waveTime = EMG::ui->port_channel2_time_textEdit->toPlainText().toInt();
    }
    waveCurrent = EMG::ui->port_channel2_current_textEdit->toPlainText().toInt();
    if(waveCurrent < 5 || waveCurrent > 9995) {
        emit EmgMsgSignal(ERROR_MSG, "电流强度应在5～9995之间，请输入有效数字");
        return;
    }
    EMG::portChannels[channelNum].setChannelWaveCurrent(waveCurrent);
    EMG::portChannels[channelNum].setChannelWaveFreq(waveFreq);
    EMG::portChannels[channelNum].setChannelWaveTime(waveTime);
    EMG::portChannels[channelNum].setChannelWaveWidth(waveWidth);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/CurrentInfo", waveCurrent);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/FreqInfo", waveFreq);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/TimeInfo", waveTime);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/WidthInfo", waveWidth);
    //emit EmgMsgSignal(ERROR_MSG, "按钮2按下");
    //留出空白给串口使用
    EMG::sendSerialSetMsg(EMG::portChannels[channelNum], channelNum);
}

/*
 * 通道三数据保存
 */
void EMG::on_port_channe13_save_pushButton_clicked() {
    int channelNum = 3;
    int waveFreq = 0, waveWidth = 0, waveTime = 0, waveCurrent = 0;
    int waveForm = EMG::portChannels[channelNum].getChannelWaveForm();
    if(waveForm != CONSTANT_WAVE) {
        waveFreq = EMG::ui->port_channel3_freq_textEdit->toPlainText().toInt();
        waveWidth = EMG::ui->port_channel3_width_textEdit->toPlainText().toInt();
        if(waveFreq < 1 || waveFreq > 1000) {
            emit EmgMsgSignal(ERROR_MSG, "频率应在1～1000之间，请输入有效数字");
            return;
        }
        if(waveWidth < 50 || waveWidth > 9950) {
            emit EmgMsgSignal(ERROR_MSG, "波宽应在50～9950之间，请输入有效数字");
            return;
        }
    }
    if(EMG::ui->port_channel3_time_textEdit->toPlainText() == "持续") {
        waveTime = INF_TIME;
    }
    else {
        waveTime = EMG::ui->port_channel3_time_textEdit->toPlainText().toInt();
    }
    waveCurrent = EMG::ui->port_channel3_current_textEdit->toPlainText().toInt();
    if(waveCurrent < 5 || waveCurrent > 9995) {
        emit EmgMsgSignal(ERROR_MSG, "电流强度应在5～9995之间，请输入有效数字");
        return;
    }
    EMG::portChannels[channelNum].setChannelWaveCurrent(waveCurrent);
    EMG::portChannels[channelNum].setChannelWaveFreq(waveFreq);
    EMG::portChannels[channelNum].setChannelWaveTime(waveTime);
    EMG::portChannels[channelNum].setChannelWaveWidth(waveWidth);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/CurrentInfo", waveCurrent);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/FreqInfo", waveFreq);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/TimeInfo", waveTime);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/WidthInfo", waveWidth);
    //emit EmgMsgSignal(ERROR_MSG, "按钮1按下");
    //留出空白给串口使用
    EMG::sendSerialSetMsg(EMG::portChannels[channelNum], channelNum);

}

/*
 * 通道四数据保存
 */
void EMG::on_port_channe14_save_pushButton_clicked() {
    int channelNum = 4;
    int waveFreq = 0, waveWidth = 0, waveTime = 0, waveCurrent = 0;
    int waveForm = EMG::portChannels[channelNum].getChannelWaveForm();
    if(waveForm != CONSTANT_WAVE) {
        waveFreq = EMG::ui->port_channel4_freq_textEdit->toPlainText().toInt();
        waveWidth = EMG::ui->port_channel4_width_textEdit->toPlainText().toInt();
        if(waveFreq < 1 || waveFreq > 1000) {
            emit EmgMsgSignal(ERROR_MSG, "频率应在1～1000之间，请输入有效数字");
            return;
        }
        if(waveWidth < 50 || waveWidth > 9950) {
            emit EmgMsgSignal(ERROR_MSG, "波宽应在50～9950之间，请输入有效数字");
            return;
        }
    }
    if(EMG::ui->port_channel4_time_textEdit->toPlainText() == "持续") {
        waveTime = INF_TIME;
    }
    else {
        waveTime = EMG::ui->port_channel4_time_textEdit->toPlainText().toInt();
    }
    waveCurrent = EMG::ui->port_channel4_current_textEdit->toPlainText().toInt();
    if(waveCurrent < 5 || waveCurrent > 9995) {
        emit EmgMsgSignal(ERROR_MSG, "电流强度应在5～9995之间，请输入有效数字");
        return;
    }
    EMG::portChannels[channelNum].setChannelWaveCurrent(waveCurrent);
    EMG::portChannels[channelNum].setChannelWaveFreq(waveFreq);
    EMG::portChannels[channelNum].setChannelWaveTime(waveTime);
    EMG::portChannels[channelNum].setChannelWaveWidth(waveWidth);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/CurrentInfo", waveCurrent);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/FreqInfo", waveFreq);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/TimeInfo", waveTime);
    EMG::settings->setValue(EMG::channelUrl[channelNum] + "/WidthInfo", waveWidth);
    //emit EmgMsgSignal(ERROR_MSG, "按钮1按下");
    //留出空白给串口使用
    EMG::sendSerialSetMsg(EMG::portChannels[channelNum], channelNum);
}

/*
 * 串口触发信息
 */
void EMG::sendSerialTriggerMsg(int channelNum) {
    QByteArray sendData = "$A1T;c" + QByteArray::number(channelNum) + ";#";
    qint64 bytesWritten = EMG::emgSerialPort->write(sendData);
    qDebug() << sendData << "\n字数" << bytesWritten << "\n";
}
