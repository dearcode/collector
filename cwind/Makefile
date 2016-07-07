CC=gcc
CFLAGS=-Wall -g
LD=gcc
LIBS=-L/usr/lib/mysql/ -L../lib/ -L../lib-html/  -L/usr/local/mysql/lib -L/usr/lib64/mysql
LDFLAGS= $(LIBS) -lmysqlclient  -lmrt -lhtml -lmrt -rdynamic -liconv -lpthread -lcrypto
ifeq (FETCH,$(DEBUG))
LDFLAGS+=-DFETCH_TEST
endif
DEPENDFLAG=-MM
INCLUDES=-I. -I../lib/ $(shell mysql_config --include)

CFLAGS:=$(CFLAGS) $(INCLUDES)
LDFLAGS:=$(LDFLAGS) 


SRCDIRS=.
PROGRAMS=cwind


SRCS=comfunc.c  cwind_db.c  fetch_web.c  server.c

OBJS=$(SRCS:.c=.o)


all: $(PROGRAMS)


$(PROGRAMS): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(filter %.o ,$+) $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $+ 

clean:
	rm -rf $(OBJS) $(PROGRAMS) a.out


cl:clean
