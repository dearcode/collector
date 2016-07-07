
SRCDIRS=lib lib-html cwind



default: cwind_exec

cwind_exec:
	for SUBDIR in $(SRCDIRS); do \
			cd $$SUBDIR; make DEBUG=$(DEBUG); cd ..; \
			done


clean:
	for SUBDIR in $(SRCDIRS); do \
			cd $$SUBDIR; make clean; cd ..; \
			done

cl:clean
