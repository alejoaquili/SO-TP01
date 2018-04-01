# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
include Makefile.inc

TARGET := slaveProcess
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
OUTPUTS := $(SOURCES:.c=.out)
rm       = rm -rf


all: clean comp link

comp:$(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -lrt
	@echo "Slave Compilation complete."

link:$(OBJECTS)
	$(LINKER) $(LFLAGS) $(OBJECTS) ./../Utils/*.o -o $(TARGET).out -lrt -lpthread
	@echo "Slave Linking complete."

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@ -lrt

%.out : %.o
	$(LINKER) $(LFLAGS) $< -o $@ -lrt

clean:
	@$(rm) $(OBJECTS)
	@$(rm) $(OUTPUTS)
	@echo "Slave Cleanup complete."

.PHONY: clean  all obj  
