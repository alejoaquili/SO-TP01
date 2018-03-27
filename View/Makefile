# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
include Makefile.inc

TARGET := viewProcess
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
OUTPUTS := $(SOURCES:.c=.out)
rm       = rm -rf


all: clean comp link

comp:$(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -lrt
	@echo "View Compilation complete."

link:$(OBJECTS)
	$(LINKER) $(LFLAGS) $(OBJECTS) ./../Utils/*.o -o $(TARGET).out -lrt
	@echo "View Linking complete."

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@ -lrt

%.out : %.o
	$(LINKER) $(LFLAGS) $< -o $@ -lrt

clean:
	@$(rm) $(OUTPUTS)
	@echo "View Cleanup complete."

.PHONY: clean  all obj  
