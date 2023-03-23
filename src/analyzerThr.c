#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "analyzerThr.h"
#include "queue.h"

extern unsigned int threadsNum;
extern unsigned long* (*accessorsCpu[10])(CpuUsageStats*);

void* analyzerThread(void* arg)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 300000;    //300 ms
    //wait some time to get threadsNum set
    nanosleep(&ts, &ts);   
    printf("%d\n", threadsNum); 
    
    CpuUsageStats* tmpCpuStats;
    CpuUsageStats* cpuStats = malloc(sizeof(CpuUsageStats) * (threadsNum + 1));
    CpuUsageStats* prevCpuStats = malloc(sizeof(CpuUsageStats) * (threadsNum + 1));

    // loop over all prevCpuStats member to init values (first CPU usage is irrelevant)
    for(size_t i = 0; i < threadsNum; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            *(accessorsCpu[j](&prevCpuStats[i])) = 1;
        }
    }

    for(int i = 0; i < 100; i++)
    {
        pthread_mutex_lock(&queueMutex);

        while((tmpCpuStats = dequeue()) == NULL)
        {
            pthread_cond_wait(&condQueue, &queueMutex);
        }
        if(tmpCpuStats != NULL)
         cpuStats[0] = *tmpCpuStats;
      
        int cnt = 1;
        while((tmpCpuStats = dequeue()) != NULL)
        {
            cpuStats[cnt++] = *tmpCpuStats;
        }


        pthread_mutex_unlock(&queueMutex);

        // printf("Cpu val: %s - %lu\n", cpuStats[0].t_cpuName, cpuStats[0].t_user);
        // printf("Prev val: %s - %lu\n", prevCpuStats[0].t_cpuName, prevCpuStats[0].t_user);
        // printf("user\tnice\tsystem\tidle\tiowait\tirq\tsoftirq\tsteal\tguest\tguest_nice\n");
        // printf("%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\n", cpuStats[0].t_user, cpuStats[0].t_nice,
        //         cpuStats[0].t_system, cpuStats[0].t_idle, cpuStats[0].t_iowait, cpuStats[0].t_irq,
        //         cpuStats[0].t_softirq, cpuStats[0].t_steal, cpuStats[0].t_guest, cpuStats[0].t_guestNice);

        // printf("%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\n", prevCpuStats[0].t_user, prevCpuStats[0].t_nice,
        //         prevCpuStats[0].t_system, prevCpuStats[0].t_idle, prevCpuStats[0].t_iowait, prevCpuStats[0].t_irq,
        //         prevCpuStats[0].t_softirq, prevCpuStats[0].t_steal, prevCpuStats[0].t_guest, prevCpuStats[0].t_guestNice);

        // now after reading the CPU usage we can process these values
        for(size_t i = 0; i < threadsNum + 1; i++)
        {
            unsigned long PrevIdle = prevCpuStats[i].t_idle + prevCpuStats[i].t_iowait;
            unsigned long Idle = cpuStats[i].t_idle + cpuStats[i].t_iowait;
            unsigned long PrevNonIdle = prevCpuStats[i].t_user + prevCpuStats[i].t_nice + prevCpuStats[i].t_system + 
                                        prevCpuStats[i].t_irq + prevCpuStats[i].t_softirq + prevCpuStats[i].t_steal;
            unsigned long NonIdle = cpuStats[i].t_user + cpuStats[i].t_nice + cpuStats[i].t_system + 
                                    cpuStats[i].t_irq + cpuStats[i].t_softirq + cpuStats[i].t_steal;
            unsigned long PrevTotal = PrevIdle + PrevNonIdle;
            unsigned long Total = Idle + NonIdle;
            unsigned long totald = Total - PrevTotal;
            unsigned long idled = Idle - PrevIdle;
            double CPU_Percentage = (double)(totald - idled)/totald;
        }

        // printf("PrevIde/Idle: %lu/%lu\n", PrevIdle, Idle);
        // printf("PrevNonIdle/NonIdle: %lu/%lu\n", PrevNonIdle, NonIdle);
        // printf("PrevTotal/Total: %lu/%lu\n", PrevTotal, Total);
        // printf("totald/idled: %lu/%lu\n", totald, idled);
        // printf("CPU usage: %f\n", CPU_Percentage);

        memcpy(prevCpuStats, cpuStats, sizeof(CpuUsageStats) * (threadsNum + 1));
    }
    free(cpuStats);
    free(prevCpuStats);
    return NULL;
}