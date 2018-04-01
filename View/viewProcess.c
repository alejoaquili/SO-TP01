#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include "errorslib.h"
#include "applicationProcess.h"
#include "sharedMemory.h"

int main(int argc, char * argv[])
{

	if(argc != 2)
	{
		fprintf(stderr, "View Failed: a lot of arguments.\n");
		exit(1);
	}
	int id = atoi(argv[1]);
	sharedMemoryADT shm = openShMem(id, O_RDONLY);

	fd_set rfd;
 	FD_ZERO( &rfd );
    FD_SET(getFd(shm), &rfd);

    char* buffer;
    do
    {
    	char newBuffer[MSG_SIZE + HASH_SIZE + 2];
    	buffer = newBuffer;
    	int result = select(getFd(shm) + 1, &rfd, 0, 0, NULL);
    	checkFail(result, "select() Failed");
		ssize_t readBytes = readShMem(shm, buffer , MSG_SIZE + HASH_SIZE + 2);
		checkFail(readBytes, "read() Failed");
		buffer[readBytes] = 0;
		if (readBytes > 0 && buffer[0] != (char)EOF)
		{
			printf("%s\n", newBuffer);
		}
	} 
	while(buffer[0] != (char)EOF);
	closeShMem(shm);
	return 0;
}
