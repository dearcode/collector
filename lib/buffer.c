#include "global.h"
#include <stdarg.h>

int buffer_create(buffer_t ** rbuf, int size)
{
	buffer_t *buf = M_alloc(sizeof(*buf) + size);
	if (!buf) {
		log_error("create buffer error size:%zd", sizeof(*buf) + size);
		return MRT_ERR;
	}

	buf->type = BUFFER_CREATE;
	buf->data = buf + 1;
	buf->len = 0;
	buf->pos = 0;
	buf->size = size;

	*rbuf = buf;

	return MRT_SUC;
}

int buffer_init(buffer_t * buf, int size)
{
	buf->data = M_alloc(size);
	if (!buf->data) {
		log_error("create buffer error size:%d", size);
		return -1;
	}

	buf->type = BUFFER_INIT;
	buf->len = 0;
	buf->pos = 0;
	buf->size = size;

	return MRT_SUC;
}

//如果buffer的剩余空间大于size就直接添加到尾部
//否则把data复制到剩余空间中之后修改size为data中剩余大小
//完成后修改pos指针,已用缓冲大小
int buffer_push(buffer_t * buf, void *data, int *size)
{
	int len = 0;

	if ((*size + buf->len) > buf->size)
		len = buf->size - buf->len;
	else
		len = *size;

	memcpy(buf->data + buf->len, data, len);

	buf->len += len;
	*size -= len;

	return MRT_SUC;
}

int buffer_clear(buffer_t * buf)
{
	buf->pos = 0;
	buf->len = 0;

	memset(buf->data, 0, buf->size);

	return MRT_SUC;
}

int buffer_read(int fd, buffer_t * buf)
{
	int rlen = 0;

	while (buf->len < buf->size) {

		if ((rlen = recv(fd, buf->data + buf->len, buf->size - buf->len, 0)) == MRT_ERR) {
			if (errno == EAGAIN)
				break;

			if (errno == EINTR)
				continue;

			log_debug("read info:[%d:%m] recv:%jd", errno, buf->len);
			return MRT_ERR;
		}

		if (rlen == 0) {
			log_error("fd:%d close.", fd);
			break;
		}
		buf->len += rlen;
	}

	log_debug("read fd:%d, size:%jd", fd, buf->len);

	return buf->len;
}

int buffer_write(int fd, buffer_t * buf)
{
	int slen = 0;
	int ssize = 0;

	M_cpvril(buf);

	while (ssize != buf->len) {
		if ((slen = write(fd, buf->data + ssize, buf->len - ssize)) == MRT_ERR) {
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)	//kernel send cache is full
				continue;

			return MRT_ERR;
		}

		ssize += slen;
	}

	log_debug("write fd:%d, size:%d", fd, ssize);

	return MRT_SUC;
}

int buffer_realloc(buffer_t ** buf, int new_size)
{
	buffer_t *oldbuf = *buf;
	buffer_t *newbuf;

	if (buffer_create(&newbuf, new_size) == MRT_ERR) {
		log_error("create new buffer error:%m, size:%d", new_size);
		return MRT_ERR;
	}

	newbuf->size = new_size;
	newbuf->pos = oldbuf->pos;
	newbuf->len = oldbuf->len;

	memcpy(newbuf->data, oldbuf->data, oldbuf->len);

	*buf = newbuf;

	buffer_cleanup(oldbuf);

	return MRT_OK;
}

int buffer_printf(buffer_t ** buf, const char *fmt, ...)
{
	va_list ap;
	int size;

	va_start(ap, fmt);
	size = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	size++;

	if ((*buf)->size < size) {
		buffer_realloc(buf, size);
	}

	va_start(ap, fmt);
	(*buf)->len = vsnprintf((*buf)->data, size, fmt, ap);
	va_end(ap);

	return (*buf)->len;
}

int buffer_cats(buffer_t ** buf, const char *fmt, ...)
{
	va_list ap;
	int size;

	va_start(ap, fmt);
	size = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	size++;

	if (((*buf)->size - (*buf)->len) < size) {
		buffer_realloc(buf, (*buf)->len + size);
	}

	va_start(ap, fmt);
	(*buf)->len += vsnprintf((*buf)->data + (*buf)->len, size, fmt, ap);
	va_end(ap);

	return (*buf)->len;
}

void buffer_cleanup(buffer_t * buf)
{
	switch (buf->type) {
	case BUFFER_INIT:
		M_free(buf->data);
		break;
	case BUFFER_CREATE:
		M_free(buf);
		break;
	case BUFFER_FILE:
		close(buf->fd);
		M_free(buf);
		break;
	}
}
