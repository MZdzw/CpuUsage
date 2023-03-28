#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdatomic.h>
#include "readerThr.h"
#include "analyzerThr.h"
#include "queue.h"

atomic_bool analyzerToClose;

void* analyzerThread(void* arg)
{
    // struct timespec ts;
    // ts.tv_sec = 0;
    // ts.tv_nsec = 300000;    //300 ms
    //wait some time to get threadsNum set
    // nanosleep(&ts, &ts);   
    // pthread_cond_wait(&condCpuStatsQueue, &queueCpuStatsMutex);
    // pthread_mutex_unlock(&queueCpuStatsMutex);
    (void)arg;     //to get rid of warning
    atomic_store(&analyzerToClose, false);
    printf("Before barrier waiting (analyzer)\n");
    pthread_barrier_wait(&barrier);
    printf("Threads num in analyzer (begining): %d\n", threadsNum); 
    
    CpuUsageStats* tmpCpuStats;
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

    for(int i = 0; i < 20; i++)
    {
        pthread_mutex_lock(&queueCpuStatsMutex);
        printf("Analyzer thread inside %d\n", i);

        while((tmpCpuStats = dequeue_CpuStats()) == NULL)
        {
            pthread_cond_wait(&condCpuStatsQueue, &queueCpuStatsMutex);
            if(atomic_load(&analyzerToClose))
            {
                free(cpuStats);
                free(prevCpuStats);
                free(cpuStatsPrint);
                printf("Closing analyzer thread\n");
                //remember to unlock mutexes
                pthread_mutex_unlock(&queueCpuStatsMutex);

                return NULL;
            }
        }
        printf("Analyzer thread after condition %d\n", i);
        if(tmpCpuStats != NULL)
         cpuStats[0] = *tmpCpuStats;
      
        int cnt = 1;
        while((tmpCpuStats = dequeue_CpuStats()) != NULL)
        {
            cpuStats[cnt++] = *tmpCpuStats;
        }


        pthread_mutex_unlock(&queueCpuStatsMutex);

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
            cpuStatsPrint[i].t_cpuUsagePercentage = (double)(totald - idled)/totald;
            strcpy(cpuStatsPrint[i].t_cpuName, cpuStats[i].t_cpuName);

            //printf("%s usage: %f\n", cpuStatsPrint[i].t_cpuName, cpuStatsPrint[i].t_cpuUsagePercentage);
        }

        // get mutex lock and push values to queue for printing
        pthread_mutex_lock(&queueCpuStatsPrinterMutex);
        for(size_t i = 0; i < threadsNum + 1; i++)
        {
            push_CpuStatsPrint(cpuStatsPrint[i]);
        }
        pthread_cond_signal(&condCpuStatsPrinterQueue);
        pthread_mutex_unlock(&queueCpuStatsPrinterMutex);

        memcpy(prevCpuStats, cpuStats, sizeof(CpuUsageStats) * (threadsNum + 1));

        atomic_store(&analyzerCheckPoint, true);
    }
    free(cpuStats);
    free(prevCpuStats);
    free(cpuStatsPrint);
    return NULL;
}
