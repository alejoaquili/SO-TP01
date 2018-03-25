//#include <stdin.h>
//#include <stdout.h>
//#include <math.h>
//#include "testlib.h"

void checkIsNotNull(void * pointer) 
{
	if (pointer == NULL)
	{
		fail("Expected a non null pointer, got: a null pointer");
	}
	else 
	{
		ok();
	}
	putchar('\n');
}

void checkBothAddressesAreNotEqual(void * anAddress, void * otherAddress) 
{
	if (anAddress == otherAddress)
	{
		fail("Expected: different addresses, got: same addresses");
	}
	else 
	{
		ok();
	}
	putchar('\n');
}

void checkIsEqualOrGreaterThan(int aNumber, int otherNumber)
{
	if (aNumber < otherNumber)
	{
		fail("Expected: first greater or equal than second, got: first lower than second");
	}
	else 
	{
		ok();
	}
	putchar('\n');
}

void checkFail(int aNumber, char* msg)
{
	if (aNumber <= 0)
	{
		fail(msg);
	}
	else 
	{
		ok();
	}
	putchar('\n');
}

void fail(char * msg) 
{
	fflush(stdout);
	fprintf(stderr, msg);
}

void ok() 
{
	printf("   Ok!\n");
}
