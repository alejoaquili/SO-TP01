
CC       = gcc
# Compiling Flags:
CFLAGS   = -c -Wall -pedantic -std=c99 -I./include -I./../Utils/include -I./../Application/include -I./../Slave/include

LINKER 	 = gcc
# Linking Flags:
LFLAGS 	 = -Wall -pedantic -std=c99 -lpthread -pthread


