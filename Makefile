# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
all: clean utils application slave view run

utils: 
	cd Utils; make all

application:
	cd Application; make all

slave:
	cd Slave; make all

view: 	
	cd View; make all

clean:
	cd Application; make clean
	cd Slave; make clean
	cd Utils; make clean
	cd View; make clean
	cd Test; make clean

run: 
	@chmod 777 ./run.sh
	@chmod 777 ./init.sh
	@echo "Running"
	@./run.sh

test:
	cd Test; make all
	
.PHONY: utils application slave view clean test all
