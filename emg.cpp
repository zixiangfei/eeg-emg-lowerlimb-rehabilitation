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
}

EMG::~EMG()
{
    delete ui;
}

void EMG::loadStyleSheet(QString name)
{
    QFile file(QString("://QSS/%1.css").arg(name));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
}
