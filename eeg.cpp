#include "eeg.h"
#include "ui_eeg.h"

EEG::EEG(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EEG)
{
    ui->setupUi(this);
    lineChartinit();
    EEG::testTimer = new QTimer(this);
    ui->battery_progressBar->setRange(0,100);
    ui->battery_progressBar->setValue(100);
    xAxis_num = 0;
}

EEG::~EEG()
{
    delete ui;
}

void EEG::lineChartinit()
{
    connect(ui->LineChartWidget, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));//连接鼠标点击信号和槽
    connect(ui->LineChartWidget, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));//连接鼠标滚轮信号和槽
    connect(ui->LineChartWidget, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));//连接曲线选择信号和槽
    ui->LineChartWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	//设置交互方式
                                          QCP::iSelectLegend | QCP::iSelectPlottables);

    QBrush qBrush(0xd6d3ce);//
    ui->LineChartWidget->setBackground(qBrush);

    ui->LineChartWidget->legend->setVisible(true);
    ui->LineChartWidget->legend->setBrush(qBrush);
    ui->LineChartWidget->legend->setIconSize(15,1);


    QFont qfont;
    qfont.setPointSize(6);
    ui->LineChartWidget->legend->setFont(qfont);
    ui->LineChartWidget->xAxis->setTicks(false);
//     ui->LineChartWidget->xAxis->setRange(0.0,GRAPH_POINT_NUM*XAXIS_STEP);
    ui->LineChartWidget->yAxis->setTicks(false);
    ui->LineChartWidget->yAxis->setAutoTicks(true);
    ui->LineChartWidget->yAxis->setAutoTickStep(true);
    ui->LineChartWidget->yAxis->setAutoSubTicks(true);
    ui->LineChartWidget->yAxis->setRange(6000.0,15000.0);

    graph_FC5 = ui->LineChartWidget->addGraph();
    graph_FC6 = ui->LineChartWidget->addGraph();
    graph_F3 = ui->LineChartWidget->addGraph();
    graph_F4 = ui->LineChartWidget->addGraph();
    graph_AF3 = ui->LineChartWidget->addGraph();
    graph_AF4 = ui->LineChartWidget->addGraph();
    graph_F7 = ui->LineChartWidget->addGraph();
    graph_F8 = ui->LineChartWidget->addGraph();
    graph_T7 = ui->LineChartWidget->addGraph();
    graph_T8 = ui->LineChartWidget->addGraph();
    graph_P7 = ui->LineChartWidget->addGraph();
    graph_P8 = ui->LineChartWidget->addGraph();
    graph_O1 = ui->LineChartWidget->addGraph();
    graph_O2 = ui->LineChartWidget->addGraph();

    QPen qPen1(QColor(255,0,0));
    graph_FC5->setPen(qPen1);
    graph_FC5->setName("FC5");

    qPen1.setColor(QColor(0,255,0));
    graph_FC6->setPen(qPen1);
    graph_FC6->setName("FC6");

    qPen1.setColor(QColor(0,0,255));
    graph_F3->setPen(qPen1);
    graph_F3->setName("F3");

    qPen1.setColor(QColor(0,255,255));
    graph_F4->setPen(qPen1);
    graph_F4->setName("F4");

    qPen1.setColor(QColor(255,0,255));
    graph_AF3->setPen(qPen1);
    graph_AF3->setName("AF3");

    qPen1.setColor(QColor(255,255,0));
    graph_AF4->setPen(qPen1);
    graph_AF4->setName("AF4");

    qPen1.setColor(QColor(255,128,0));
    graph_F7->setPen(qPen1);
    graph_F7->setName("F7");

    qPen1.setColor(QColor(128,255,0));
    graph_F8->setPen(qPen1);
    graph_F8->setName("F8");

    qPen1.setColor(QColor(0,255,128));
    graph_T7->setPen(qPen1);
    graph_T7->setName("T7");

    qPen1.setColor(QColor(0,128,255));
    graph_T8->setPen(qPen1);
    graph_T8->setName("T8");

    qPen1.setColor(QColor(255,0,128));
    graph_P7->setPen(qPen1);
    graph_P7->setName("P7");

    qPen1.setColor(QColor(128,0,255));
    graph_P8->setPen(qPen1);
    graph_P8->setName("P8");

    qPen1.setColor(QColor(128,0,128));
    graph_O1->setPen(qPen1);
    graph_O1->setName("O1");

    qPen1.setColor(QColor(0,128,128));
    graph_O2->setPen(qPen1);
    graph_O2->setName("O2");

    ui->LineChartWidget->rescaleAxes(true);
    ui->LineChartWidget->replot();

}

