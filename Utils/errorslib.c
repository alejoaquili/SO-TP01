#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "errorslib.h"

void checkFail(int aNumber, char* msg)
{
	if (aNumber < 0)
	{
		fail(msg);
	}
}

void fail(char * msg) 
{
	fflush(stdout);
	perror(msg);
	exit(1);
}


void checkIsNotNull(void * aPointer, char* msg)
{
	if( aPointer == (void *) 0 )
		fail(msg);
}