#ifndef APPLICATION_PROCESS_H
#define APPLICATION_PROCESS_H

#define SLAVE_QTY 5
#define FILE_SIZE 256
#define HASH_SIZE 32
#define MSG_SIZE FILE_SIZE + HASH_SIZE + 2
#define QUEUE_FILE_NAME  "/filesToHash"
#define QUEUE_HASH_STORAGE  "/hashToStorage"
#define SLAVE_PATH "./Slave/slaveProcess.out"
#define MAX_PID_LENGTH 10

#endif

