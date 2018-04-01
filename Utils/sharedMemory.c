#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include "errorslib.h"
#include "sharedMemory.h"

#define MAX_NAME 14

typedef struct sharedMemoryCDT {
    sem_t* semaphore;
    void* pointer;
    int fd;
    int id;
    int memSize;
    char* semName;
    char* shmName;
} sharedMemoryCDT;

sem_t* createSemaphore(sharedMemoryADT shm);
sem_t* openSemaphore(sharedMemoryADT shm);


sharedMemoryADT sharedMemoryCreator(const int id, const long memSize, const long flags)
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
    shm->pointer = mmap(NULL, memSize, PROT_READ, MAP_SHARED, shm->fd, (off_t)0);
    checkIsNotNull(shm->pointer," Null shmPointer");
    if(shm->pointer == MAP_FAILED)
        fail("mmap() Failed");
    return shm;
}

int getId(sharedMemoryADT shm)
{
    checkIsNotNull(shm, "Null sharedMemoryADT pointer");
    return shm->id;
}

int getFd(sharedMemoryADT shm)
{
    return shm->fd;
}


void deleteShMem(sharedMemoryADT shm)
{
    checkIsNotNull(shm->pointer, "Null shm pointer");
    munmap(shm->pointer, shm->memSize);
    shm_unlink(shm->shmName);
    sem_unlink(shm->semName);
    free(shm->shmName);
    free(shm->semName);
    free(shm);
}

sharedMemoryADT openShMem(const int id, const long flags) 
{
    sharedMemoryADT shm = malloc(sizeof(sharedMemoryCDT));
    shm->id = id;
    shm->shmName = calloc(MAX_NAME, sizeof(char));
    sprintf(shm->shmName, "/shm%d", id);
    shm->pointer = NULL;
    
    shm->semaphore = openSemaphore(shm);
    checkIsNotNull(shm->semaphore, "sem_open() Failed");
    shm->fd = shm_open(shm->shmName, flags, 0777);
    checkFail(shm->fd, "shm_open() Failed");
    return shm;
}

void closeShMem(sharedMemoryADT shm)
{
    checkIsNull(shm->pointer, "closeShMem() Failed, you must use deletShm()");
    free(shm->shmName);
    free(shm->semName);
    free(shm);
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

ssize_t writeShMem(sharedMemoryADT shm, const void* buffer , size_t nbytes)
{
    sem_wait(shm->semaphore);
    ssize_t result = write(shm->fd, buffer, nbytes);
    sem_post(shm->semaphore);
    return result;
}

ssize_t readShMem(sharedMemoryADT shm, void* buffer , size_t nbytes)
{
    sem_wait(shm->semaphore);
    ssize_t result = read(shm->fd, buffer, nbytes);
    sem_post(shm->semaphore);
    return result;
}

