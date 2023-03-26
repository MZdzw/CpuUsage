#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "readerThr.h"
#include "analyzerThr.h"
#include "printerThr.h"

pthread_mutex_t queueCpuStatsMutex;
pthread_mutex_t queueCpuStatsPrinterMutex;

pthread_cond_t condCpuStatsQueue;
pthread_cond_t condCpuStatsPrinterQueue;

pthread_barrier_t barrier;

int main(void)
{
    system("clear");
    pthread_t reader_T;
    pthread_t analyzer_T;
    pthread_t printer_T;

    pthread_mutex_init(&queueCpuStatsMutex, NULL);
    pthread_mutex_init(&queueCpuStatsPrinterMutex, NULL);

    pthread_cond_init(&condCpuStatsQueue, NULL);
    pthread_cond_init(&condCpuStatsPrinterQueue, NULL);

    pthread_barrier_init(&barrier, NULL, 3);

    pthread_create(&reader_T, NULL, &readerThread, NULL);
    pthread_create(&analyzer_T, NULL, &analyzerThread, NULL);
    pthread_create(&printer_T, NULL, &printerThread, NULL);

    pthread_join(reader_T, NULL);
    pthread_join(analyzer_T, NULL);
    pthread_join(printer_T, NULL);

    pthread_mutex_destroy(&queueCpuStatsMutex);
    pthread_mutex_destroy(&queueCpuStatsMutex);

    pthread_cond_destroy(&condCpuStatsQueue);
    pthread_cond_destroy(&condCpuStatsPrinterQueue);

    pthread_barrier_destroy(&barrier);

    return 0;
}