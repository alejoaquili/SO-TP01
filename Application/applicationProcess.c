#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h> 
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include "errorslib.h"
#include "messageQueue.h"
#include "applicationProcess.h"
#include "processlib.h"

void readHashes(int fd, sem_t* semaphore);
void freeSpace(int qty, void * memory, ...);

int main(int argc, char * argv[]) 
{
	messageQueueADT mqHashes;
	pid_t* children;
	int* status;

//create shm name

	char shmName[MAX_PID_LENGTH+4];
	sprintf(shmName, "/shm%d", getpid());

//create SHM 
	//mode_t old_umask = umask(0); S_IRUSR | S_IWUSR
	int shmFd = shm_open(shmName, O_WRONLY | O_CREAT, 0777);
	//umask(old_umask);
	checkFail(shmFd, "shm_open Failed");
	int size = (MSG_SIZE + HASH_SIZE + 2) * (argc-1);
	void * shmPointer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
	

//before start
	printf("applicationProcess PID = %d\n", (int)getpid());
	printf("Starting in a seconds ...\n");
	sleep(10);

//semaphore
	char semName[MAX_PID_LENGTH+4];
	sprintf(semName, "/sem%d", getpid());

	sem_t* mutexSemaphore = sem_open("sem", O_CREAT|O_EXCL, 0777, 1);


	if(mutexSemaphore == (void*)0)
		printf("La cagaste\n");



	enqueueFiles(argv + 1, argc - 1);
	
	mqHashes = messageQueueCreator(QUEUE_HASH_STORAGE, O_RDONLY, argc - 1, MSG_SIZE + HASH_SIZE + 2);
	children = childFactory(SLAVE_QTY, SLAVE_PATH);
	status = calloc(SLAVE_QTY, sizeof(int));

	reciveHashes(mqHashes, shmFd, mutexSemaphore, argc - 1);

	for(int j = 0; j < SLAVE_QTY; j++)
		waitpid(children[j], &(status[j]), 0);
	freeSpace(1, children);
	closeMQ(mqHashes);
	printf("All Child process finished.\n");
	deleteMQ(QUEUE_FILE_NAME);
	deleteMQ(QUEUE_HASH_STORAGE);

	printf("Waiting for a view process ...\n");
	sleep(10);


	munmap(shmPointer, size);
	//Grabar en disco

	// free de SHM y semaforo
	shm_unlink(shmName);
	sem_unlink(semName);

	return 0;
}

void reciveHashes(messageQueueADT mqHashes, int shmFd, sem_t* semaphore, long qty)
{
	fd_set rfd;
	int fd = getDescriptor(mqHashes);
 	FD_ZERO( &rfd );
    FD_SET(fd, &rfd);

    while(qty--)
    {
    	int result = select(fd + 1, &rfd, 0, 0, NULL);
    	checkFail(result, "Select Failed");
    	readHashes(shmFd, semaphore);
	}
}

void readHashes(int fd, sem_t* semaphore) 
{
	ssize_t bytesRead;
	char fileHashed[MSG_SIZE + HASH_SIZE + 2];
	messageQueueADT mqHashes = openMQ(QUEUE_HASH_STORAGE, O_RDONLY);
	bytesRead = readMessage(mqHashes, fileHashed, NULL);
	sem_wait(semaphore);
	write(fd, fileHashed, MSG_SIZE + HASH_SIZE + 2);
	printf("%s\n", fileHashed);
	sem_post(semaphore);
	closeMQ(mqHashes);
	
}

void enqueueFiles(char** nameFiles, long qty)
{
	messageQueueADT mqFiles;
	mqFiles = messageQueueCreator(QUEUE_FILE_NAME, O_WRONLY, qty, MSG_SIZE);
	enqueueMessages(mqFiles, nameFiles, qty);
	closeMQ(mqFiles);
}

void freeSpace(int qty, void * memory, ...) 
{
	va_list args;
    va_start(args, memory);

    for (int i = 0; i < qty; i++)
    {
        free(va_arg(args, void*));
    }
    va_end(args);
}


