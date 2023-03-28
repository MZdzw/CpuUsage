#ifndef READERTHR_H
#define READERTHR_H
#include <stdatomic.h>

#define FILE_OPEN_ERROR -1
#define FILE_READ_ERROR -2
#define INCORRECT_NUM_CPU_USAGE -3
#define READ_NO_CPU_THR -4

extern unsigned int threadsNum;
extern atomic_bool readerToClose;
extern atomic_bool readerCheckPoint;

void* readerThread(void* arg);

int getCpuStatsFromFile(char* fileName);
int getCpuThreadsNum(char* fileName);

#endif
