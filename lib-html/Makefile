CC=gcc
ifeq (HTML,$(DEBUG))
CFLAGS=-Wall -g -lcrypto -DHTML_TEST
PROGRAMS=html_test
else
CFLAGS=-Wall -g 
PROGRAMS=libhtml.a
endif
INCL=-I../include/ -I/usr/include/mysql/ -I/usr/local/mysql/include
LIB= -L../lib

LDFLAGS=-lpthread -ldl -lm -lrt -lmrt -pg

SRCDIRS=.

SRCS= html_func.c http_func.c

OBJS=$(SRCS:.c=.o)

all: $(PROGRAMS) 


libhtml.a: $(OBJS)
	ar cru $@ $(OBJS)
	ranlib $@

test: html_func.c http_func.c main.c
	$(CC) $(INCL) $(LIB) $(CFLAGS) -o $@ $+ $(LDFLAGS) -lcrypto  -lmrt -lmrt -rdynamic -liconv -lpthread

%.o:%.c
	$(CC) $(CFLAGS) $(INCL) -c -o $@ $+

clean:
	$(RM) $(OBJS) $(PROGRAMS) html_test

cl:clean

