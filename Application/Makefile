# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
include Makefile.inc

TARGET := applicationProcess
SOURCES := $(wildcard *.c)
OUTPUTS := $(SOURCES:.c=.out)
rm       = rm -rf


all: clean obj 

obj:$(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET).out -lrt
	@echo "Application Compilation complete."

%.out : %.c
	$(CC) $(CFLAGS) $< -o $@ -lrt

clean:
	@$(rm) $(OUTPUTS)
	@echo "Application Cleanup complete."

.PHONY: clean  all obj 