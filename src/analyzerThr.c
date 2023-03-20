#include <stdio.h>
#include <unistd.h>
#include "analyzerThr.h"
#include "queue.h"

extern unsigned int threadsNum;

void* analyzerThread(void* arg)
{
    CpuUsageStats* cpuStats;

    for(int i = 0; i < 100; i++)
    {
        pthread_mutex_lock(&queueMutex);
        while((cpuStats = dequeue()) == NULL)
        {
            pthread_cond_wait(&condQueue, &queueMutex);
        }
        while((cpuStats = dequeue()) != NULL)
            printf("Analyzer queue: %s\n", cpuStats->t_cpuName);

        pthread_mutex_unlock(&queueMutex);
    }
    return NULL;
}