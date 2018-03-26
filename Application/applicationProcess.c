#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>
#include "errorslib.h"
#include "messageQueue.h"
#include "applicationProcess.h"
#include "processlib.h"

void readHashes();
void freeSpace(int qty, void * memory, ...);

int main(int argc, char * argv[]) 
{
	messageQueueADT mqHashes;
	pid_t* children;
	int* status;

	enqueueFiles(argv + 1, argc - 1);
	
	mqHashes = messageQueueCreator(QUEUE_HASH_STORAGE, O_RDONLY, argc - 1, MSG_SIZE + HASH_SIZE + 2);
	children = childFactory(SLAVE_QTY, SLAVE_PATH);
	status = calloc(SLAVE_QTY, sizeof(int));

	reciveHashes(mqHashes, argc - 1);

	for(int j = 0; j < SLAVE_QTY; j++)
	{
		waitpid(children[j], &(status[j]), 0);
	}

	closeMQ(mqHashes);
	printf("All Child process finished.\n");
	deleteMQ(QUEUE_FILE_NAME);
	deleteMQ(QUEUE_HASH_STORAGE);

	freeSpace(1, children);
	return 0;
}

void reciveHashes(messageQueueADT mqHashes, long qty)
{
	fd_set rfd;
	int fd = getDescriptor(mqHashes);
 	FD_ZERO( &rfd );
    FD_SET(fd, &rfd);
    while(qty--)
    {
    	int result = select(fd + 1, &rfd, 0, 0, NULL);
    	checkFail(result, "Select Failed");
    	readHashes();
	}
}

void readHashes() 
{
	ssize_t bytesRead;
	char fileHashed[MSG_SIZE + HASH_SIZE + 2];
	messageQueueADT mqHashes = openMQ(QUEUE_HASH_STORAGE, O_RDONLY);
	bytesRead = readMessage(mqHashes, fileHashed, NULL);
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
