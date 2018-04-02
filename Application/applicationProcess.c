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

void checkMaxMsgQueue(int qty);
void enqueueFiles(char** nameFiles, long qty);
void readHashes(sharedMemoryADT shm, FILE * outputFile);
void recieveHashes(messageQueueADT mqHashes, sharedMemoryADT shm, long qty, 
															FILE * outputFile);	
void readAHash(sharedMemoryADT shm, char* buffer);
void shareAHash(sharedMemoryADT shm, FILE * outputFile, char* fileHashed);
void setASentinelInShMem(sharedMemoryADT shm);

int main(int argc, char * argv[]) 
{
	checkMaxMsgQueue(argc - 1);

	messageQueueADT mqHashes;
	pid_t* children; 
	int* childrenStatus = calloc(SLAVE_QTY, sizeof(int));
	int memSize = (MSG_SIZE + HASH_SIZE + 2) * (argc -1), pid = getpid();

	printf("applicationProcess PID = %d\n", pid);

	sharedMemoryADT shm = sharedMemoryCreator(pid, memSize, O_RDWR);

	FILE * outputFile = fopen("./output.txt", "w+");
	checkIsNotNull(outputFile, "fopen() Failed");

	enqueueFiles(argv + 1, argc - 1);
	
	mqHashes = messageQueueCreator(QUEUE_HASH_STORAGE, O_RDONLY, argc - 1, 
													 MSG_SIZE + HASH_SIZE + 2);
	children = childFactory(SLAVE_QTY, SLAVE_PATH);
	reciveHashes(mqHashes, shm, argc - 1, outputFile);

	fclose(outputFile);

	for(int j = 0; j < SLAVE_QTY; j++)
		waitpid(children[j], &(childrenStatus[j]), 0);

	freeSpace(2, children, childrenStatus);
	closeMQ(mqHashes);
	printf("All Child process finished.\n");

	deleteMQ(QUEUE_FILE_NAME);
	deleteMQ(QUEUE_HASH_STORAGE);

	printf("Waiting for a view process ...\n");
	sleep(5);
	deleteShMem(shm);
	return 0;
}

void recieveHashes(messageQueueADT mqHashes, sharedMemoryADT shm, long qty, 
															 FILE * outputFile)
{
	int fd = getDescriptor(mqHashes);
	fd_set fdReadSet = createASetOfFds(1, fd);

    while(qty--)
    {
    	char buffer[MSG_SIZE + HASH_SIZE + 2];
    	waitForFds(fd, fdReadSet);
    	readAHash(shm, buffer);
    	shareAHash(shm, outputFile, buffer);
	}
	setASentinelInShMem(shm);
}

void readAHash(sharedMemoryADT shm, char* buffer)
{
	ssize_t bytesRead;
	messageQueueADT mqHashes = openMQ(QUEUE_HASH_STORAGE, O_RDONLY);

	bytesRead = readMessage(mqHashes, buffer, NULL);
	checkFail(bytesRead, "readMessage() Failed");
	closeMQ(mqHashes);
}

void shareAHash(sharedMemoryADT shm, FILE * outputFile, char* fileHashed)
{
	ssize_t result;

	result = writeShMem(shm, fileHashed, MSG_SIZE + HASH_SIZE + 2);
	checkFail(result, "writeShMem() Failed");
	fprintf(outputFile, "%s\n", fileHashed);

	printf("%s\n", fileHashed);
}

void setASentinelInShMem(sharedMemoryADT shm)
{
	ssize_t result;
	char * sentinel = calloc (MSG_SIZE + HASH_SIZE + 2, sizeof(char));
	sentinel[0] = -1;

	result = writeShMem(shm, sentinel, MSG_SIZE + HASH_SIZE + 2);
	checkFail(result, "writeShMem() Failed");
	free(sentinel);
}

void enqueueFiles(char** nameFiles, long qty)
{
	messageQueueADT mqFiles;
	mqFiles = messageQueueCreator(QUEUE_FILE_NAME, O_WRONLY, qty, MSG_SIZE);
	enqueueMessages(mqFiles, nameFiles, qty);
	closeMQ(mqFiles);
}

void checkMaxMsgQueue(int qty)
{
	if(qty > 10)
	{
		char command[50];
		sprintf(command, "echo %d > /proc/sys/fs/mqueue/msg_max", qty);
		system(command);
	}
}

void freeSpace(int qty, ...) 
{
	va_list args;
    va_start(args, qty);

    for (int i = 0; i < qty; i++)
        free(va_arg(args, void*));

    va_end(args);
}


