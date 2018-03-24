#include  <sys/types.h>
#include <sys/wait.h>
#include  <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <mqueue.h>

#define MSG_SIZE 256
#define QUEUE_NAME  "/filesToHash"
#define SLAVE_QTY 1

extern int errno;

int main(int argc, char * argv[]) 
{
	struct mq_attr attributes;
	mqd_t mqDescriptor;

	attributes.mq_maxmsg = argc - 1;
	attributes.mq_msgsize = MSG_SIZE;
	attributes.mq_flags = O_NONBLOCK;

	mqDescriptor = mq_open (QUEUE_NAME, O_WRONLY| O_CREAT, 0666, &attributes);
	if (mqDescriptor < 0)
	{
			fprintf(stderr, "mq_open Failed\n");
			perror("mq_open Failed");
			exit(1);
	}

	for(int i = 0 ; i < argc - 1; i++)
	{
		if(mq_send(mqDescriptor, argv[i+1], MSG_SIZE, 0) < 0)
		{
			fprintf(stderr, "mq_send Failed\n");
			perror("mq_send Failed");
			exit(1);
		}
	}


	int status[SLAVE_QTY];
	pid_t childs[SLAVE_QTY];

	for (int i = 0; i < SLAVE_QTY; i++)
	{ 
		childs[i] = fork();
		if(childs[i] < 0)
		{
			fprintf(stderr, "Fork Failed\n");
			perror("Fork Failed");
			exit(1);
		}
		else if (childs[i] == 0)
		{
			execlp("./slaveProcess.out", " ", ((char *)NULL));
			fprintf(stderr, "Exec Failed\n");
			perror("Exec Failed");
			exit(1);
		}
	}

	for(int j = 0; j < SLAVE_QTY; j++)
	{
		waitpid(childs[j], &(status[j]), 0);
	}
	fflush(stdout);
	printf("All Child process finished.\n");
	mq_unlink(QUEUE_NAME);
	return 0;
}

