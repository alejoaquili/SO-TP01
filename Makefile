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
	@echo "To run the program please enter: <sudo ./Application/applicationProcess.out><file1, file2, ..., fileN>\n"

test:
	cd Utils; make all
	cd Test; make all
	
.PHONY: utils application slave view clean test all
