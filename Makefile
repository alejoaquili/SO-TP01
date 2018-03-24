# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
include Makefile.inc

SOURCES  := $(wildcard *.c)
OBJECTS  := $(SOURCES:.c=*.o)
rm       = rm -f

all: clean obj run


obj: $(SOURCES) 
	@$(CC) $(CFLAGS) $(SOURCES)
	@echo "Compilation and Linking complete."

clean:
	@$(rm) $(TARGET) $(OBJECTS)
	@echo "Cleanup complete."

run:
	./applicationProcess.out c.out true.out p.c

.PHONY: clean  all obj run