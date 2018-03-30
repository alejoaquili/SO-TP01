# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
include Makefile.inc

TARGET := testSO-TP01
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
OUTPUTS := $(SOURCES:.c=.out)
rm       = rm -rf


all: clean comp link run

comp:$(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -lrt
	@echo "Test Compilation complete."

link:$(OBJECTS)
	$(LINKER) $(LFLAGS) $(OBJECTS) ./../Utils/*.o  ./../Application/*.o ./../Slave/*.o  ./../View/*.o -o $(TARGET).out -lrt
	@echo "Test Linking complete."

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@ -lrt

%.out : %.o
	$(LINKER) $(LFLAGS) $< -o $@ -lrt

clean:
	@$(rm) $(OUTPUTS)
	@$(rm) $(OBJECTS)
	@echo "Test Cleanup complete."

run:
	@echo "Test Running"
	@./testSO-TP01.out

.PHONY: clean  all obj run
