#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include "errorslib.h"

pid_t * childFactory(int qty, char* childName)
{
	pid_t* children = malloc(qty * sizeof(pid_t*));

	for (int i = 0; i < qty; i++)
	{ 
		children[i] = fork();
		checkFail(children[i], "fork() Failed");
		
		if (children[i] == 0)
		{
			execlp(childName, " ", ((char *)NULL));

			fail("exec() Failed");
		}
	}
	return children;
}

pid_t * childFactoryWithArgs(int qty, char* childPath, char** args)
{
	pid_t* children = malloc(qty * sizeof(pid_t*));

	for (int i = 0; i < qty; i++)
	{ 
		children[i] = fork();
		checkFail(children[i], "fork() Failed");

		if (children[i] == 0)
		{
			execlp(childPath, args[i], ((char *)NULL));
			fail("exec() Failed");
		}
	}
	return children;
}

fd_set necesitoUnNombreParaEstaFuncion(int fd)
{
	fd_set rfd;
 	FD_ZERO( &rfd );
    FD_SET(fd, &rfd);
    return rfd;
}

void waitForOtherProcess(int fd, fd_set rfd)
{
    int result = select(fd + 1, &rfd, 0, 0, NULL);
   	checkFail(result, "select() Failed");
}
