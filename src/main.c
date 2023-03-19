#include <stdio.h>
#include <pthread.h>
#include "readerThr.h"
#include "analyzerThr.h"


int main(void)
{
    pthread_t reader_T;
    pthread_t analyzer_T;

    pthread_create(&reader_T, NULL, &readerThread, NULL);
    pthread_create(&analyzer_T, NULL, &analyzerThread, NULL);

    pthread_join(reader_T, NULL);
    pthread_join(analyzer_T, NULL);

    return 0;
}