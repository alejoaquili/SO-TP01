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
	char fileToHash[MSG_SIZE];
	ssize_t bytesRead;

	mqFiles = openMQ(QUEUE_FILE_NAME, O_RDONLY);
	while((bytesRead = readMessage(mqFiles, fileToHash, NULL)) >= 0)
		hashAFile(bytesRead, fileToHash);
	
	if(errno != EAGAIN) 
		fail("readMessage() Failed");
	closeMQ(mqFiles);
	return 0;
}

void hashAFile(ssize_t bytesRead, char * fileToHash)
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
		parentProcess(fd, fileToHash);
}

void parentProcess(int * fd, char* fileToHash)
{
	char buffer[MSG_SIZE + HASH_SIZE + 2], buffer2[MSG_SIZE + HASH_SIZE + 2];
	char hash[HASH_SIZE];
	messageQueueADT mqHashes;

	close(fd[1]);
	read(fd[0], buffer, sizeof(buffer));

	sscanf(buffer, "%32s ", hash);
	sprintf(buffer2, "%s: %s%c", fileToHash, hash, 0);
	mqHashes = openMQ(QUEUE_HASH_STORAGE, O_WRONLY);
	enqueueMessage(mqHashes, buffer2);
	closeMQ(mqHashes);
} 

void childProcess(int * fd, char * fileToHash)
{
	dup2(fd[1], 1);
	close(fd[0]);
	md5sum(MSG_SIZE + MD5SUM_LENGTH, fileToHash);
	exit(1);
}

void md5sum(size_t commandLength, char * fileToHash)
{
	char command [commandLength];
	sprintf(command, "%s %s", "md5sum", fileToHash); 
	system(command);
}

