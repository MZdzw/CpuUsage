#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "readerThr.h"

#define MAX 80

static int getCpuStatsFromFile();

static char buf[MAX];
struct CpuUsageStats cpuStats;
unsigned long* ptrCpyStatsLookUpTable[10] = {
 &(cpuStats.t_user), &(cpuStats.t_nice), &(cpuStats.t_system), &(cpuStats.t_idle),
 &(cpuStats.t_iowait), &(cpuStats.t_irq), &(cpuStats.t_softirq), 
 &(cpuStats.t_steal), &(cpuStats.t_guest), &(cpuStats.t_guestNice)
 };


void* readerThread(void* arg)
{

    for(int i = 0; i < 20; i++)
    {
        sleep(1);
        printf("Reader Thread\n");

        if(getCpuStatsFromFile() != OK)
            return NULL;
    }

    return NULL;
}

static int getCpuStatsFromFile()
{
    FILE* ptrStatsFile = fopen("/proc/stat", "r");   //open in read-only mode stat file
    if(ptrStatsFile == NULL)
    {
        perror("Could not open stat file");
        fclose(ptrStatsFile);
        return FILE_OPEN_ERROR;      //return error
    }
    if(fgets(buf, MAX, ptrStatsFile) == NULL)
    {
        fclose(ptrStatsFile);
        return FILE_READ_ERROR;
    }

    char* ptrStringSplit = strtok(buf + 3, " ");   //"CPU \0 92324..."
                                        // 012 3  456
    
    char* ptrTmp;
    size_t cnt = 0;
    while(ptrStringSplit != NULL)
	{
        if(cnt < 10)
        {
            *ptrCpyStatsLookUpTable[cnt++] = strtoul(ptrStringSplit, &ptrTmp, 10);
            ptrStringSplit = strtok(NULL, " ");
        }
	}
    printf("%lu\n", cpuStats.t_user);

    fclose(ptrStatsFile);
    return OK;
}