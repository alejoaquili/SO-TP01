# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
include Makefile.inc

TARGET := slaveProcess
SOURCES := $(wildcard *.c)
OUTPUTS := $(SOURCES:.c=.out)
rm       = rm -rf


all: clean obj

obj:$(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET).out -lrt
	@echo "Slave Compilation complete."

%.out : %.c
	$(CC) $(CFLAGS) $< -o $@ -lrt

clean:
	@$(rm) $(OUTPUTS)
	@echo "Slave Cleanup complete."

.PHONY: clean  all obj 