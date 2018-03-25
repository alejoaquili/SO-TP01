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

int main(void) 
{
	mqd_t mqDescriptor;
	char fileToHash[MSG_SIZE];
	ssize_t bytesRead;
	mqDescriptor = mq_open(QUEUE_NAME, O_RDONLY | O_NONBLOCK);
	if (mqDescriptor < 0)
	{
			fprintf(stderr, "mq_open Failed\n");
			perror("mq_open Failed");
			exit(1);
	}
	while((bytesRead = mq_receive(mqDescriptor, fileToHash, MSG_SIZE, NULL)) > 0)
	{
		fileToHash[bytesRead] = 0;
		pid_t child;
		int fd[2];
		checkFail(pipe(fd), "Pipe Failed");

		child = fork();
		checkFail(child, "Fork Failed");
		
		if (child == 0)
		{
			dup2(fd[1], 1);
			close(fd[0]);
			char command [MSG_SIZE + MD5SUM_LENGTH];
			sprintf(command, "%s %s", "md5sum", fileToHash); 
			system(command);
			exit(1);
		}
		else
		{
			char buf[MSG_SIZE + MD5SUM_LENGTH];
			close(fd[1]);
	    	read(fd[0], buf, sizeof(buf));

			char new [300];
			fflush(stdout);
	    	sprintf(new, "%s %s%c%c", "HASH: ", buf,0,0);
	    	printf("%s\n", new);
		}

	}
	mq_close(mqDescriptor);
	if(bytesRead < 0)
	{
		if(errno == EAGAIN)
			return 0;
		fprintf(stderr, "mq_receive Failed\n");
		perror("mq_receive Failed");
		exit(1);
	}	
	return 0;
}
