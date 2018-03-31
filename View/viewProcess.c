#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/select.h>
#include "errorslib.h"
#include "applicationProcess.h"

int readLine(int shmFd, char* buffer, sem_t* semaphore);

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
	checkFail(shmFd, "shm_open() Failed");

	fd_set rfd;
 	FD_ZERO( &rfd );
    FD_SET(shmFd, &rfd);

    char* b;
    do
    {
    	char newBuffer[MSG_SIZE + HASH_SIZE + 2];
    	b = newBuffer;
    	int result = select(shmFd + 1, &rfd, 0, 0, NULL);
    	checkFail(result, "select() Failed");
		int readBytes = readLine(shmFd, newBuffer, mutexSemaphore);
		b[readBytes] = 0;
		if (readBytes > 0 && b[0] != (char)EOF)
		{
			printf("%s\n", newBuffer);
		}
	} while(b[0] != (char)EOF);
}

int readLine(int shmFd, char* buffer, sem_t* semaphore) 
{
   	sem_wait(semaphore);
	int readBytes = read(shmFd, buffer, MSG_SIZE + HASH_SIZE + 2);
	sem_post(semaphore);
	checkFail(readBytes, "read() Failed");
	return readBytes;
}