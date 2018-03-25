#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <mqueue.h>
#include "errorslib.h"
#include <stdarg.h>

#define MSG_SIZE 256
#define QUEUE_NAME  "/filesToHash"
#define FIFO_PATH "./Fifos/slave"
#define SLAVE_QTY 2
#define SLAVE_PATH "./Slave/slaveProcess.out"

mqd_t messageQueueCreator(char* name, long flags1, struct mq_attr* attributes);
pid_t * childFactory(int qty, char* childName, int * fifoFds);

int main(int argc, char * argv[]) 
{
	mqd_t mqDescriptor;
	struct mq_attr attributes;
	int fifoFds[SLAVE_QTY];
	pid_t* childs;
	int* status;

	attributes = setMQAttributes(&attributes, argc - 1, MSG_SIZE, O_NONBLOCK);
	mqDescriptor = messageQueueCreator(QUEUE_NAME, O_WRONLY| O_CREAT, &attributes);
	sendMessages(mqDescriptor, argv + 1, argc - 1, MSG_SIZE);

	char ** fifoPaths = makeNames(FIFO_PATH, strlen(FIFO_PATH), SLAVE_QTY);

	childs = childFactory(SLAVE_QTY, SLAVE_PATH, fifoPaths);

	status = calloc(SLAVE_QTY, sizeof(int));

	for(int j = 0; j < SLAVE_QTY; j++)
	{
		waitpid(childs[j], &(status[j]), 0);
	}
	fflush(stdout);
	printf("All Child process finished.\n");
	mq_unlink(QUEUE_NAME);

	freeSpace(1, childs);
	return 0;
}

char** makeNames(char* name, int length, int qty)
{
	char** names = malloc(qty * sizeof(char**));
	for (int i = 0; i < qty; ++i)
	{
		names[i] = malloc((length + 10) * sizeof(int));
		sprintf(names[i], "%s%d%c", name, i+1,'\0');
	}
	return names;
}

void setMQAttributes(struct mq_attr* attributes, long maxMsg, long msgSize, long flags)
{
	attributes->mq_maxmsg = maxMsg;
	attributes->mq_msgsize = msgSize;
	attributes->mq_flags = flags;
}

mqd_t messageQueueCreator(char* name, long flags, struct mq_attr* attributes) 
{
	mqd_t mqDescriptor =  mq_open(name, flags, 0666, attributes);
	checkFail(*mqDescriptor, "mq_open Failed");

	return mqDescriptor;
}

void sendMessages(mqd_t mqDescriptor, char** msgs, int qty, int msgSize) 
{
	for(int i = 0 ; i < qty; i++)
	{
		int result = mq_send(mqDescriptor, msgs[i], msgSize, 0);
		checkFail(result, "mq_send Failed");
	}
}

pid_t * childFactory(int qty, char* childName, int * fifoFds)
{
	pid_t* childs = malloc(qty * sizeof(pid_t*));
	char fifoName[20];
	for (int i = 0; i < qty; i++)
	{ 
		childs[i] = fork();
		checkFail(childs[i], "Fork Failed");
		if (childs[i] == 0)
		{
			sprintf(fifoName, "%s%d%c", FIFO_NPATH, i+1,'\0');
			checkFail(mkfifo(fifoName, 0666), "mkfifo Failed");
			//fifoFds[i] = open(fifoName, O_RDONLY);
			execlp(childName, fifoName, ((char *)NULL));
			fail("Exec Failed");
		}
	}
	return childs;
}

void freeSpace(int qty, void * memory, ...) 
{
	va_list args;
    va_start(args, memory);

    for (int i = 0; i < qty; i++)
    {
        free(va_arg(args, void*));
    }
    va_end(args);
}
