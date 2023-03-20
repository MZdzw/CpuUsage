#include <stdio.h>
#include <pthread.h>
#include "readerThr.h"
#include "analyzerThr.h"

pthread_mutex_t queueMutex;
pthread_cond_t condQueue;

int main(void)
{
    pthread_t reader_T;
    pthread_t analyzer_T;
    pthread_mutex_init(&queueMutex, NULL);
    pthread_cond_init(&condQueue, NULL);

    pthread_create(&reader_T, NULL, &readerThread, NULL);
    pthread_create(&analyzer_T, NULL, &analyzerThread, NULL);

    pthread_join(reader_T, NULL);
    pthread_join(analyzer_T, NULL);

    pthread_mutex_destroy(&queueMutex);
    pthread_cond_destroy(&condQueue);

    return 0;
}