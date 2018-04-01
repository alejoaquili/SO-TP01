#ifndef UNIT_TEST
#define UNIT_TEST

void testCreateMessageQueue(CuTest* tc);
void testQueueMessage(CuTest* tc);
void testQueueOpen(CuTest* tc);
void testSharedMemoryCreation(CuTest* tc);
void testSharedMemoryOpen(CuTest* tc);
void testReadWriteSharedMemory(CuTest* tc);

void givenAMessageQueue();
void givenAMessage();
void givenAnId();
void givenASharedMemory();



int whenSharedMemoryOpened();
int whenSharedMemoryCreated();
int whenMessageQueued();
int whenOpenMessageQueue();
int whenSharedMemoryWriteRead();
int whenAMessageQueueCreated();

CuSuite* CuQueueGetSuite(void);
#endif