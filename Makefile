# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
all: application slave run

application:
	cd Application; make all

slave:
	cd Slave; make all

clean:
	cd Application; make clean
	cd Slave; make clean

run: 
	@chmod 777 ./run.sh
	@echo "Running"
	@./run.sh
	
.PHONY: application slave clean all