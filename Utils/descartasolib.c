#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "errorslib.h"
#include "messageQueue.h"
#include <stdarg.h>

char** makeNames(char* name, int length, int qty)
{
	char** names = malloc(qty * sizeof(char**));
	for (int i = 0; i < qty; ++i)
	{
		names[i] = malloc((length + 10) * sizeof(int));
		sprintf(names[i], "%s%d%c", name, i+1,'\0');
	}
	return names;
}