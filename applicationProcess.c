#include  <sys/types.h>
#include <sys/wait.h>
#include  <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <mqueue.h>

#define MSG_SIZE 256
#define SLAVE_QTY 4

int main(int argc, char * argv[]) 
{
	struct mq_attr attributes;
	mqd_t mqDescriptor;
	unsigned int priority;

	int status[SLAVE_QTY];
	pid_t childs[SLAVE_QTY];

	attributes.mq_maxmsg = argc -1;
	attributes.mq_msgsize = MSG_SIZE;
	attributes.mq_flags = MQ_NONBLOCK;

	mqDescriptor = mq_open ("filesToHashMQ", O_WRONLY | O_NONBLOCK, 0664, &attributes);
	

	for (int i = 0; i < SLAVE_QTY; i++)
	{ 
		childs[i] = fork();
		if(childs[i] < 0)
		{
			fprintf(stderr, "Fork Failed\n");
			perror("Fork Failed\n");
			exit(1);
		}
		else if (childs[i] == 0)
		{
			execlp("./slaveProcess.out", " ", ((char *)NULL));
			fprintf(stderr, "Exec Failed\n");
			perror("Exec Failed\n");
			exit(1);
		}
	}

	for(int j = 0; j < SLAVE_QTY; j++)
	{
		waitpid(childs[j], &(status[j]), 0);
	}
	printf("All Child process finished.\n");
	return 0;
}
