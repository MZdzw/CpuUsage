#include <stdio.h>
#include <stdlib.h>
#include "printerThr.h"
#include "queue.h"

#define gotoxy(x,y) printf("\033[%u;%uH", (x), (y))

extern unsigned int threadsNum;

static void printCpuUsage(CpuUsageStatsPrint* cpuUsage);

void* printerThread(void* arg)
{
    // struct timespec ts;
    // ts.tv_sec = 0;
    // ts.tv_nsec = 300000;    //300 ms
    //wait some time to get threadsNum set

    // nanosleep(&ts, &ts);   
    // pthread_cond_wait(&condCpuStatsQueue, &queueCpuStatsMutex);
    // pthread_mutex_unlock(&queueCpuStatsMutex);
    printf("Before barrier waiting (printer)\n");
    pthread_barrier_wait(&barrier);
    printf("Threads num in printer (begining): %d\n", threadsNum); 

    CpuUsageStatsPrint* tmpCpuStatsPrint;
    CpuUsageStatsPrint* cpuStatsPrint = malloc(sizeof(CpuUsageStatsPrint) * (threadsNum + 1) * 3); //times 3 in case to buffer some data

    for(int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&queueCpuStatsPrinterMutex);
        printf("Printer thread inside: %d\n", i);

        while((tmpCpuStatsPrint = dequeue_CpuStatsPrint()) == NULL)
        {
            pthread_cond_wait(&condCpuStatsPrinterQueue, &queueCpuStatsPrinterMutex);
        }
        cpuStatsPrint[0] = *tmpCpuStatsPrint;

        unsigned int cnt = 1;
        while((tmpCpuStatsPrint = dequeue_CpuStatsPrint()) != NULL)
        {
            cpuStatsPrint[cnt++] = *tmpCpuStatsPrint;
        }

        pthread_mutex_unlock(&queueCpuStatsPrinterMutex);

        // now print the queue (CPU stats)
        printCpuUsage(cpuStatsPrint);
        // for(size_t i = 0; i < (threadsNum + 1); i++)
        // {
        //     printf("%s usage: %f\n", cpuStatsPrint[i].t_cpuName, cpuStatsPrint[i].t_cpuUsagePercentage);
        // }
    }
    free(cpuStatsPrint);
    return NULL;
}

static void printCpuUsage(CpuUsageStatsPrint* cpuUsage)
{
    unsigned int size;
    system("clear");
    for(unsigned int i = 0; i < (threadsNum + 1); i++)
    {
        gotoxy(i + 1, 1);
        size = cpuUsage[i].t_cpuUsagePercentage * 10;
        printf("%s", cpuUsage[i].t_cpuName);
        gotoxy(i + 1, 6);
        printf("usage:  ");
        gotoxy(i + 1, 13);
        printf("[");
        printf("          ");   //clear space between square brackets
        gotoxy(i + 1, 14);
        for(unsigned int j = 0; j < size; j++)
            printf("|");
        gotoxy(i + 1, 24);  
        printf("] - %.1f%%  ", cpuUsage[i].t_cpuUsagePercentage * 100);
    }
}