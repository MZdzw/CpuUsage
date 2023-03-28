#ifndef WATCHDOGTHR_H
#define WATCHDOGTHR_H
#include <stdbool.h>

// extern atomic_bool readerCheckPoint;
// extern atomic_bool analyzerCheckPoint;
// extern atomic_bool printerCheckPoint;

extern atomic_bool watchdogToClose;

void* watchdogThread(void* arg);

#endif
