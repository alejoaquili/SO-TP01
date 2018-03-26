#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <mqueue.h>
#include "errorslib.h"

#define MSG_SIZE 256
#define MAX_HASH 32
#define QUEUE_NAME  "/filesToHash"
#define MD5SUM_LENGTH 7

void md5sum(size_t commandLength, char * fileToHash);
void childProcess(int * fd, char * fileToHash);
void parentProcess(int * fd, char * fifoToWrite);
int checkQueueIsEmpty(mqd_t mqDescriptor, ssize_t bytesRead);
void hashAFile(ssize_t bytesRead, char * fileToHash, char * fifoToWrite);

int main(int argc, char * argv[]) 
{
	mqd_t mqDescriptor;
	char fileToHash[MSG_SIZE];
	ssize_t bytesRead;

	mqDescriptor = mq_open(QUEUE_NAME, O_RDONLY | O_NONBLOCK);
	checkFail(mqDescriptor, "mq_open Failed");

	while((bytesRead = mq_receive(mqDescriptor, fileToHash, MSG_SIZE, NULL)) > 0)
		hashAFile(bytesRead, fileToHash, argv[1]);
	
	checkQueueIsEmpty(mqDescriptor, bytesRead);
}

void md5sum(size_t commandLength, char * fileToHash)
{
	char command [commandLength];
	sprintf(command, "%s %s", "md5sum", fileToHash); 
	system(command);
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

	int fifoFd;
	char buffer[MSG_SIZE + MAX_HASH + 4];

	close(fd[1]);
	read(fd[0], buffer, sizeof(buffer));
	char hash[MAX_HASH];
	char fileName[MSG_SIZE];
	sscanf(buffer, "%s %s", hash, fileName);
	sprintf(buffer, "<%s> <%s>%c", fileName, hash, 0);

	fifoFd = open(fifoToWrite, O_WRONLY);
	checkFail(fifoFd, "Open Failed");
	write(fifoFd, buffer, MSG_SIZE + MAX_HASH + 4);
	close(fifoFd);
}


int checkQueueIsEmpty(mqd_t mqDescriptor, ssize_t bytesRead)
{
	mq_close(mqDescriptor);
	if(bytesRead < 0)
	{
		if(errno == EAGAIN)//Queue is Empty 
			return 0;
		fail("mq_receive Failed");
	}	
	return 0;
}

void hashAFile(ssize_t bytesRead, char * fileToHash, char * fifoToWrite)
{
	pid_t child;
	int fd[2];
	fileToHash[bytesRead] = 0;
	checkFail(pipe(fd), "Pipe Failed");
	checkFail(child = fork(), "Fork Failed");
	if (child == 0)
		childProcess(fd, fileToHash);
	else
		parentProcess(fd, fifoToWrite);
}

