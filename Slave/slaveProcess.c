#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <mqueue.h>
#include "errorslib.h"

#define MSG_SIZE 256
#define QUEUE_NAME  "/filesToHash"
#define MD5SUM_LENGTH 7

void md5sum(size_t commandLength, char * fileToHash);
void childProcess(int * fd, char * fileToHash);
void parentProcess(int * fd);
int checkQueueIsEmpty(mqd_t mqDescriptor, ssize_t bytesRead);
void hashAFile(ssize_t bytesRead, char * fileToHash);

int main(void) 
{
	mqd_t mqDescriptor;
	char fileToHash[MSG_SIZE];
	ssize_t bytesRead;

	mqDescriptor = mq_open(QUEUE_NAME, O_RDONLY | O_NONBLOCK);
	checkFail(mqDescriptor, "mq_open Failed");

	while((bytesRead = mq_receive(mqDescriptor, fileToHash, MSG_SIZE, NULL)) > 0)
		hashAFile(bytesRead, fileToHash);
	
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

void parentProcess(int * fd)
{
	char buf[MSG_SIZE + MD5SUM_LENGTH];
	close(fd[1]);
	read(fd[0], buf, sizeof(buf));
	char new [300];
	fflush(stdout);
	sprintf(new, "%s %s%c%c", "HASH: ", buf,0,0);
	printf("%s\n", new);
}


int checkQueueIsEmpty(mqd_t mqDescriptor, ssize_t bytesRead)
{
	mq_close(mqDescriptor);
	if(bytesRead < 0)
	{
		if(errno == EAGAIN)
			return 0;
		fail("mq_receive Failed");
	}	
	return 0;
}

void hashAFile(ssize_t bytesRead, char * fileToHash)
{
	fileToHash[bytesRead] = 0;

	pid_t child;
	int fd[2];

	checkFail(pipe(fd), "Pipe Failed");

	checkFail(child = fork(), "Fork Failed");
		
	if (child == 0)
		childProcess(fd, fileToHash);
	else
		parentProcess(fd);
}

