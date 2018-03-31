#include <stdio.h>
#include "include/CuTest.h"
#include "../Utils/include/messageQueue.h"


long maxQueuedMessages = 2;
long currentQueuedMessages = 0;
long maxMessageSize = 0;