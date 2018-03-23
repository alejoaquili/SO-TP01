# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
include Makefile.inc

TARGET   = SO-TP01

SOURCES  := $(wildcard *.c)
OBJECTS  := $(SOURCES:.c=*.o)
rm       = rm -f

all: clean $(TARGET) run
$(TARGET): obj
	@$(LINKER) $(TARGET) $(LFLAGS) $(OBJECTS)
	@echo "Linking complete."

obj: $(SOURCES) 
	@$(CC) $(CFLAGS) $(SOURCES)
	@echo "Compilation complete."

clean:
	@$(rm) $(TARGET) $(OBJECTS)
	@echo "Cleanup complete."

run:
	./$(TARGET)

.PHONY: clean  all run