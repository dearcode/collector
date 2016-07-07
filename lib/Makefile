CC=gcc
ifeq (MEM,$(DEBUG))
CFLAGS=-Wall -g -lcrypto -DMEMORY_DEBUG -rdynamic
PROGRAMS=libmrt.a
else ifeq (CONF, $(DEBUG))
CFLAGS=-Wall -g -lcrypto -DCONF_TEST
PROGRAMS=conf_test
else ifeq (HASH, $(DEBUG))
CFLAGS=-Wall -g -lcrypto -DHASH_TABLE_TEST
PROGRAMS=hash_test
else ifeq (COMM, $(DEBUG))
CFLAGS=-Wall -g -lcrypto -DCOMMON_TEST
PROGRAMS=comm_test
else ifeq (BINMAP, $(DEBUG))
CFLAGS=-Wall -g -lcrypto -DBINMAP_TEST
PROGRAMS=binmap_test
else ifeq (FILE, $(DEBUG))
CFLAGS=-Wall -g -lcrypto -DFILE_FUNC_TEST
PROGRAMS=file_test
else
CFLAGS=-Wall -g -rdynamic
PROGRAMS=libmrt.a
endif
INCL=-I. $(shell mysql_config --include)
LIB= 

LDFLAGS=-lpthread -ldl -lm -lrt -rdynamic

SRCDIRS=.

SRCS= mem_pool.c socket_func.c string.c strutil.c charset.c \
	  comm_func.c file_func.c hashmap.c hashdb.c binmap.c ini_func.c logger.c \
	  base64.c xxtea.c mysql_func.c factory.c event_center.c \
	  buffer.c file_buffer.c

OBJS=$(SRCS:.c=.o)

all: $(PROGRAMS) 


libmrt.a: $(OBJS)
	ar cru $@ $(OBJS)
	ranlib $@
	rm -f *.o

comm_test: string_func.o logger.o hashmap.o common.o mem_pool.o
	$(CC) $(CFLAGS) -o $@ $(filter %.o ,$+) $(LDFLAGS)

conf_test: conf_func.o string_func.o logger.o hashmap.o common.o mem_pool.o
	$(CC) $(CFLAGS) -o $@ $(filter %.o ,$+) $(LDFLAGS)

mem_test: mem_pool.o string_func.o logger.o comm_func.o
	$(CC) $(CFLAGS) -o $@ $(filter %.o ,$+) $(LDFLAGS)

hash_test: hashmap.o mem_pool.o string_func.o logger.o
	$(CC) $(CFLAGS) -o $@ $(filter %.o ,$+) $(LDFLAGS)

file_test: file_func.o hashmap.o mem_pool.o string_func.o logger.o common.o
	$(CC) $(CFLAGS) -o $@ $(filter %.o ,$+) $(LDFLAGS)

binmap_test: binmap.o mem_pool.o logger.o 
	$(CC) $(CFLAGS) -o $@ $(filter %.o ,$+) $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) $(INCL) -c -o $@ $+

clean:
	$(RM) $(OBJS) $(PROGRAMS) mem_test hash_test file_test core comm_test

cl:clean

