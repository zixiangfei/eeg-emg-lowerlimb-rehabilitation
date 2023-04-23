#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QObject>
#include <QTimer>

typedef struct CPU_OCCUPY
{
    char name[20];
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
}CPU_OCCUPY;

typedef struct MEM_OCCUPY
{
    char name1[20];
    unsigned int total;
    char name2[20];
    unsigned int free;
}MEM_OCCUPY;

class SystemInfo : public QObject
{
    Q_OBJECT
public:
    SystemInfo();
    ~SystemInfo();

private:
    void getMEM_OCCUPY();
    void getCPU_OCCUPY();

    CPU_OCCUPY CPU_INFO_o;
    CPU_OCCUPY CPU_INFO_n;
    MEM_OCCUPY MEM_INFO;
    double CPUUsage;
    double MEMUsage;
    QTimer *qtimer;

private slots:
    void timerOutSlot();

signals:
    void CPUUsageSignal(double u);
    void MEMUsageSignal(double u);

};

#endif // SYSTEMINFO_H
