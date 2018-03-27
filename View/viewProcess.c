#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h>
#include "errorslib.h"
#include "applicationProcess.h"

int main(int argc, char * argv[])
{
	int shmFd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
	checkFail(shmFd, "shm_open Failed");
	int size = ((MSG_SIZE + HASH_SIZE + 2) * (argc-1))+ MAX_PID_LENGTH;
	void * shmPointer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);


	//semaforo


	char buffer[MSG_SIZE + HASH_SIZE + 2];
	read(shmFd, buffer, sizeof(buffer));
	printf("%s\n", buffer);



	shm_unlink(SHARED_MEMORY_NAME);


}

