#ifndef READERTHR_H
#define READERTHR_H

#define FILE_OPEN_ERROR -1
#define FILE_READ_ERROR -2
#define INCORRECT_NUM_CPU_USAGE -3
#define READ_NO_CPU_THR -4

void* readerThread(void* arg);

int getCpuStatsFromFile(char* fileName);
int getCpuThreadsNum(char* fileName);

#endif
