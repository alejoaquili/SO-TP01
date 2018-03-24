#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <mqueue.h>

#define MSG_SIZE 256
#define QUEUE_NAME  "/filesToHash"
#define MD5SUM_LENGTH 7

int main(void) 
{
	mqd_t mqDescriptor;
	char fileToHash[MSG_SIZE];
	ssize_t bytesRead;
	mqDescriptor = mq_open(QUEUE_NAME, O_RDONLY | O_NONBLOCK);
	bytesRead = mq_receive(mqDescriptor, fileToHash, MSG_SIZE, NULL);
	if(bytesRead < 0)
	{
		switch (errno)
		{
			case EAGAIN:
				exit(0);
				break;
			default:
				fprintf(stderr, "mq_receive Failed\n");
				perror("mq_receive Failed");
				exit(1);
				break;
		}
	}
	mq_close(mqDescriptor);


	pid_t child;
	int fd[2];
	if(pipe(fd) < 0)
	{
		fprintf(stderr, "Pipe Failed\n");
		perror("Pipe Failed\n");
		exit(1);
	}

	child = fork();
	if(child < 0)
	{
		fprintf(stderr, "Fork Failed\n");
		perror("Fork Failed\n");
		exit(1);
	}
	else if (child == 0)
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
    		sprintf(new, "%s %s", "HASH: ", buf);
    		printf("%s\n", new);
		exit(1);
	}
	return 0;
}

