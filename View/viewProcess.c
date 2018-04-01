#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include "errorslib.h"
#include "applicationProcess.h"
#include "sharedMemory.h"
#include "processlib.h"

void printTheHash(char* buffer, int readBytes);
int readTheNextHash(sharedMemoryADT shm, char* buffer, fd_set rfd);

int main(int argc, char * argv[])
{
	checkAreEquals(argc, 2, "View Failed: invalid number of arguments");

	int id = atoi(argv[1]);
	sharedMemoryADT shm = openShMem(id, O_RDONLY);

	fd_set rfd = necesitoUnNombreParaEstaFuncion(getShMemFd(shm));

    char* buffer;
    do
    {
		char newBuffer[MSG_SIZE + HASH_SIZE + 2];
    	buffer = newBuffer;
    	int readBytes = readTheNextHash(shm, buffer, rfd);

    	printTheHash(buffer, readBytes);
	}
	while(buffer[0] != (char)EOF);
	closeShMem(shm);
	return 0;
}

int readTheNextHash(sharedMemoryADT shm, char* buffer, fd_set rfd)
{
    waitForOtherProcess(getShMemFd(shm), rfd);
	ssize_t readBytes = readShMem(shm, buffer , MSG_SIZE + HASH_SIZE + 2);
	checkFail(readBytes, "read() Failed");
	buffer[readBytes] = 0;

	return readBytes;
}

void printTheHash(char* buffer, int readBytes)
{
	if (readBytes > 0 && buffer[0] != (char)EOF)
	{
		printf("%s\n", buffer);
	}
}