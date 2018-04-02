#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <mqueue.h>

typedef struct messageQueueCDT* messageQueueADT;

messageQueueADT messageQueueCreator(const char* name, const long flagsMQ, 
									const long maxMsg, const long msgSize);
void deleteMQ(char* name);

messageQueueADT openMQ(const char* name, const long flagsMQ);

void closeMQ(messageQueueADT mq);

void enqueueMessage(messageQueueADT mq, const char* msg);

void enqueueMessages(messageQueueADT mq, char** msgs, const int qty);

ssize_t readMessage(messageQueueADT mq, char* buffer, unsigned int* priority);

void setMQAttributes(messageQueueADT mq, const long maxMsg,const long msgSize);

int notifyMQ(messageQueueADT mq, struct sigevent* signalEvent);

mqd_t getMQDescriptor(messageQueueADT mq);

#endif

