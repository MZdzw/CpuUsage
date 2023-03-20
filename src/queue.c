#include <stdio.h>
#include "queue.h"


struct Queue cpuStatsQueue = {.front = 0, .rear = 0, .size = 0};


bool isFull()
{
    return (cpuStatsQueue.size == MAX_CAPACITY);
}

bool isEmpty()
{
    return (cpuStatsQueue.size == 0);
}

int push(CpuUsageStats cpuStats)
{
    if(isFull())
        return FULL_ERROR;
    cpuStatsQueue.cpuValues[cpuStatsQueue.rear] = cpuStats;
    cpuStatsQueue.rear = (cpuStatsQueue.rear + 1) % MAX_CAPACITY;
    cpuStatsQueue.size++;
    return OK;
}

CpuUsageStats* dequeue()
{
    if(isEmpty())
        return NULL;
    CpuUsageStats* cpuStatsDequeued = &(cpuStatsQueue.cpuValues[cpuStatsQueue.front]);
    cpuStatsQueue.front = (cpuStatsQueue.front + 1) % MAX_CAPACITY;
    cpuStatsQueue.size--;
    return cpuStatsDequeued;
}

CpuUsageStats* front()
{
    if(isEmpty())
        return NULL;
    return &(cpuStatsQueue.cpuValues[cpuStatsQueue.front]);
}

CpuUsageStats* rear()
{
    if(isEmpty())
        return NULL;
    return &(cpuStatsQueue.cpuValues[cpuStatsQueue.rear]);
}

void printQueue()
{
    for(size_t i = 0; i < cpuStatsQueue.size; i++)
    {
        int index = (cpuStatsQueue.front + i) % MAX_CAPACITY;
        printf("%s\n", cpuStatsQueue.cpuValues[index].t_cpuName);
    }
}