#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "readerThr.h"
#include "queue.h"

#define MAX 100

extern struct Queue cpuStatsQueue;

static int getCpuStatsFromFile();
static int getCpuThreadsNum();

static char buf[MAX];
unsigned int threadsNum;

CpuUsageStats cpuStats;
unsigned long* ptrCpyStatsLookUpTable[10] = {
 &(cpuStats.t_user), &(cpuStats.t_nice), &(cpuStats.t_system), &(cpuStats.t_idle), 
 &(cpuStats.t_iowait), &(cpuStats.t_irq), &(cpuStats.t_softirq), 
 &(cpuStats.t_steal), &(cpuStats.t_guest), &(cpuStats.t_guestNice)
 };


void* readerThread(void* arg)
{
    getCpuThreadsNum();
    for(int i = 0; i < 100; i++)
    {
        pthread_mutex_lock(&queueMutex);
        //alternative below (need timespec struct as param)
        //pthread_delay_np(&ts);
        printf("Reader Thread\n");
        if(getCpuStatsFromFile() != OK)
            return NULL;

        printQueue();
        pthread_cond_signal(&condQueue);
        pthread_mutex_unlock(&queueMutex);
        sleep(1);       //cause thread to go off
    }

    return NULL;
}

static int getCpuStatsFromFile()
{
    FILE* ptrStatsFile = fopen("/proc/stat", "r");
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
                *ptrCpyStatsLookUpTable[cnt++] = strtoul(ptrStringSplit, &ptrTmp, 10);
                ptrStringSplit = strtok(NULL, " ");
            }
            else
            {
                return INCORRECT_NUM_CPU_USAGE;
            }
        }
        //we got CPU usage in cpuStats variable
        push(cpuStats);
    }

    fclose(ptrStatsFile);
    return OK;
}

static int getCpuThreadsNum()
{
    FILE* ptrStatsFile = fopen("/proc/stat", "r");
    if(ptrStatsFile == NULL)
    {
        perror("Could not open stat file");
        fclose(ptrStatsFile);
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
    threadsNum--;           //don't count "cpu"

    fclose(ptrStatsFile);
    return OK;              //don't count "cpu"
}