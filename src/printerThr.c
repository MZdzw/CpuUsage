#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include "readerThr.h"
#include "printerThr.h"
#include "queue.h"

#define gotoxy(x,y) printf("\033[%u;%uH", (x), (y))

static void printCpuUsage(CpuUsageStatsPrint* cpuUsage);

atomic_bool printerCheckPoint;
atomic_bool printerToClose;

void* printerThread(void* arg)
{
    (void)arg;                                  //to get rid of warning
    atomic_init(&printerCheckPoint, false);
    atomic_init(&printerToClose, false);
    pthread_barrier_wait(&barrier);

    CpuUsageStatsPrint* tmpCpuStatsPrint;
    CpuUsageStatsPrint* cpuStatsPrint = malloc(sizeof(CpuUsageStatsPrint) * (threadsNum + 1) * 3); //times 3 in case to buffer some data

    for(;;)
    {
        pthread_mutex_lock(&queueCpuStatsPrinterMutex);
        while((tmpCpuStatsPrint = dequeue_CpuStatsPrint()) == NULL)
        {
            // here wait for signal from analyzer thread (or from watchdog)
            pthread_cond_wait(&condCpuStatsPrinterQueue, &queueCpuStatsPrinterMutex);
            // if signal came from watchdog that means it has to terminate itself
            if(atomic_load(&printerToClose))
            {
                free(cpuStatsPrint);
                pthread_mutex_unlock(&queueCpuStatsPrinterMutex);
                return NULL;
            }
        }
        cpuStatsPrint[0] = *tmpCpuStatsPrint;

        size_t cnt = 1;
        // read all values from queue
        while((tmpCpuStatsPrint = dequeue_CpuStatsPrint()) != NULL)
        {
            cpuStatsPrint[cnt++] = *tmpCpuStatsPrint;
        }
        pthread_mutex_unlock(&queueCpuStatsPrinterMutex);

        // now print the queue (CPU stats)
        printCpuUsage(cpuStatsPrint);

        // mark itself to watchdog of being alive
        atomic_store(&printerCheckPoint, true);
    }
    // free(cpuStatsPrint);
    // return NULL;
}

// function print CPU usage to console (in formatted style)
static void printCpuUsage(CpuUsageStatsPrint* cpuUsage)
{
    unsigned int size;
    for(unsigned int i = 0; i < (threadsNum + 1); i++)
    {
        gotoxy(i + 1, 1);
        size = (unsigned int)(cpuUsage[i].t_cpuUsagePercentage * 10);
        if(i == 0)
            printf("%s  ", cpuUsage[i].t_cpuName);
        else
            printf("%s ", cpuUsage[i].t_cpuName);
        printf("usage: ");
        printf("[");
        printf("          ");   //clear space between square brackets
        gotoxy(i + 1, 14);
        for(unsigned int j = 0; j < size; j++)
            printf("|");
        gotoxy(i + 1, 24);  
        printf("] - %.1f%%  ", (double)(cpuUsage[i].t_cpuUsagePercentage) * 100);
    }
    printf("\n");
}
