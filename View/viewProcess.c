#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/select.h>
#include "errorslib.h"
#include "applicationProcess.h"

int main(int argc, char * argv[])
{
	checkFail(argc-2, "View Failed");
	char shmName[MAX_PID_LENGTH+4];
	sprintf(shmName, "/shm%s", argv[1]);


	char semName[MAX_PID_LENGTH + 4];
	sprintf(semName, "/sem%s", argv[1]);
	sem_t* mutexSemaphore = sem_open(semName, O_RDWR);

	//shm
	int shmFd = shm_open(shmName, O_RDONLY, 0777);
	checkFail(shmFd, "shm_open Failed");

    int  qty = 2;
    while(qty--)
    {
    	sem_wait(mutexSemaphore);
    	char buffer[MSG_SIZE + HASH_SIZE + 2];
		int readBytes = read(shmFd, buffer, sizeof(buffer));
		checkFail(readBytes, "read Failed");
		printf("%s\n", buffer);
		sem_post(mutexSemaphore);
	}

}

