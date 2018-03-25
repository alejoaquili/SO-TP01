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

typedef struct {
	mq_attr attributes;
	mqd_t mqDescriptor;
} messageQueueCDT;

typedef messageQueueCDT messageQueue;


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