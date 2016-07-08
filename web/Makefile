CC=gcc
LIBS= -L../lib -L../lib-html $(shell mysql_config --libs) -lpthread
DEPENDFLAG=-g -Wall 
#-rdynamic
INCLUDES=-I. -I../lib/ $(shell mysql_config --include)

CFLAGS:=$(CFLAGS) $(INCLUDES)  -ggdb3

ifeq (TRUE,$(DBG))
		LDFLAGS= $(LIBS) -lmysqlclient -lcrypto -lpthread -lmrt $(DEPENDFLAG) -DDBUG
else ifeq (TRUE, $(PFMC))
		LDFLAGS= $(LIBS) -lmysqlclient -lcrypto -lpthread -lmrt $(DEPENDFLAG) 
else 
		LDFLAGS= $(LIBS) -lmysqlclient -lcrypto -lpthread -lmrt $(DEPENDFLAG) -rdynamic
endif


SRCDIRS=.
PROGRAMS=account_center

#SRCS=$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
SRCS=config.c  process.c  server.c

OBJS=$(SRCS:.c=.o)


all: $(PROGRAMS)

$(PROGRAMS): $(OBJS)
	$(CC) -o $@ $(filter %.o ,$+) $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -c $+ $(DEPENDFLAG)

clean:
	rm -rf $(OBJS) $(PROGRAMS)

cl:clean

