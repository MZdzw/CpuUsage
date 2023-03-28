#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdatomic.h>
#include "readerThr.h"
#include "analyzerThr.h"
#include "queue.h"

atomic_bool analyzerCheckPoint;
atomic_bool analyzerToClose;

void* analyzerThread(void* arg)
{
    (void)arg;     //to get rid of warning
    atomic_init(&analyzerCheckPoint, false);
    atomic_init(&analyzerToClose, false);

    pthread_barrier_wait(&barrier);
    
    CpuUsageStats* tmpCpuStats;
    //dynamically allocate memory for cpu stats, because we don't know how many cpu threads can have user
    CpuUsageStats* cpuStats = malloc(sizeof(CpuUsageStats) * (threadsNum + 1) * 3);     //times 3 in case to buffer some data
    CpuUsageStats* prevCpuStats = malloc(sizeof(CpuUsageStats) * (threadsNum + 1) * 3);

    CpuUsageStatsPrint* cpuStatsPrint = malloc(sizeof(CpuUsageStatsPrint) * (threadsNum + 1));

    // loop over all prevCpuStats member to init values (first CPU usage is irrelevant)
    for(size_t i = 0; i < threadsNum + 1; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            *(accessorsCpu[j](&prevCpuStats[i])) = 1;
        }
    }

    for(;;)
    {
        pthread_mutex_lock(&queueCpuStatsMutex);

        while((tmpCpuStats = dequeue_CpuStats()) == NULL)
        {
            // here wait for signal from reader thread (or from watchdog)
            pthread_cond_wait(&condCpuStatsQueue, &queueCpuStatsMutex);
            // if signal came from watchdog that means it has to terminate itself
            if(atomic_load(&analyzerToClose))
            {
                free(cpuStats);
                free(prevCpuStats);
                free(cpuStatsPrint);
                //remember to unlock mutexes
                pthread_mutex_unlock(&queueCpuStatsMutex);

                return NULL;
            }
        }
        if(tmpCpuStats != NULL)
            cpuStats[0] = *tmpCpuStats;
      
        size_t cnt = 1;
        // read all cpu stats from queue
        while((tmpCpuStats = dequeue_CpuStats()) != NULL)
        {
            cpuStats[cnt++] = *tmpCpuStats;
        }


        pthread_mutex_unlock(&queueCpuStatsMutex);

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
            cpuStatsPrint[i].t_cpuUsagePercentage = (double)(totald - idled)/totald;

            strcpy(cpuStatsPrint[i].t_cpuName, cpuStats[i].t_cpuName);
        }

        // get mutex lock and push values to queue for printing
        pthread_mutex_lock(&queueCpuStatsPrinterMutex);
        for(size_t i = 0; i < threadsNum + 1; i++)
        {
            push_CpuStatsPrint(cpuStatsPrint[i]);
        }
        pthread_cond_signal(&condCpuStatsPrinterQueue);
        pthread_mutex_unlock(&queueCpuStatsPrinterMutex);

        // copy current values to previous variable
        memcpy(prevCpuStats, cpuStats, sizeof(CpuUsageStats) * (threadsNum + 1));

        // mark itself to watchdog of being alive
        atomic_store(&analyzerCheckPoint, true);
    }
    // free(cpuStats);
    // free(prevCpuStats);
    // free(cpuStatsPrint);
    // return NULL;
}
