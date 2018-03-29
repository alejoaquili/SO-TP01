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

	//shm
	int shmFd = shm_open(shmName, O_RDONLY | O_CREAT, 0777);
	checkFail(shmFd, "shm_open Failed");
	//int size = (MSG_SIZE + HASH_SIZE + 2) * (argc-1);
	//void * shmPointer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);


	//semaforo Mutex
	char semName[MAX_PID_LENGTH + 4];
	sprintf(semName, "/sem%s", argv[1]);
	sem_t* mutexSemaphore = sem_open("sem", O_RDWR);
	perror("sem_open Failed:");
	if(mutexSemaphore == (void*)0)
		printf("La cagaste\n");


	//fd_set rfd;
 	//FD_ZERO( &rfd );
    //FD_SET(shmFd, &rfd);

    int  qty = 2;
    while(qty--)
    {
    	sem_wait(mutexSemaphore);
    	//int result = select(shmFd + 1, 0, &rfd, 0, NULL);
    	//checkFail(result, "Select Failed");
    	char buffer[MSG_SIZE + HASH_SIZE + 2];
		int readBytes = read(shmFd, buffer, sizeof(buffer));
		checkFail(readBytes, "read Failed");

		sem_post(mutexSemaphore);
	}
	
	//munmap(shmPointer, size);
	shm_unlink(shmName);
	sem_unlink(semName);
	

}

