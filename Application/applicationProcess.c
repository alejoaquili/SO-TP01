#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
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

void enqueueFiles(char** nameFiles, long qty);
void readHashes(int fd, sem_t* semaphore, FILE * outputFile);
void reciveHashes(messageQueueADT mqHashes, int shmFd, long qty, 
										  sem_t* semaphore, FILE * outputFile);

int main(int argc, char * argv[]) 
{
	messageQueueADT mqHashes;
	pid_t* children;
	int* status;

	printf("applicationProcess PID = %d\n", (int)getpid());

//create the output.txt

	FILE * outputFile = fopen("./output.txt", "w+");
	checkIsNotNull(outputFile, "fopen() Failed");

	enqueueFiles(argv + 1, argc - 1);
	
	mqHashes = messageQueueCreator(QUEUE_HASH_STORAGE, O_RDONLY, argc - 1, MSG_SIZE + HASH_SIZE + 2);
	children = childFactory(SLAVE_QTY, SLAVE_PATH);
	status = calloc(SLAVE_QTY, sizeof(int));

	reciveHashes(mqHashes, shmFd, argc - 1, mutexSemaphore, outputFile);

	for(int j = 0; j < SLAVE_QTY; j++)
		waitpid(children[j], &(status[j]), 0);

	free(children);
	free(status);

	//freeSpace(1, children, status);
	closeMQ(mqHashes);
	printf("All Child process finished.\n");
	deleteMQ(QUEUE_FILE_NAME);
	deleteMQ(QUEUE_HASH_STORAGE);

	printf("Waiting for a view process ...\n");
	sleep(10);
	munmap(shmPointer, size);
	
	// free de SHM y semaforo
	shm_unlink(shmName);
	sem_unlink(semName);
	free(semName);

	return 0;
}

void reciveHashes(messageQueueADT mqHashes, int shmFd, long qty, sem_t* semaphore, FILE * outputFile)
{
	//semaphore
	char* semName = calloc(MAX_PID_LENGTH+4, sizeof(char));
	sprintf(semName, "/sem%d", getpid());
    sem_unlink(semName);
	sem_t* mutexSemaphore = sem_open(semName, O_CREAT|O_EXCL, 0777, 1); //ver valgrind jode
	//sem_post(mutexSemaphore);

//create shm name

	char shmName[MAX_PID_LENGTH+4];
	sprintf(shmName, "/shm%d", getpid());

//create SHM 
	int shmFd = shm_open(shmName, O_WRONLY | O_CREAT, 0777);
	checkFail(shmFd, "shm_open() Failed");
	int size = (MSG_SIZE + HASH_SIZE + 2) * argc;
	void * shmPointer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
	checkIsNotNull(shmPointer," Null shmPointer");
	
	fd_set rfd;
	int fd = getDescriptor(mqHashes);
 	FD_ZERO( &rfd );
    FD_SET(fd, &rfd);

    while(qty--)
    {
    	int result = select(fd + 1, &rfd, 0, 0, NULL);
    	checkFail(result, "select() Failed");
    	readHashes(shmFd, semaphore, outputFile);
	}

	//
	sem_wait(semaphore);
	char * sentinela = calloc (MSG_SIZE + HASH_SIZE + 2, sizeof(char));
	sentinela[0] = -1;
	write(shmFd, sentinela, MSG_SIZE + HASH_SIZE + 2); 
	free(sentinela);
	sem_post(semaphore);
	fclose(outputFile);
}

void readHashes(int fd, sem_t* semaphore, FILE * outputFile)
{
	ssize_t bytesRead;
	char fileHashed[MSG_SIZE + HASH_SIZE + 2];
	messageQueueADT mqHashes = openMQ(QUEUE_HASH_STORAGE, O_RDONLY);

	bytesRead = readMessage(mqHashes, fileHashed, NULL);
	checkFail(bytesRead, "readMessage() Failed");
	sem_wait(semaphore);
	write(fd, fileHashed, MSG_SIZE + HASH_SIZE + 2);
	printf("%s\n", fileHashed);
	sem_post(semaphore);
	closeMQ(mqHashes);
	fprintf(outputFile, "%s\n", fileHashed);
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
        free(va_arg(args, void*));

    va_end(args);
}


