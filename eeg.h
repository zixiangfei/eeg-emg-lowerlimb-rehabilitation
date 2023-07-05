#ifndef EEG_H
#define EEG_H

#include <QWidget>
#include "includes.h"
#include "emokit/emokit.h"
#include "qcustomplot.h"
#include <QTimer>

namespace Ui {
class EEG;
}

class EEG : public QWidget
{
    Q_OBJECT

public:
    explicit EEG(QWidget *parent = 0);
    ~EEG();

private slots:
    void mousePress();
    void mouseWheel();
    void selectionChanged();
    void batterySlot(unsigned char bat);
    void signalQualitySlot(emokit_contact_quality cq);
    void siganlValueSlot(emokit_signal es);

private:
    void lineChartinit();
    int signalQualityValue(short v);
    void lineChartAddData(emokit_signal es);
private:
    Ui::EEG *ui;
    QCPGraph *graph_FC5;
    QCPGraph *graph_FC6;
    QCPGraph *graph_F3;
    QCPGraph *graph_F4;
    QCPGraph *graph_AF3;
    QCPGraph *graph_AF4;
    QCPGraph *graph_F7;
    QCPGraph *graph_F8;
    QCPGraph *graph_T7;
    QCPGraph *graph_T8;
    QCPGraph *graph_P7;
    QCPGraph *graph_P8;
    QCPGraph *graph_O1;
    QCPGraph *graph_O2;
    //QTimer *testTimer;

    double xAxis_num;

};

#endif // EEG_H
