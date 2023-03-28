#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "readerThr.h"
#include "queue.h"

#define MAX 100

static char buf[MAX];
static CpuUsageStats cpuStats;

unsigned int threadsNum;

atomic_bool readerCheckPoint;
atomic_bool readerToClose;

void* readerThread(void* arg)
{
    (void)arg;     //to get rid of warning
    atomic_init(&readerCheckPoint, false);
    atomic_init(&readerToClose, false);

    if(getCpuThreadsNum("/proc/stat") != OK)
        return NULL;

    pthread_barrier_wait(&barrier);

    for(;;)
    {
        sleep(1);                                           //cause thread to go off
        pthread_mutex_lock(&queueCpuStatsMutex);
        
        if(getCpuStatsFromFile("/proc/stat") != OK)
            return NULL;

        // send signal that this thread has pushed values on queue (to analyzer thread)
        pthread_cond_signal(&condCpuStatsQueue);
        pthread_mutex_unlock(&queueCpuStatsMutex);

        // mark itself to watchdog of being alive
        atomic_store(&readerCheckPoint, true);
        // watchdog or sigterm decided to terminate thread
        if(atomic_load(&readerToClose))
        {
            return NULL;
        }
    }
    return NULL;
}

// function obtain cpu stats from /proc/stat file and push on queue
int getCpuStatsFromFile(char* fileName)
{
    FILE* ptrStatsFile = fopen(fileName, "r");
    if(ptrStatsFile == NULL)
    {
        perror("Could not open stat file");
        fclose(ptrStatsFile);
        return FILE_OPEN_ERROR;                     //return error
    }

    char* ptrStringSplit;
    char* ptrTmp;
    for(size_t i = 0; i < threadsNum + 1; i++)
    {
        // get one line
        if(fgets(buf, MAX, ptrStatsFile) == NULL)
        {
            fclose(ptrStatsFile);
            return FILE_READ_ERROR;
        }

        // obtained cpu name
        ptrStringSplit = strtok(buf, " ");
        memcpy(cpuStats.t_cpuName, ptrStringSplit, sizeof(cpuStats.t_cpuName));

        // go to value which follows cpu name
        ptrStringSplit = strtok(NULL, " ");
        size_t cnt = 0;
        // get the all 10 values and save them to cpuStats struct
        while(ptrStringSplit != NULL)
        {
            if(cnt < 10)
            {
                *(accessorsCpu[cnt++](&cpuStats)) = strtoul(ptrStringSplit, &ptrTmp, 10);
                ptrStringSplit = strtok(NULL, " ");
            }
            else
            {
                return INCORRECT_NUM_CPU_USAGE;
            }
        }
        //we got CPU usage in cpuStats struct variable - push on queue
        push_CpuStats(cpuStats);
    }
    fclose(ptrStatsFile);
    return OK;
}

// function count threads of cpu (the number depends on device)
int getCpuThreadsNum(char* fileName)
{
    threadsNum = 0;
    FILE* ptrStatsFile = fopen(fileName, "r");
    if(ptrStatsFile == NULL)
    {
        perror("Could not open stat file");
        //fclose(ptrStatsFile);                     //you can't close if you don't open a file
        return FILE_OPEN_ERROR;                     //return error
    }

    if(fgets(buf, MAX, ptrStatsFile) == NULL)       //read one line of file
    {
        fclose(ptrStatsFile);
        return FILE_READ_ERROR;
    }

    char* ptrStringSplit = strtok(buf, " ");        //"CPU \0 92324..."
    while(memcmp(ptrStringSplit, "cpu", 3) == 0)
    {
        threadsNum++;
        if(fgets(buf, MAX, ptrStatsFile) == NULL)   //read next line of file
        {
            fclose(ptrStatsFile);
            return FILE_READ_ERROR;
        }

        ptrStringSplit = strtok(buf, " ");          //get first word from line
    }
    if(threadsNum != 0)
        threadsNum--;                               //don't count "cpu"

    if(threadsNum == 0)
    {
        fclose(ptrStatsFile);
        return READ_NO_CPU_THR;
    }

    fclose(ptrStatsFile);
    return OK;
}
