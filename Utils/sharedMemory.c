#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "processlib.h"
#include <fcntl.h>
#include <semaphore.h>
#include "errorslib.h"
#include "sharedMemory.h"

#include <errno.h>

#define MAX_NAME 14

typedef struct sharedMemoryCDT {
    sem_t * semaphore;
    int fd;
    int id;
    int memSize;
    char * semName;
    char * shmName;
} sharedMemoryCDT;

sem_t * createSemaphore(sharedMemoryADT shm);
sem_t * openSemaphore(sharedMemoryADT shm);


sharedMemoryADT sharedMemoryCreator(const int id, const long memSize, 
                                                              const long flags)
{   
    sharedMemoryADT shm = malloc(sizeof(sharedMemoryCDT));
    shm->id = id;
    shm->memSize = memSize;
    shm->shmName = calloc(MAX_NAME, sizeof(char));
    sprintf(shm->shmName, "/shm%d", id);
    
    shm->semaphore = createSemaphore(shm);
    checkIsNotNull(shm->semaphore, "sem_open() Failed");

    shm->fd = shm_open(shm->shmName, flags | O_CREAT, 0777);
    checkFail(shm->fd, "shm_open() Failed");
    return shm;
}

int getShMemId(sharedMemoryADT shm)
{
    checkIsNotNull(shm, "Null sharedMemoryADT pointer");
    return shm->id;
}

int getShMemFd(sharedMemoryADT shm)
{
    return shm->fd;
}

void deleteShMem(sharedMemoryADT shm)
{
    shm_unlink(shm->shmName);
    sem_unlink(shm->semName);
    freeSpace(3, shm->shmName, shm->semName, shm);
}

sharedMemoryADT openShMem(const int id, const long flags) 
{
    sharedMemoryADT shm = malloc(sizeof(sharedMemoryCDT));
    shm->id = id;
    shm->shmName = calloc(MAX_NAME, sizeof(char));
    sprintf(shm->shmName, "/shm%d", id);
    
    shm->semaphore = openSemaphore(shm);
    checkIsNotNull(shm->semaphore, "sem_open() Failed");
    shm->fd = shm_open(shm->shmName, flags, 0777);
    checkFail(shm->fd, "shm_open() Failed");
    return shm;
}

void closeShMem(sharedMemoryADT shm)
{
    freeSpace(3, shm->shmName, shm->semName, shm);
}

sem_t* createSemaphore(sharedMemoryADT shm)
{
    shm->semName = calloc(MAX_NAME, sizeof(char));
    sprintf(shm->semName, "/sem%d", shm->id);
    sem_unlink(shm->semName);
    return sem_open(shm->semName, O_CREAT|O_EXCL, 0777, 1);
}

sem_t* openSemaphore(sharedMemoryADT shm)
{
    shm->semName = calloc(MAX_NAME, sizeof(char));
    sprintf(shm->semName, "/sem%d", shm->id);
    return sem_open(shm->semName, O_RDWR);
}

ssize_t writeShMem(sharedMemoryADT shm, const void * buffer , size_t nbytes)
{
    sem_wait(shm->semaphore);
    ssize_t result = write(shm->fd, buffer, nbytes);
    sem_post(shm->semaphore);
    return result;
}

ssize_t readShMem(sharedMemoryADT shm, void * buffer , size_t nbytes)
{
    sem_wait(shm->semaphore);
    ssize_t result = read(shm->fd, buffer, nbytes);
    sem_post(shm->semaphore);
    return result;
}

