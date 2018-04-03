#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "messageQueue.h"
#include "errorslib.h"
#include "slaveProcess.h"
#include "applicationProcess.h"

int main(int argc, char * argv[]) 
{
	messageQueueADT mqFiles;
	char fileToHash[FILE_SIZE];
	ssize_t bytesRead;

	mqFiles = openMQ(QUEUE_FILE_NAME, O_RDONLY);
	while((bytesRead = readMessage(mqFiles, fileToHash, NULL)) >= 0)
	{
		char hashedFile[MSG_SIZE];
		hashTheFile(fileToHash, hashedFile);
		sendTheHash(hashedFile);
	}
	
	if(errno != EAGAIN) 
		fail("readMessage() Failed");
	closeMQ(mqFiles);
	return 0;
}

void sendTheHash(char * hashedFile)
{
	messageQueueADT mqHashes;
	mqHashes = openMQ(QUEUE_HASH_STORAGE, O_WRONLY);
	enqueueMessage(mqHashes, hashedFile);
	closeMQ(mqHashes);
}

void hashTheFile(char * fileToHash, char * buffer)
{
	pid_t child;
	int fd[2];

	int result = pipe(fd);
	checkFail(result, "pipe() Failed");
	child = fork();
	checkFail(child, "fork() Failed");

	if (child == 0)
		childProcess(fd, fileToHash);
	else
		parentProcess(fd, fileToHash, buffer);
}

void parentProcess(int * fd, char * fileToHash, char * buffer)
{
	char buffer2[MSG_SIZE], hash[HASH_SIZE];

	close(fd[1]);
	read(fd[0], buffer2, sizeof(buffer2));

	sscanf(buffer2, "%32s ", hash);
	sprintf(buffer, "%s: %s%c", fileToHash, hash, 0);
} 

void childProcess(int * fd, char * fileToHash)
{
	dup2(fd[1], 1);
	close(fd[0]);
	md5sum(fileToHash);
	exit(1);
}

void md5sum(char * fileToHash)
{
	char command [FILE_SIZE + MD5SUM_LENGTH];
	sprintf(command, "%s %s", "md5sum", fileToHash); 
	system(command);
}

