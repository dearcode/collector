all: lib lib-html cwind

.PHONY: lib lib-html cwind


lib:
	cd lib; make DEBUG=$(DEBUG); cd ..; 

lib-html:
	cd lib-html; make DEBUG=$(DEBUG); cd ..; 

cwind:
	cd cwind; make DEBUG=$(DEBUG); cd ..; 


clean:
	cd cwind; make clean; cd ..; 
	cd lib; make clean; cd ..; 
	cd lib-html; make clean; cd ..; 

cl:clean
