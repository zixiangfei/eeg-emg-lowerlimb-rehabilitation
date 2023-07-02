#include "emg.h"
#include "ui_emg.h"

EMG::EMG(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EMG)
{
    ui->setupUi(this);
    //loadStyleSheet("emgStyle");
    EMG::settings = new QSettings("Settings.ini");
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
    //更新UI
    EMG::updateChannel1UI();
    EMG::updateChannel2UI();
    EMG::updateChannel3UI();
    EMG::updateChannel4UI();
//    for(int i = 1; i <= 4; ++i) {
//        qDebug()<<i<<'\n';
//        qDebug()<<EMG::portChannels[i].getChannelState()<<'\n';
//        qDebug()<<EMG::portChannels[i].getChannelWaveCurrent()<<'\n';
//        qDebug()<<EMG::portChannels[i].getChannelWaveForm()<<'\n';
//        qDebug()<<EMG::portChannels[i].getChannelWaveFreq()<<'\n';
//        qDebug()<<EMG::portChannels[i].getChannelWaveTime()<<'\n';
//        qDebug()<<EMG::portChannels[i].getChannelWaveWidth()<<'\n';
//    }

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
        EMG::ui->port_channel1_time_textEdit->setText(QString::number(EMG::portChannels[1].getChannelWaveCurrent()));
    }
    if(EMG::portChannels[1].getChannelWaveForm() != CONSTANT_WAVE) {
        EMG::ui->port_channel1_width_textEdit->setText(QString::number(EMG::portChannels[1].getChannelWaveWidth()));
        EMG::ui->port_channel1_freq_textEdit->setText(QString::number(EMG::portChannels[1].getChannelWaveFreq()));
    }
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
        EMG::ui->port_channel2_time_textEdit->setText(QString::number(EMG::portChannels[2].getChannelWaveCurrent()));
    }
    if(EMG::portChannels[2].getChannelWaveForm() != CONSTANT_WAVE) {
        EMG::ui->port_channel2_width_textEdit->setText(QString::number(EMG::portChannels[2].getChannelWaveWidth()));
        EMG::ui->port_channel2_freq_textEdit->setText(QString::number(EMG::portChannels[2].getChannelWaveFreq()));
    }
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
        EMG::ui->port_channel3_time_textEdit->setText(QString::number(EMG::portChannels[3].getChannelWaveCurrent()));
    }
    if(EMG::portChannels[3].getChannelWaveForm() != CONSTANT_WAVE) {
        EMG::ui->port_channel3_width_textEdit->setText(QString::number(EMG::portChannels[3].getChannelWaveWidth()));
        EMG::ui->port_channel3_freq_textEdit->setText(QString::number(EMG::portChannels[3].getChannelWaveFreq()));
    }
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
        EMG::ui->port_channel4_time_textEdit->setText(QString::number(EMG::portChannels[4].getChannelWaveCurrent()));
    }
    if(EMG::portChannels[4].getChannelWaveForm() != CONSTANT_WAVE) {
        EMG::ui->port_channel4_width_textEdit->setText(QString::number(EMG::portChannels[4].getChannelWaveWidth()));
        EMG::ui->port_channel4_freq_textEdit->setText(QString::number(EMG::portChannels[4].getChannelWaveFreq()));
    }
}

/*
 * 保存通道一的信息并且发送串口数据
 */
void EMG::on_port_channel1_shoot_pushButton_clicked() {
    qDebug() << "test" <<'\n';
}

/*
 * 保存通道二的信息并且发送串口数据
 */
void EMG::on_port_channel2_shoot_pushButton_clicked() {

}

/*
 * 保存通道三的信息并且发送串口数据
 */
void EMG::on_port_channel3_shoot_pushButton_clicked() {

}

/*
 * 保存通道四的信息并且发送串口数据
 */
void EMG::on_port_channel4_shoot_pushButton_clicked() {

}
