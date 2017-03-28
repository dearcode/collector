all: lib lib-html collector

.PHONY: lib lib-html collector


lib:
	cd lib; make DEBUG=$(DEBUG); cd ..; 

lib-html:
	cd lib-html; make DEBUG=$(DEBUG); cd ..; 

collector:
	cd server; make DEBUG=$(DEBUG); cd ..; 


clean:
	cd server; make clean; cd ..; 
	cd lib; make clean; cd ..; 
	cd lib-html; make clean; cd ..; 

cl:clean