#define XAXIS_STEP 0.2
#define GRAPH_POINT_NUM 250
void EEG::lineChartAddData(emokit_signal es)
{
    xAxis_num += XAXIS_STEP;

    if(xAxis_num/XAXIS_STEP > GRAPH_POINT_NUM)
    {
        graph_FC5->clearData();
        graph_FC6->clearData();
        graph_F3->clearData();
        graph_F4->clearData();
        graph_AF3->clearData();
        graph_AF4->clearData();
        graph_F7->clearData();
        graph_F8->clearData();
        graph_T7->clearData();
        graph_T8->clearData();
        graph_P7->clearData();
        graph_P8->clearData();
        graph_O1->clearData();
        graph_O2->clearData();
        xAxis_num = 0;
    }
    graph_FC5->addData(xAxis_num,es.FC5);
    graph_FC6->addData(xAxis_num,es.FC6);
    graph_F3->addData(xAxis_num,es.F3);
    graph_F4->addData(xAxis_num,es.F4);
    graph_AF3->addData(xAxis_num,es.AF3);
    graph_AF4->addData(xAxis_num,es.AF4);
    graph_F7->addData(xAxis_num,es.F7);
    graph_F8->addData(xAxis_num,es.F8);
    graph_T7->addData(xAxis_num,es.T7);
    graph_T8->addData(xAxis_num,es.T8);
    graph_P7->addData(xAxis_num,es.P7);
    graph_P8->addData(xAxis_num,es.P8);
    graph_O1->addData(xAxis_num,es.O1);
    graph_O2->addData(xAxis_num,es.O2);

    ui->LineChartWidget->rescaleAxes(true);
    ui->LineChartWidget->replot();
}

