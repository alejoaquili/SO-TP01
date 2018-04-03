#ifndef VIEW_PROCESS_H
#define VIEW_PROCESS_H

#include "sharedMemory.h"

void printTheHash(char * buffer, int readBytes);

int readTheNextHash(sharedMemoryADT shm, char * buffer, fd_set rfd);

#endif

