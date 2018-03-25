# ------------------------------------------------
# Generic Makefile
#
# Author: aaquili@itba.edu.ar
# Date  : 23-03-2018
# ------------------------------------------------
RM:=	rm -f
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
	$(RM) ./Fifos/*
	@chmod 777 ./run.sh
	@echo "Running"
	@./run.sh
	
.PHONY: util application slave clean all
