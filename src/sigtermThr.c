#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdatomic.h>
#include "sigtermThr.h"
#include "queue.h"
#include "analyzerThr.h"
#include "printerThr.h"
#include "readerThr.h"
#include "watchdogThr.h"

static volatile sig_atomic_t done = 0;
 
void term(int signum)
{
    (void)signum;
    done = 1;
}

void* sigtermThread(void* arg)
{
    (void)arg;     //to get rid of warning
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
 
    int loop = 0;
    while (!done)
    {
        unsigned int t = sleep(3);
        /* sleep returns the number of seconds left if
         * interrupted */
        while (t > 0)
        {
            printf("Loop run was interrupted with %d "
                   "sec to go, finishing...\n", t);
            t = sleep(t);
        }
        printf("Finished loop run %d.\n", loop++);
    }
    // there you can terminate all threads

    //close all threads
    atomic_store(&readerToClose, true);
    atomic_store(&analyzerToClose, true);
    atomic_store(&printerToClose, true);
    atomic_store(&watchdogToClose, true);

    //in case of analyzer and printer threads you need to send signal
    printf("Sigterm broadcast end\n");
    pthread_cond_broadcast(&condCpuStatsQueue);
    pthread_cond_broadcast(&condCpuStatsPrinterQueue);       
 
    printf("done.\n");
    return NULL;
}
