#include "systeminfo.h"
#include <stdio.h>
#include <string.h>
SystemInfo::SystemInfo()
{
    CPUUsage = 0;
    MEMUsage = 0;
    qtimer = new QTimer();
    qtimer->setInterval(1500);
    qtimer->start();
    connect(qtimer,SIGNAL(timeout()),this,SLOT(timerOutSlot()));
}

SystemInfo::~SystemInfo()
{
    qtimer->stop();
    delete qtimer;
}

void SystemInfo::getCPU_OCCUPY()
{
    FILE *fd;
    char buf[256]={'\0'};

    unsigned int od, nd;
    unsigned int id, sd;

    fd = fopen("/proc/stat","r");

    memcpy(CPU_INFO_o.name, CPU_INFO_n.name, sizeof(CPU_INFO_n.name));
    CPU_INFO_o.user = CPU_INFO_n.user;
    CPU_INFO_o.nice = CPU_INFO_n.nice;
    CPU_INFO_o.system = CPU_INFO_n.system;
    CPU_INFO_o.idle = CPU_INFO_n.idle;

    fgets(buf, sizeof(buf), fd);
    sscanf(buf, "%s %u %u %u %u", CPU_INFO_n.name, &CPU_INFO_n.user, &CPU_INFO_n.nice, &CPU_INFO_n.system, &CPU_INFO_n.idle);

    fclose(fd);

    od = CPU_INFO_o.user + CPU_INFO_o.nice + CPU_INFO_o.system + CPU_INFO_o.idle;
    nd = CPU_INFO_n.user + CPU_INFO_n.nice + CPU_INFO_n.system + CPU_INFO_n.idle;

    id = CPU_INFO_n.user + CPU_INFO_n.system + CPU_INFO_n.nice;
    sd = CPU_INFO_o.user + CPU_INFO_o.system + CPU_INFO_o.nice;

    if(nd != od)
    {
        CPUUsage = (id - sd)*100.0/(nd-od);
        if(CPUUsage > 100.0)
            CPUUsage = 0.0;
    }
    else
    {
        CPUUsage = 0.0;
    }
}
void SystemInfo::getMEM_OCCUPY()
{
    FILE *fd;
    char buf[256]={'\0'};

    fd = fopen("/proc/meminfo","r");

    fgets(buf, sizeof(buf), fd);
    sscanf(buf, "%s %u", MEM_INFO.name1, &MEM_INFO.total);

    fgets(buf, sizeof(buf), fd);
    sscanf(buf, "%s %u", MEM_INFO.name2, &MEM_INFO.free);


    MEMUsage = 100.0 - (MEM_INFO.free*1.0/MEM_INFO.total)*100.0;
    if(MEMUsage > 100.0)
        MEMUsage = 0.0;
    fclose(fd);
}



void SystemInfo::timerOutSlot()
{
    getCPU_OCCUPY();
    getMEM_OCCUPY();

    emit CPUUsageSignal(CPUUsage);
    emit MEMUsageSignal(MEMUsage);
}
