all: init cwind

CC=gcc

INCLUDES=-I. -I../lib/ $(shell mysql_config --include)
CFLAGS:= -Wall -g $(INCLUDES)

ifeq (true,$(DEBUG))
CFLAGS+=-D__DEBUG__
LDFLAGS+=-D__DEBUG__
endif

LIBS=../lib-html/libhtml.a ../lib/libmrt.a 
LDFLAGS+= $(shell mysql_config --libs) -rdynamic -liconv $(LIBS)


ifeq (FETCH,$(DEBUG)) 
LDFLAGS+=-DFETCH_TEST
endif

SRCDIRS=.

.PHONY:init cwind


SRCS=comfunc.c  db.c  fetch_web.c  server.c

OBJS=$(SRCS:.c=.o)



init:
	@mkdir -p ../bin/

cwind: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o ../bin/$@ $(filter %.o ,$+) $(LDFLAGS) 

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $+ 

clean:
	rm -rf $(OBJS) ../bin/cwind


cl:clean
