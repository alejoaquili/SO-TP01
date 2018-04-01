#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <mqueue.h>

typedef struct sharedMemoryCDT* sharedMemoryADT;

sharedMemoryADT sharedMemoryCreator(const int id, const long memSize, 
															 const long flags);

void deleteShMem(sharedMemoryADT shm);

int getId(sharedMemoryADT shm);

int getFd(sharedMemoryADT shm);


sharedMemoryADT openShMem(const int id, const long flags);

void closeShMem(sharedMemoryADT shm);

ssize_t writeShMem(sharedMemoryADT shm, const void* buffer , size_t nbytes);

ssize_t readShMem(sharedMemoryADT shm, void* buffer , size_t nbytes);

#endif

