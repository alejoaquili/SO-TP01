#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "messageQueue.h"
#include "errorslib.h"
#include "applicationProcess.h"

#define MD5SUM_LENGTH 7

void md5sum(size_t commandLength, char * fileToHash);
void childProcess(int * fd, char * fileToHash);
void parentProcess(int * fd, char * fifoToWrite);
int checkQueueIsEmpty(mqd_t mqDescriptor, ssize_t bytesRead);
void hashAFile(ssize_t bytesRead, char * fileToHash, char * fifoToWrite);

int main(int argc, char * argv[]) 
{
	messageQueueADT mqFiles;
	char fileToHash[MSG_SIZE];
	ssize_t bytesRead;

	mqFiles = openMQ(QUEUE_FILE_NAME, O_RDONLY);
	while((bytesRead = readMessage(mqFiles, fileToHash, NULL)) >= 0)
		hashAFile(bytesRead, fileToHash, argv[1]);
	
	if(errno != EAGAIN) 
		fail("readMessage Failed");
	closeMQ(mqFiles);
	return 0;

}

void hashAFile(ssize_t bytesRead, char * fileToHash, char * fifoToWrite)
{
	pid_t child;
	int fd[2];
	fileToHash[bytesRead] = 0;
	int result = pipe(fd);
	checkFail(result, "Pipe Failed");
	child = fork();
	checkFail(child, "Fork Failed");

	if (child == 0)
		childProcess(fd, fileToHash);
	else
		parentProcess(fd, fifoToWrite);
}

void childProcess(int * fd, char * fileToHash)
{
	dup2(fd[1], 1);
	close(fd[0]);
	md5sum(MSG_SIZE + MD5SUM_LENGTH, fileToHash);
	exit(1);
}

void parentProcess(int * fd, char * fifoToWrite)
{
	char buffer[MSG_SIZE + HASH_SIZE + 2];
	char hash[HASH_SIZE];
	char fileName[MSG_SIZE];
	messageQueueADT mqHashes;

	close(fd[1]);
	read(fd[0], buffer, sizeof(buffer));
	sscanf(buffer, "%s %s", hash, fileName);
	sprintf(buffer, "%s: %s%c", fileName, hash, 0);

	mqHashes = openMQ(QUEUE_HASH_STORAGE, O_WRONLY);
	sendMessage(mqHashes, buffer);
	closeMQ(mqHashes);
} 

void md5sum(size_t commandLength, char * fileToHash)
{
	char command [commandLength];
	sprintf(command, "%s %s", "md5sum", fileToHash); 
	system(command);
}

