#include <stdio.h>
#include <unistd.h>
#include "readerThr.h"

void* readerThread(void* arg)
{

    for(int i = 0; i < 10; i++)
    {
        sleep(1);
        printf("Reader Thread\n");
    }
    return NULL;
}