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
#define SLAVE_QTY 1
#define SLAVE_NAME "./Slave/slaveProcess.out"

extern int errno;

mqd_t messageQueueCreator(char* name, long flags1, struct mq_attr* attributes);
pid_t * childFactory(int qty, char* childName);

int main(int argc, char * argv[]) 
{
	mqd_t mqDescriptor;
	pid_t* childs;
	int* status;
	struct mq_attr attributes;

	attributes.mq_maxmsg = argc - 1;
	attributes.mq_msgsize = MSG_SIZE;
	attributes.mq_flags = O_NONBLOCK;

	mqDescriptor = messageQueueCreator(QUEUE_NAME, O_WRONLY| O_CREAT, &attributes);

	for(int i = 0 ; i < argc - 1; i++)
	{
		if(mq_send(mqDescriptor, argv[i+1], MSG_SIZE, 0) < 0)
		{
			fprintf(stderr, "mq_send Failed\n");
			perror("mq_send Failed");
			exit(1);
		}
	}

	childs = childFactory(SLAVE_QTY, SLAVE_NAME);
	status = calloc(SLAVE_QTY, sizeof(int));

	for(int j = 0; j < SLAVE_QTY; j++)
	{
		waitpid(childs[j], &(status[j]), 0);
	}
	fflush(stdout);
	printf("All Child process finished.\n");
	mq_unlink(QUEUE_NAME);

	//freeSpace(&mqDescriptor, childs);
	return 0;
}

mqd_t messageQueueCreator(char* name, long flags1, struct mq_attr* attributes) 
{
	mqd_t* mqDescriptor = malloc(sizeof(mqd_t));

	*mqDescriptor = mq_open(name, flags1, 0666, attributes);
	checkFail(*mqDescriptor, "mq_open Failed");

	return *mqDescriptor;
}

pid_t * childFactory(int qty, char* childName)
{
	pid_t* childs = malloc(qty * sizeof(pid_t*));

	for (int i = 0; i < qty; i++)
	{ 
		childs[i] = fork();
		checkFail(childs[i], "Fork Failed");
		
		if (childs[i] == 0)
		{
			execlp(childName, " ", ((char *)NULL));

			fail("Exec Failed");
		}
	}
	return childs;
}

/*
void freeSpace(void * memory, ...) 
{
	va_list args;
    va_start(args, qty);

    for (int i = 0; i < qty; i++)
    {
        free(va_args(args, i));
    }
    va_end(args);
}
*/
