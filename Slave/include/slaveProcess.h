#ifndef SALVE_PROCESS_H
#define SALVE_PROCESS_H

#define MD5SUM_LENGTH 7

void hashTheFile(char * fileToHash, char * buffer);

void md5sum(char * fileToHash);

void childProcess(int * fd, char * fileToHash);

void parentProcess(int * fd, char * fileToHash, char* buffer);

int checkQueueIsEmpty(mqd_t mqDescriptor, ssize_t bytesRead);

void sendTheHash(char * hashedFile);

#endif