#ifndef PROCESS_LIB_H
#define PROCESS_LIB_H

pid_t * childFactory(int qty, char* childName);

pid_t * childFactoryWithArgs(int qty, char* childPath, char** args);

fd_set necesitoUnNombreParaEstaFuncion(int fd);

void waitForOtherProcess(int fd, fd_set rfd);

#endif

