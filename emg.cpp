#include "emg.h"
#include "ui_emg.h"

EMG::EMG(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EMG)
{
    ui->setupUi(this);
    //loadStyleSheet("emgStyle");
    QPixmap init_pixmap(":/Image/channel_shutdown.png");
    int label_width = ui->port_channel_state_label_1->width(), label_height = ui->port_channel_state_label_1->height();
    QPixmap fixmap = init_pixmap.scaled(label_width, label_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);//按比例缩放
    ui->port_channel_state_label_1->setPixmap(fixmap);
    ui->port_channel_state_label_2->setPixmap(fixmap);
    ui->port_channel_state_label_3->setPixmap(fixmap);
    ui->port_channel_state_label_4->setPixmap(fixmap);
    //初始化图案，测试用

    //connect(ui->port_channel_tabWidget, SIGNAL(tabBarClicked(int)), this, SLOT(on_port_channel_tabWidget_tabBarClicked(int))); //绑定切换通道事件
    //使用connectSlotByName就不用在代码中connect否则会触发两次
    EMG::now_selected_channel = ui->port_channel_tabWidget->currentIndex();
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
    EMG::now_selected_channel = index;
    //qDebug() << "now selected tab: " << index << '\n';
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
    if(EMG::ui->sine_radioButton->isChecked()) {
        qDebug() << "正弦波" << '\n';
    }
    if(EMG::ui->trangular_radioButton->isChecked()) {
        qDebug() << "三角波" << '\n';
    }
    if(EMG::ui->constant_radioButton->isChecked()) {
        qDebug() << "恒波" << '\n';
    }
    if(EMG::ui->square_radioButton->isChecked()) {
        qDebug() << "方波" << '\n';
    }
}
