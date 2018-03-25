# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
include Makefile.inc

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
rm       = rm -rf


all: clean obj

obj:$(SOURCES)
	$(CC) -c $(CFLAGS) $(SOURCES) -lrt
	@echo "Utils Compilation complete."

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ -lrt

clean:
	@$(rm) $(OBJECTS)
	@echo "Utils Cleanup complete."

.PHONY: clean  all obj 
