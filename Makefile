all: cwind

SRCDIRS=lib lib-html cwind

.PHONY:cwind


cwind:
	for SUBDIR in $(SRCDIRS); do \
		cd $$SUBDIR; make DEBUG=$(DEBUG); cd ..; \
		done


clean:
	for SUBDIR in $(SRCDIRS); do \
		cd $$SUBDIR; make clean; cd ..; \
		done

cl:clean
