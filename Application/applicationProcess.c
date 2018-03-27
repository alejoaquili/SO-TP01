#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/mman.h> 
#include <fcntl.h>
#include <semaphore.h>
#include "errorslib.h"
#include "messageQueue.h"
#include "applicationProcess.h"
#include "processlib.h"

void readHashes(int fd);
void freeSpace(int qty, void * memory, ...);

int main(int argc, char * argv[]) 
{
	messageQueueADT mqHashes;
	pid_t* children;
	int* status;

//create SHM

	int shmFd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0666);
	checkFail(shmFd, "shm_open Failed");
	int size = ((MSG_SIZE + HASH_SIZE + 2) * (argc-1))+ MAX_PID_LENGTH;
	void * shmPointer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
	
//write PID on SHM
	char buf[MAX_PID_LENGTH];
	sprintf(buf, "%d", (int)getpid());
	int writeResult = write(shmFd, buf, MAX_PID_LENGTH);
	checkFail(writeResult, "write Failed");


//before start
	printf("applicationProcess PID = %d\n", (int)getpid());
	printf("Starting in a seconds ...\n");
	sleep(5);

//semaphore

	sem_t* semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0666);
	int res = sem_init(semaphore, 1, 1);
	checkFail(res, "sem_init Failed");
	

	enqueueFiles(argv + 1, argc - 1);
	
	mqHashes = messageQueueCreator(QUEUE_HASH_STORAGE, O_RDONLY, argc - 1, MSG_SIZE + HASH_SIZE + 2);
	children = childFactory(SLAVE_QTY, SLAVE_PATH);
	status = calloc(SLAVE_QTY, sizeof(int));

	reciveHashes(mqHashes, shmFd, argc - 1);

	for(int j = 0; j < SLAVE_QTY; j++)
		waitpid(children[j], &(status[j]), 0);
	freeSpace(1, children);
	closeMQ(mqHashes);
	printf("All Child process finished.\n");
	deleteMQ(QUEUE_FILE_NAME);
	deleteMQ(QUEUE_HASH_STORAGE);

// free de SHM
	shm_unlink(SHARED_MEMORY_NAME);

	return 0;
}

void reciveHashes(messageQueueADT mqHashes, int shmFd, long qty)
{
	fd_set rfd;
	int fd = getDescriptor(mqHashes);
 	FD_ZERO( &rfd );
    FD_SET(fd, &rfd);
    while(qty--)
    {
    	int result = select(fd + 1, &rfd, 0, 0, NULL);
    	checkFail(result, "Select Failed");
    	readHashes(shmFd);
	}
}

void readHashes(int fd) 
{
	ssize_t bytesRead;
	char fileHashed[MSG_SIZE + HASH_SIZE + 2];
	messageQueueADT mqHashes = openMQ(QUEUE_HASH_STORAGE, O_RDONLY);
	bytesRead = readMessage(mqHashes, fileHashed, NULL);
	write(fd, fileHashed, MSG_SIZE + HASH_SIZE + 2);
	printf("%s\n", fileHashed);
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


