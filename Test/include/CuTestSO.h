#ifndef UNIT_TEST
#define UNIT_TEST

void testCreateMessageQueue(CuTest* tc);
void testQueueMessage(CuTest* tc);
void testQueueOpen(CuTest* tc);
void testSharedMemoryCreation(CuTest* tc);
void testSharedMemoryOpen(CuTest* tc);


void givenAMessageQueue();
void givenAMessage();
void givenAnId();
void givenASharedMemory();



int whenSharedMemoryOpened();
int whenSharedMemoryCreated();
int whenMessageQueued();
int whenOpenMessageQueue();

CuSuite* CuQueueGetSuite(void);
#endif