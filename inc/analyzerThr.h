#ifndef ANALYZERTHR_H
#define ANALYZERTHR_H
#include <stdatomic.h>

extern atomic_bool analyzerToClose;
extern atomic_bool analyzerCheckPoint;

void* analyzerThread(void* arg);

#endif
