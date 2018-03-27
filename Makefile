# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
all: clean utils application slave run

utils: 
	cd Utils; make all

application:
	cd Application; make all

slave:
	cd Slave; make all

clean:
	cd Application; make clean
	cd Slave; make clean

run: 
	@chmod 777 ./run.sh
	@chmod 777 ./init.sh
	@echo "Running"
	@./run.sh
	
.PHONY: utils application slave clean all
