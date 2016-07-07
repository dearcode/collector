#ifndef __BUFFER_H__
#define __BUFFER_H__

#define BUFFER_INIT     1
#define BUFFER_CREATE   2
#define BUFFER_FILE     3

typedef struct {
	int type;		// buffer的类型，BUFFER_INIT的或者是BUFFER_CREATE, BUFFER_FILE
	int fd;			//当前类型为BUFFER_FILE为文件fd

	void *data;		//缓冲区, init是单独分配的data空间，create出来的buffer不能为data重新分配空间

	size_t size;		//分配的空间大小
	size_t len;		//当前数据大小
	size_t pos;		//读指针，指向当前读位置

	int auto_pop;		//如果为1，在缓冲区满后自动调用pop_func将数据推出
	int (*pop_func) (int, void *, size_t);

	list_node_t node;
} buffer_t;

int buffer_create(buffer_t ** rbuf, int size);

int buffer_init(buffer_t * buf, int size);

void buffer_cleanup(buffer_t * buf);

//如果buffer的剩余空间大于size就直接添加到尾部
//否则把data复制到剩余空间中之后修改size为data中剩余大小
//完成后修改wpos指针,已用缓冲大小
int buffer_push(buffer_t * buf, void *data, int *size);

int buffer_clear(buffer_t * buf);

int buffer_read(int fd, buffer_t * buf);

int buffer_write(int fd, buffer_t * buf);

//从头开始写入内容
int buffer_printf(buffer_t ** buf, const char *fmt, ...);

//从上次写入的尾部开始写入内容
int buffer_cats(buffer_t ** buf, const char *fmt, ...);

#endif
