#include <mqueue.h>
#include <stdlib.h>
#include <string.h>
#include "errorslib.h"
#include "messageQueue.h"

typedef struct mq_attr attr_t;
static attr_t makeMQAttributes(const long maxMsg, const long msgSize);

typedef struct messageQueueCDT {
	attr_t attributes;
	mqd_t descriptor;
} messageQueueCDT;


messageQueueADT messageQueueCreator(const char* name, const long flagsMQ, const long maxMsg, const long msgSize)
{	
	messageQueueADT mq = malloc(sizeof(messageQueueCDT));

	mq->attributes = makeMQAttributes(maxMsg, msgSize);
	mq->descriptor =  mq_open(name, flagsMQ | O_CREAT, 0666, &(mq->attributes));
	checkFail(mq->descriptor, "mq_open_create Failed");

	return mq;
}

void deleteMQ(char* name)
{
	int result = mq_unlink(name);
	checkFail(result, "mq_unlink Failed");
}

messageQueueADT openMQ(const char* name, const long flagsMQ)
{
	messageQueueADT mq = malloc(sizeof(messageQueueCDT));

	mq->descriptor =  mq_open(name, flagsMQ | O_NONBLOCK);
	checkFail(mq->descriptor, "mq_open Failed");

	int result = mq_getattr(mq->descriptor, &(mq->attributes));
	checkFail(result, "mq_getattr Failed");

	return mq;
}

void closeMQ(messageQueueADT mq)
{
	int result = mq_close(mq->descriptor);
	checkFail(result, "mq_close Failed");
	free(mq);
}

void sendMessage(messageQueueADT mq, const char* msg)
{
	int result = mq_send(mq->descriptor, msg, mq->attributes.mq_msgsize, 0);
	checkFail(result, "mq_send Failed");
}

void sendMessages(messageQueueADT mq, char** msgs, const int qty) 
{
	for(int i = 0 ; i < qty; i++)
	{
		sendMessage(mq, msgs[i]);
	}
}

ssize_t readMessage(messageQueueADT mq, char* buffer, unsigned int* priority)
{
	return mq_receive(mq->descriptor, buffer, mq->attributes.mq_msgsize, priority);
}

int notifyMQ(messageQueueADT mq, struct sigevent* signalEvent)
{
	return mq_notify(mq->descriptor, signalEvent);
}

void setMQAttributes(messageQueueADT mq, const long maxMsg,const long msgSize)
{
	attr_t newAttr = makeMQAttributes(maxMsg, msgSize);
	int result = mq_setattr(mq->descriptor, &newAttr, &(mq->attributes));
	checkFail(result, "mq_setattr Failed");
}

mqd_t getDescriptor(messageQueueADT mq)
{
	return mq->descriptor;
}

static attr_t makeMQAttributes(const long maxMsg, const long msgSize)
{
	attr_t attributes;
	attributes.mq_maxmsg = maxMsg;
	attributes.mq_msgsize = msgSize;
	attributes.mq_flags = O_NONBLOCK;
	return attributes;
}


