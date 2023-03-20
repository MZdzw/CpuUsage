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

//variable initialized in main function
extern pthread_mutex_t queueMutex;     //for accessing queue
extern pthread_cond_t condQueue;

struct Queue
{
    CpuUsageStats cpuValues[MAX_CAPACITY];
    size_t front;       //index for dequeue
    size_t rear;        //index for push
    size_t size;        //current size of queue
};

// functions for processing the queue

bool isFull();
bool isEmpty();
int push(CpuUsageStats cpuStats);
CpuUsageStats* dequeue();
CpuUsageStats* front();
CpuUsageStats* rear();

void printQueue();

#endif