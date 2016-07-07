#include "global.h"

int file_buffer_create(buffer_t ** b, char *path)
{
	buffer_t *buf = M_alloc(sizeof(*buf));
	if (!buf) {
		log_error("create buffer error size:%zd", sizeof(*buf));
		return MRT_ERR;
	}

	int fd = open(path, O_RDONLY);
	if (fd == MRT_ERR) {
		log_error("open file:%s error:%m", path);
		return MRT_ERR;
	}

	buf->type = BUFFER_FILE;
	buf->data = buf + 1;
	buf->fd = fd;
	buf->pos = 0;
	buf->size = fd_file_size(fd);
	buf->len = buf->size;

	*b = buf;

	log_debug("open file:%s size:%jd", path, buf->size);

	return MRT_SUC;

}

int file_buffer_init(file_handle_t * file, int size)
{
	file->buffer = M_alloc(sizeof(buffer_t));
	if (!file->buffer) {
		log_error("file buffer create error, size:%d", size);
		return MRT_ERR;
	}

	return buffer_init(file->buffer, size);
}

void file_buffer_deinit(file_handle_t * file)
{
	if (file)
		if (file->buffer) {
			file_buffer_flush(file);
			buffer_cleanup(file->buffer);
			file->buffer = NULL;
		}
}

int file_buffer_flush(file_handle_t * file)
{
	if (file_write_loop(file->fd, file->buffer->data, file->buffer->len)) {
		log_error("write file error:%m");
		return MRT_ERR;
	}
//    log_debug("write file:%s size:%u", file->from, file->buffer->len);

	buffer_clear(file->buffer);

	return MRT_SUC;
}

int file_buffer_write(file_handle_t * file, void *data, int size)
{
	int dsize = size;

	if (buffer_push(file->buffer, data, &dsize) == MRT_ERR) {
		log_error("buffer append error.");
		return MRT_ERR;
	}
	//如果size不为0，说明buffer已满, 需要将buffer写到磁盘上
	while (dsize > 0) {
		if (file_buffer_flush(file) == MRT_ERR) {
			log_error("file:(%s) flush error.", file->from);
			return MRT_ERR;
		}

		if (buffer_push(file->buffer, (data + (size - dsize)), &dsize) == MRT_ERR) {
			log_error("buffer append error.");
			return MRT_ERR;
		}
	}

	//   log_debug("file:(%s) write buffer success, data size:%d", file->from, size);

	return MRT_SUC;
}
