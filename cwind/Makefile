CC=gcc


INCLUDES=-I. -I../lib/ $(shell mysql_config --include)
CFLAGS:= -Wall -g $(INCLUDES)

LIBS=../lib-html/libhtml.a ../lib/libmrt.a 
LDFLAGS= $(shell mysql_config --libs) -rdynamic -liconv $(LIBS)

ifeq (FETCH,$(DEBUG)) 
LDFLAGS+=-DFETCH_TEST
endif


SRCDIRS=.
PROGRAMS=cwind


SRCS=comfunc.c  cwind_db.c  fetch_web.c  server.c

OBJS=$(SRCS:.c=.o)


all: $(PROGRAMS)


$(PROGRAMS): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(filter %.o ,$+) $(LDFLAGS) 

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $+ 

clean:
	rm -rf $(OBJS) $(PROGRAMS) a.out


cl:clean
