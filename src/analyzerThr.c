#include <stdio.h>
#include <unistd.h>
#include "analyzerThr.h"

void* analyzerThread(void* arg)
{

    for(int i = 0; i < 10; i++)
    {
        sleep(2);
        printf("Analyzer Thread\n");
    }
    return NULL;
}