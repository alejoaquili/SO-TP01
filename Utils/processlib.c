#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <mqueue.h>
#include "errorslib.h"

pid_t * childFactory(int qty, char* childName)
{
	pid_t* childs = malloc(qty * sizeof(pid_t*));

	for (int i = 0; i < qty; i++)
	{ 
		childs[i] = fork();
		checkFail(childs[i], "Fork Failed");
		
		if (childs[i] == 0)
		{
			execlp(childName, " ", ((char *)NULL));

			fail("Exec Failed");
		}
	}
	return childs;
}

pid_t * childFactoryWithArgs(int qty, char* childPath, char** args)
{
	pid_t* childs = malloc(qty * sizeof(pid_t*));

	for (int i = 0; i < qty; i++)
	{ 
		childs[i] = fork();
		checkFail(childs[i], "Fork Failed");

		if (childs[i] == 0)
		{
			execlp(childPath, args[i], ((char *)NULL));
			fail("Exec Failed");
		}
	}
	return childs;
}

