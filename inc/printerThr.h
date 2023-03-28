#ifndef PRINTERTHR_H
#define PRINTERTHR_H
#include <stdatomic.h>

extern atomic_bool printerToClose;
extern atomic_bool printerCheckPoint;

void* printerThread(void* arg);

#endif
