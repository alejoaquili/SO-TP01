#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>
#include "errorslib.h"
#include "messageQueue.h"
#include "applicationProcess.h"
#include "processlib.h"
#include "sharedMemory.h"

void enqueueFiles(char** nameFiles, long qty);
void readHashes(sharedMemoryADT shm, FILE * outputFile);
void reciveHashes(messageQueueADT mqHashes, sharedMemoryADT shm, long qty, FILE * outputFile);

int main(int argc, char * argv[]) 
{
	messageQueueADT mqHashes;
	pid_t* children; 
	int* status = calloc(SLAVE_QTY, sizeof(int));
	int size = (MSG_SIZE + HASH_SIZE + 2) * (argc -1), pid = getpid();

	printf("applicationProcess PID = %d\n", pid);

	sharedMemoryADT shm = sharedMemoryCreator(pid, size, O_RDWR);

	FILE * outputFile = fopen("./output.txt", "w+");
	checkIsNotNull(outputFile, "fopen() Failed");

	enqueueFiles(argv + 1, argc - 1);
	
	mqHashes = messageQueueCreator(QUEUE_HASH_STORAGE, O_RDONLY, argc - 1, MSG_SIZE + HASH_SIZE + 2);
	
	children = childFactory(SLAVE_QTY, SLAVE_PATH);
	
	reciveHashes(mqHashes, shm, argc - 1, outputFile);

	fclose(outputFile);

	for(int j = 0; j < SLAVE_QTY; j++)
		waitpid(children[j], &(status[j]), 0);

	freeSpace(2, children, status);

	closeMQ(mqHashes);
	printf("All Child process finished.\n");
	deleteMQ(QUEUE_FILE_NAME);
	deleteMQ(QUEUE_HASH_STORAGE);
	printf("Waiting for a view process ...\n");
	sleep(5);
	deleteShMem(shm);
	return 0;
}

void reciveHashes(messageQueueADT mqHashes, sharedMemoryADT shm, long qty, FILE * outputFile)
{
	int fd = getDescriptor(mqHashes);
	ssize_t result;
	fd_set rfd;

 	FD_ZERO( &rfd );
    FD_SET(fd, &rfd);

    while(qty--)
    {
    	int result = select(fd + 1, &rfd, 0, 0, NULL);
    	checkFail(result, "select() Failed");
    	readHashes(shm, outputFile);
	}
	char * centinela = calloc (MSG_SIZE + HASH_SIZE + 2, sizeof(char));
	centinela[0] = -1;
	result = writeShMem(shm, centinela, MSG_SIZE + HASH_SIZE + 2);
	checkFail(result, "writeShMem() Failed");
	free(centinela);
}

void readHashes(sharedMemoryADT shm, FILE * outputFile)
{
	ssize_t bytesRead, result;
	char fileHashed[MSG_SIZE + HASH_SIZE + 2];
	messageQueueADT mqHashes = openMQ(QUEUE_HASH_STORAGE, O_RDONLY);

	bytesRead = readMessage(mqHashes, fileHashed, NULL);
	checkFail(bytesRead, "readMessage() Failed");

	printf("%s\n", fileHashed);

	result = writeShMem(shm, fileHashed, MSG_SIZE + HASH_SIZE + 2);
	checkFail(result, "writeShMem() Failed");
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

void freeSpace(int qty, ...) 
{
	va_list args;
    va_start(args, qty);

    for (int i = 0; i < qty; i++)
        free(va_arg(args, void*));

    va_end(args);
}


