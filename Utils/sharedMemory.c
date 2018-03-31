
typedef struct sharedMemoryCDT {
	sem_t* semaphore;
	void* pointer;
	int pid;
	int fd;
} sharedMemoryCDT;


sharedMemoryADT sharedMemoryCreator(const char* name, const long memSize)
{	
	sharedMemoryADT shm = malloc(sizeof(sharedMemoryCDT));
	
	char* semName = calloc(MAX_PID_LENGTH+4, sizeof(char));
	sprintf(semName, "/sem%d", getpid());
	shm->semaphore = createSemaphore(shmName);
//create shm name

	char shmName[MAX_PID_LENGTH+4];
	sprintf(shmName, "/shm%d", getpid());

//create SHM 
	shm->fd = shm_open(shmName, O_RDWR | O_CREAT, 0777);
	checkFail(shm->fd, "shm_open() Failed");
	shm->pointer = mmap(0, memSize, PROT_READ | PROT_WRITE, MAP_SHARED, shm->fd, 0);
	checkIsNotNull(shm->pointer," Null shmPointer");
	
	return shm;
}

void deleteShmMem(sharedMemoryADT shm)
{
	munmap(shm->pointer, size);
	shm_unlink(shmName);
	sem_unlink(semName);
	free(shm);
}

sem_t* createSemaphore(char* semName)
{
	sem_unlink(semName);
	return sem_open(semName, O_CREAT|O_EXCL, 0777, 1);
}

ssize_t writeShmMem(sharedMemoryADT shm, const void* buffer , size_t nbytes)
{
	sem_wait(shm->semaphore);
	ssize_t result = write(shm->fd, buffer, nbytes);
	sem_post(shm->semaphore);
	return result;
}

ssize_t readShmMem(sharedMemoryADT shm, const void* buffer , size_t nbytes)
{
	sem_wait(shm->semaphore);
	ssize_t result = read(shm->fd, buffer, nbytes);
	sem_post(shm->semaphore);
	return result;
}

