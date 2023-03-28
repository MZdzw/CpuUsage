#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "readerThr.h"
#include "queue.h"

#define MAX 100

// extern struct QueueCpuStats cpuStatsQueue;
// extern unsigned long* (*accessorsCpu[10])(CpuUsageStats*);


static char buf[MAX];
static CpuUsageStats cpuStats;

unsigned int threadsNum;
atomic_bool readerToClose;


void* readerThread(void* arg)
{
    (void)arg;     //to get rid of warning
    atomic_store(&readerToClose, false);
    printf("First reader\n");
    // pthread_mutex_lock(&queueCpuStatsMutex);
    if(getCpuThreadsNum("/proc/stat") != OK)
        return NULL;
    printf("Reader broadcast\n");
    // pthread_cond_broadcast(&condCpuStatsQueue);
    // pthread_mutex_unlock(&queueCpuStatsMutex);
    printf("Before barrier waiting (reader)\n");
    pthread_barrier_wait(&barrier);

    for(int i = 0; i < 20; i++)
    {
        sleep(1);       //cause thread to go off
        pthread_mutex_lock(&queueCpuStatsMutex);
        
        //alternative below (need timespec struct as param)
        //pthread_delay_np(&ts);
        printf("Reader Thread inside %d\n", i);
        if(getCpuStatsFromFile("/proc/stat") != OK)
            return NULL;

        //printQueue();
        pthread_cond_signal(&condCpuStatsQueue);
        pthread_mutex_unlock(&queueCpuStatsMutex);

        atomic_store(&readerCheckPoint, true);
        if(atomic_load(&readerToClose))
        {
            return NULL;
        }
    }

    return NULL;
}

int getCpuStatsFromFile(char* fileName)
{
    FILE* ptrStatsFile = fopen(fileName, "r");
    if(ptrStatsFile == NULL)
    {
        perror("Could not open stat file");
        fclose(ptrStatsFile);
        return FILE_OPEN_ERROR;      //return error
    }

    char* ptrStringSplit;
    char* ptrTmp;
    for(size_t i = 0; i < threadsNum + 1; i++)
    {
        if(fgets(buf, MAX, ptrStatsFile) == NULL)
        {
            fclose(ptrStatsFile);
            return FILE_READ_ERROR;
        }

        ptrStringSplit = strtok(buf, " ");
        memcpy(cpuStats.t_cpuName, ptrStringSplit, sizeof(cpuStats.t_cpuName));

        ptrStringSplit = strtok(NULL, " ");
        size_t cnt = 0;
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
        //we got CPU usage in cpuStats variable
        push_CpuStats(cpuStats);
    }

    fclose(ptrStatsFile);
    return OK;
}

int getCpuThreadsNum(char* fileName)
{
    threadsNum = 0;
    FILE* ptrStatsFile = fopen(fileName, "r");
    if(ptrStatsFile == NULL)
    {
        perror("Could not open stat file");
        //fclose(ptrStatsFile);      //you can't close if you don't open a file
        return FILE_OPEN_ERROR;      //return error
    }

    if(fgets(buf, MAX, ptrStatsFile) == NULL)   //read one line of file
    {
        fclose(ptrStatsFile);
        return FILE_READ_ERROR;
    }

    char* ptrStringSplit = strtok(buf, " ");   //"CPU \0 92324..."
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
        threadsNum--;           //don't count "cpu"

    if(threadsNum <= 0)
    {
        fclose(ptrStatsFile);
        return READ_NO_CPU_THR;
    }

    fclose(ptrStatsFile);
    return OK;              //don't count "cpu"
}
