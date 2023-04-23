#include "setting.h"
#include "ui_setting.h"
#include "includes.h"
#include "IcoHelper/iconhelper.h"

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);

    settings = new QSettings("Settings.ini");
    if(!settings->value(SYSTEMINFO_INI).isNull())
    {
        bool b = settings->value(SYSTEMINFO_INI).toBool();
        ui->systeminfo_radioButton->setChecked(b);
        if(b == true)
            ui->systeminfo_radioButton->setText(tr("OFF"));
        else
            ui->systeminfo_radioButton->setText(tr("ON"));
    }
}

Setting::~Setting()
{
    delete settings;
    delete ui;
}

void Setting::on_systeminfo_radioButton_clicked()
{
    if(ui->systeminfo_radioButton->isChecked())
    {
        emit systemInfoSignal(true);
        emit systeminfoMsgSignal(INFO_MSG, "open CPU and MEM");
        ui->systeminfo_radioButton->setText(tr("OFF"));
        settings->setValue(SYSTEMINFO_INI,true);
    }
    else
    {
        emit systemInfoSignal(false);
        emit systeminfoMsgSignal(INFO_MSG, "close CPU and MEM");
        ui->systeminfo_radioButton->setText(tr("ON"));
        settings->setValue(SYSTEMINFO_INI,false);
    }
}


void Setting::on_exit_pushButton_clicked()
{
    qApp->exit();
}
