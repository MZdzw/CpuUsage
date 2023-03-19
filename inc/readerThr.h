#ifndef READERTHR_H
#define READERTHR_H

#define OK 1
#define FILE_OPEN_ERROR -1
#define FILE_READ_ERROR -2

struct CpuUsageStats
{
    unsigned long t_user;
    unsigned long t_nice;
    unsigned long t_system;
    unsigned long t_idle;
    unsigned long t_iowait;
    unsigned long t_irq;
    unsigned long t_softirq;
    unsigned long t_steal;
    unsigned long t_guest;
    unsigned long t_guestNice;
};

void* readerThread(void* arg);

#endif
