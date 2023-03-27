#include <stdio.h>
#include <unistd.h>
#include <stdatomic.h>
#include "watchdogThr.h"
#include "queue.h"

extern atomic_bool readerToClose;
extern atomic_bool analyzerToClose;
extern atomic_bool printerToClose;

atomic_bool readerCheckPoint;
atomic_bool analyzerCheckPoint;
atomic_bool printerCheckPoint;

void* watchdogThread(void* arg)
{
    (void)arg;     //to get rid of warning
    atomic_init(&readerCheckPoint, false);
    atomic_init(&analyzerCheckPoint, false);
    atomic_init(&printerCheckPoint, false);
    printf("Watchdog: %d\n", atomic_load(&readerCheckPoint));
    for(int i = 0; i < 10; i++)
    {
        atomic_store(&readerCheckPoint, false);
        atomic_store(&analyzerCheckPoint, false);
        atomic_store(&printerCheckPoint, false);
        sleep(2);
        printf("Watchdog readerCheckPoint: %d\n", atomic_load(&readerCheckPoint));
        printf("Watchdog analyzerCheckPoint: %d\n", atomic_load(&analyzerCheckPoint));
        printf("Watchdog printerCheckPoint: %d\n", atomic_load(&printerCheckPoint));
        if((!atomic_load(&readerCheckPoint)) || (!atomic_load(&analyzerCheckPoint)) || 
            (!atomic_load(&printerCheckPoint)))
        {
            //close all threads
            atomic_store(&readerToClose, true);
            atomic_store(&analyzerToClose, true);
            atomic_store(&printerToClose, true);

            //in case of analyzer and printer threads you need to send signal
            printf("Watchdog broadcast end\n");
            pthread_cond_broadcast(&condCpuStatsQueue);
            pthread_cond_broadcast(&condCpuStatsPrinterQueue);

            return NULL;         
        }
    }
    return NULL;
}
