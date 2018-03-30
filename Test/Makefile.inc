
CC       = gcc
# Compiling Flags:
CFLAGS   = -c -g -Wall -pedantic -std=c99 -I./include -I./../Utils/include -I./../Application/include -I./../Slave/include

LINKER 	 = gcc
# Linking Flags:
LFLAGS 	 = -g -Wall -pedantic -std=c99 -lpthread -pthread


