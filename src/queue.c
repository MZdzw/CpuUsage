#include <stdio.h>
#include "queue.h"


struct Queue cpuStatsQueue = {.front = 0, .rear = 0, .size = 0};
// define an array of function pointers to the accessor functions
unsigned long* (*accessorsCpu[10])(CpuUsageStats*) = {
    &getUser,
    &getNice,
    &getSystem,
    &getIdle,
    &getIowait,
    &getIrq,
    &getSoftirq,
    &getSteal,
    &getGuest,
    &getGuestNice
};


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

unsigned long* getUser(CpuUsageStats *stats) {
    return &(stats->t_user);
}

unsigned long* getNice(CpuUsageStats *stats) {
    return &(stats->t_nice);
}

unsigned long* getSystem(CpuUsageStats *stats) {
    return &(stats->t_system);
}

unsigned long* getIdle(CpuUsageStats *stats) {
    return &(stats->t_idle);
}

unsigned long* getIowait(CpuUsageStats *stats) {
    return &(stats->t_iowait);
}

unsigned long* getIrq(CpuUsageStats *stats) {
    return &(stats->t_irq);
}

unsigned long* getSoftirq(CpuUsageStats *stats) {
    return &(stats->t_softirq);
}

unsigned long* getSteal(CpuUsageStats *stats) {
    return &(stats->t_steal);
}

unsigned long* getGuest(CpuUsageStats *stats) {
    return &(stats->t_guest);
}

unsigned long* getGuestNice(CpuUsageStats *stats) {
    return &(stats->t_guestNice);
}