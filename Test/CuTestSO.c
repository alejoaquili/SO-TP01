#include <stdio.h>
#include "include/CuTest.h"
#include "messageQueue.h"
#include "errorslib.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h> 
#include <fcntl.h>
#include "include/CuTestSO.h"
#include <mqueue.h>
#include "string.h"

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
int id;
long sharedMemoryFlags = O_RDWR;


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
	return suite;
}
