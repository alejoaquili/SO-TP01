#ifndef UNIT_TEST
#define UNIT_TEST

void testCreateMessageQueue(CuTest* tc);
void testQueueMessage(CuTest* tc);
void testQueueOpen(CuTest* tc);

void givenAMessageQueue();
void givenAMessage();


int whenAMessageQueueCreated();
int whenMessageQueued();
int whenOpenMessageQueue();

CuSuite* CuQueueGetSuite(void);
#endif