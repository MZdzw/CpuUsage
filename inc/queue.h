#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include <pthread.h>

#define OK 0
#define FULL_ERROR -1

#define MAX_CAPACITY 20

typedef struct
{
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