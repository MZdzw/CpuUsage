#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include <pthread.h>

#define OK 0
#define FULL_ERROR -1

#define MAX_CAPACITY 40

typedef struct
{
    char t_cpuName[6];          // cpuxx\0
    unsigned long t_user;
    unsigned long t_nice;
    unsigned long t_system;
    unsigned long t_idle;
    unsigned long t_iowait;
    unsigned long t_irq;
    unsigned long t_softirq;
    unsigned long t_steal;
    unsigned long t_guest;
    unsigned long t_guestNice;
} CpuUsageStats;

typedef struct 
{
    char t_cpuName[6];
    double t_cpuUsagePercentage;
} CpuUsageStatsPrint;


//variable initialized in main function
extern pthread_mutex_t queueCpuStatsMutex;     //for accessing queue with CpuStats (analyzer-reader)
extern pthread_mutex_t queueCpuStatsPrinterMutex;     //for accessing queue for printing

extern pthread_cond_t condCpuStatsQueue;
extern pthread_cond_t condCpuStatsPrinterQueue;

extern pthread_barrier_t barrier;

struct QueueCpuStats
{
    CpuUsageStats cpuValues[MAX_CAPACITY];
    size_t front;       //index for dequeue
    size_t rear;        //index for push
    size_t size;        //current size of queue
};

// functions for processing the queue

bool isFull_CpuStats();
bool isEmpty_CpuStats();
int push_CpuStats(CpuUsageStats cpuStats);
CpuUsageStats* dequeue_CpuStats();
CpuUsageStats* front_CpuStats();
CpuUsageStats* rear_CpuStats();

void printQueue();

//queue for printing purposes
struct QueueCpuStatsPrint
{
    CpuUsageStatsPrint cpuValues[MAX_CAPACITY];
    size_t front;       //index for dequeue
    size_t rear;        //index for push
    size_t size;        //current size of queue
};

// functions for processing the printing queue

bool isFull_CpuStatsPrint();
bool isEmpty_CpuStatsPrint();
int push_CpuStatsPrint(CpuUsageStatsPrint cpuStatsPrint);
CpuUsageStatsPrint* dequeue_CpuStatsPrint();
CpuUsageStatsPrint* front_CpuStatsPrint();
CpuUsageStatsPrint* rear_CpuStatsPrint();



// add accessors to CpuUsageStats members
// This method allows to be independent of the memory layout
unsigned long* getUser(CpuUsageStats *stats);
unsigned long* getNice(CpuUsageStats *stats);
unsigned long* getSystem(CpuUsageStats *stats);
unsigned long* getIdle(CpuUsageStats *stats);
unsigned long* getIowait(CpuUsageStats *stats);
unsigned long* getIrq(CpuUsageStats *stats);
unsigned long* getSoftirq(CpuUsageStats *stats);
unsigned long* getSteal(CpuUsageStats *stats);
unsigned long* getGuest(CpuUsageStats *stats);
unsigned long* getGuestNice(CpuUsageStats *stats);


#endif