void EEG::batterySlot(unsigned char bat)
{
    if(bat > 20)
    {
        ui->battery_progressBar->setStyleSheet("QProgressBar::chunk {width: 6px;margin-top: 2px;margin-bottom: 2px;margin-left: 2px;margin-right: 2px;background-color: green;}");
    }
    else
    {
        ui->battery_progressBar->setStyleSheet("QProgressBar::chunk {width: 6px;margin-top: 2px;margin-bottom: 2px;margin-left: 2px;margin-right: 2px;background-color: red;}");
    }
    ui->battery_progressBar->setValue(int(bat));
}
int EEG::signalQualityValue(short v)
{
    if(v>4000) return 3;
    if(v>3000) return 2;
    if(v>2000) return 1;

    return 0;

}
void EEG::signalQualitySlot(emokit_contact_quality cq)
{
    switch(signalQualityValue(cq.AF3))
    {
        case 0: ui->AF3label->setStyleSheet("background-color:black");break;
        case 1: ui->AF3label->setStyleSheet("background-color:red");break;
        case 2: ui->AF3label->setStyleSheet("background-color:yellow");break;
        case 3: ui->AF3label->setStyleSheet("background-color:green");break;
    }

    switch(signalQualityValue(cq.AF4))
    {
        case 0: ui->AF4label->setStyleSheet("background-color:black");break;
        case 1: ui->AF4label->setStyleSheet("background-color:red");break;
        case 2: ui->AF4label->setStyleSheet("background-color:yellow");break;
        case 3: ui->AF4label->setStyleSheet("background-color:green");break;
    }

    switch(signalQualityValue(cq.F3))
    {
        case 0: ui->F3label->setStyleSheet("background-color:black");break;
        case 1: ui->F3label->setStyleSheet("background-color:red");break;
        case 2: ui->F3label->setStyleSheet("background-color:yellow");break;
        case 3: ui->F3label->setStyleSheet("background-color:green");break;
    }

    switch(signalQualityValue(cq.F4))
    {
        case 0: ui->F4label->setStyleSheet("background-color:black");break;
        case 1: ui->F4label->setStyleSheet("background-color:red");break;
        case 2: ui->F4label->setStyleSheet("background-color:yellow");break;
        case 3: ui->F4label->setStyleSheet("background-color:green");break;
    }

    switch(signalQualityValue(cq.F7))
    {
        case 0: ui->F7label->setStyleSheet("background-color:black");break;
        case 1: ui->F7label->setStyleSheet("background-color:red");break;
        case 2: ui->F7label->setStyleSheet("background-color:yellow");break;
        case 3: ui->F7label->setStyleSheet("background-color:green");break;
    }

    switch(signalQualityValue(cq.F8))
    {
        case 0: ui->F8label->setStyleSheet("background-color:black");break;
        case 1: ui->F8label->setStyleSheet("background-color:red");break;
        case 2: ui->F8label->setStyleSheet("background-color:yellow");break;
        case 3: ui->F8label->setStyleSheet("background-color:green");break;
    }

    switch(signalQualityValue(cq.FC5))
    {
        case 0: ui->Fc5label->setStyleSheet("background-color:black");break;
        case 1: ui->Fc5label->setStyleSheet("background-color:red");break;
        case 2: ui->Fc5label->setStyleSheet("background-color:yellow");break;
        case 3: ui->Fc5label->setStyleSheet("background-color:green");break;
    }

    switch(signalQualityValue(cq.FC6))
    {
        case 0: ui->Fc6label->setStyleSheet("background-color:black");break;
        case 1: ui->Fc6label->setStyleSheet("background-color:red");break;
        case 2: ui->Fc6label->setStyleSheet("background-color:yellow");break;
        case 3: ui->Fc6label->setStyleSheet("background-color:green");break;
    }

    switch(signalQualityValue(cq.O1))
    {
        case 0: ui->O1label->setStyleSheet("background-color:black");break;
        case 1: ui->O1label->setStyleSheet("background-color:red");break;
        case 2: ui->O1label->setStyleSheet("background-color:yellow");break;
        case 3: ui->O1label->setStyleSheet("background-color:green");break;
    }

    switch(signalQualityValue(cq.O2))
    {
        case 0: ui->O2label->setStyleSheet("background-color:black");break;
        case 1: ui->O2label->setStyleSheet("background-color:red");break;
        case 2: ui->O2label->setStyleSheet("background-color:yellow");break;
        case 3: ui->O2label->setStyleSheet("background-color:green");break;
    }

    switch(signalQualityValue(cq.T7))
    {
        case 0: ui->T7label->setStyleSheet("background-color:black");break;
        case 1: ui->T7label->setStyleSheet("background-color:red");break;
        case 2: ui->T7label->setStyleSheet("background-color:yellow");break;
        case 3: ui->T7label->setStyleSheet("background-color:green");break;
    }
    switch(signalQualityValue(cq.T8))
    {
        case 0: ui->T8label->setStyleSheet("background-color:black");break;
        case 1: ui->T8label->setStyleSheet("background-color:red");break;
        case 2: ui->T8label->setStyleSheet("background-color:yellow");break;
        case 3: ui->T8label->setStyleSheet("background-color:green");break;
    }
    switch(signalQualityValue(cq.P7))
    {
        case 0: ui->P7label->setStyleSheet("background-color:black");break;
        case 1: ui->P7label->setStyleSheet("background-color:red");break;
        case 2: ui->P7label->setStyleSheet("background-color:yellow");break;
        case 3: ui->P7label->setStyleSheet("background-color:green");break;
    }
    switch(signalQualityValue(cq.P8))
    {
        case 0: ui->P8label->setStyleSheet("background-color:black");break;
        case 1: ui->P8label->setStyleSheet("background-color:red");break;
        case 2: ui->P8label->setStyleSheet("background-color:yellow");break;
        case 3: ui->P8label->setStyleSheet("background-color:green");break;
    }
}

void EEG::siganlValueSlot(emokit_signal es)
{

    lineChartAddData(es);
}



/**
 * @brief MainWindow::mousePress
 */
void EEG::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged
  if (ui->LineChartWidget->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    ui->LineChartWidget->axisRect()->setRangeDrag(ui->LineChartWidget->xAxis->orientation());
  }
  else if (ui->LineChartWidget->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    ui->LineChartWidget->axisRect()->setRangeDrag(ui->LineChartWidget->yAxis->orientation());
  }
  else
  {
    ui->LineChartWidget->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
  }
}

/**
 * @brief MainWindow::mouseWheel
 */
void EEG::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed
  if (ui->LineChartWidget->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    ui->LineChartWidget->axisRect()->setRangeZoom(ui->LineChartWidget->xAxis->orientation());
  }
  else if (ui->LineChartWidget->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    ui->LineChartWidget->axisRect()->setRangeZoom(ui->LineChartWidget->yAxis->orientation());
  }
  else
  {
    ui->LineChartWidget->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
  }
}

/**
 * @brief MainWindow::selectionChanged
 */
void EEG::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->LineChartWidget->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->LineChartWidget->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->LineChartWidget->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->LineChartWidget->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->LineChartWidget->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->LineChartWidget->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->LineChartWidget->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->LineChartWidget->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->LineChartWidget->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->LineChartWidget->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->LineChartWidget->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->LineChartWidget->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<ui->LineChartWidget->graphCount(); ++i)
  {
    QCPGraph *graph = ui->LineChartWidget->graph(i);
    QCPPlottableLegendItem *item = ui->LineChartWidget->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }
}

