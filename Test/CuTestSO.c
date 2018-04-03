#include <stdio.h>
#include "CuTest.h"
#include "messageQueue.h"
#include "errorslib.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h> 
#include <fcntl.h>
#include "CuTestSO.h"
#include <mqueue.h>
#include "string.h"
#include "sharedMemory.h"

/*-----------------------------------
 variables for message queue testing
 ------------------------------------
*/
long maxQueuedMessages = 2;
long currentQueuedMessages = 0;
long maxMessageSize = 256;
long messageQueueFlags = O_RDWR;
char* name = "/test1";
char* message;
int messageLength = 8;

messageQueueADT mq;
//------------------------------------



/*
--------------------------------------
variables for shared memory testing
--------------------------------------
*/

sharedMemoryADT sharedMemory;
int sharedMemoryId;
long sharedMemoryFlags = O_RDWR;
long sharedMemorySize = 256;

char * sharedMemoryMessage = "testing";
long sharedMemoryMessageLength = 8;

//------------------------------------



void testCreateMessageQueue(CuTest* tc)
{
	CuAssert(tc,"Fail to create a message queue", whenAMessageQueueCreated());
	closeMQ(mq);
}

void testQueueMessage(CuTest* tc)
{

	givenAMessageQueue();
	givenAMessage();
	CuAssert(tc, "Fail to queue a message ", whenMessageQueued());
	closeMQ(mq);
}

void testQueueOpen(CuTest* tc)
{
	givenAMessageQueue();
	CuAssert(tc, "Fail to open an existing queue", whenOpenMessageQueue());
	closeMQ(mq);

}


void testSharedMemoryCreation(CuTest* tc)
{
	givenAnId();
	CuAssert(tc, " Fail to create a shared memory", whenSharedMemoryCreated());
	deleteShMem(sharedMemory);
}


void testSharedMemoryOpen(CuTest* tc)
{
	givenASharedMemory();
	CuAssert(tc, "Fail to open an existing shared memory", whenSharedMemoryOpened());
	deleteShMem(sharedMemory);

}

void testReadWriteSharedMemory(CuTest* tc)
{
	//givenASharedMemory();
	CuAssert(tc, "Fail Read or Write operation", whenSharedMemoryWrittenaAndRead());
	deleteShMem(sharedMemory);	
}





void givenAnId()
{
	sharedMemoryId = getpid();
}

void givenASharedMemory()
{
	givenAnId();
	sharedMemory = sharedMemoryCreator(sharedMemoryId, sharedMemorySize,
		sharedMemoryFlags);
}

void givenAMessage()
{
	message = "testing";
	enqueueMessage(mq, message);
}


void givenAMessageQueue()
{
	mq = messageQueueCreator(name,messageQueueFlags,maxQueuedMessages, 
		maxMessageSize);
}



int whenSharedMemoryWrittenaAndRead()
{
	pid_t child = fork();
	if(child == 0)
	{
		sharedMemoryADT shm = sharedMemoryCreator(sharedMemoryId, sharedMemorySize,
		sharedMemoryFlags);
		writeShMem(shm, sharedMemoryMessage, sharedMemoryMessageLength);
		closeShMem(shm);
		exit(0);

	}
	else
	{
		wait(NULL);
		sharedMemoryADT shm2 = openShMem(sharedMemoryId, O_RDONLY);
		char readMessage[sharedMemoryMessageLength];
		readShMem(shm2, readMessage, sharedMemoryMessageLength);
		int response = 1;
		if(strcmp(readMessage, sharedMemoryMessage) != 0)
			response = 0;
		return response;
	}	

	return 0;

}



int whenSharedMemoryOpened()
{
	sharedMemoryADT testSharedmemory =  openShMem(sharedMemoryId, sharedMemoryFlags);
	if(getShMemId(testSharedmemory) != getShMemId(sharedMemory))
		return 0;

	return 1;
}

int whenSharedMemoryCreated()
{
	sharedMemory =  sharedMemoryCreator(sharedMemoryId, sharedMemorySize,
		sharedMemoryFlags);
	if(sharedMemory == NULL)
		return 0;

	return 1;

}

int whenOpenMessageQueue()
{
	if((void*)0 == openMQ(name, messageQueueFlags))
		return 0;

	return 1;
}


int whenAMessageQueueCreated()
{
	mq = messageQueueCreator(name,messageQueueFlags,maxQueuedMessages,
	 maxMessageSize);
	if(mq == (void*) 0)
		return 0;

	return 1;
}

int whenMessageQueued()
{
	char* messageRecieved = malloc( messageLength*sizeof(char) );
	readMessage(mq, messageRecieved, NULL);
	int response = 1;
	if(strcmp(message, messageRecieved) != 0 )
		response = 0;

	//free(messageRecieved); no se porque tira error
	return response;
}










CuSuite* CuQueueGetSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testCreateMessageQueue);
	SUITE_ADD_TEST(suite, testQueueMessage);
	SUITE_ADD_TEST(suite, testQueueOpen);
	SUITE_ADD_TEST(suite, testSharedMemoryCreation);
	SUITE_ADD_TEST(suite, testSharedMemoryOpen);
	SUITE_ADD_TEST(suite, testReadWriteSharedMemory);
	return suite;
}
