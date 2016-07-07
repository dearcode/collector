#include "global.h"
#include "config.h"
#include "process.h"

extern operate_t *opts;

//每个连接进入之后会运行这个函数
int on_accept(void *dat)
{
	conn_t *conn = (conn_t *) dat;

	log_info("new request from:%s", conn->addr_str);

	return MRT_SUC;
}

int on_request(void *dat)
{
	conn_t *conn = (conn_t *) dat;
	string_t *req = string_create(MAX_LINE);
	buffer_t *buf = NULL;

	while ((buf = LIST_FIRST(conn, recv_bufs))) {
		if (string_catb(req, buf->data, buf->len) == MRT_ERR) {
			log_info("%x string_catb size:%jd error", conn->id, buf->len);
			return MRT_ERR;
		}

		buffer_cleanup(buf);
		LIST_REMOVE_HEAD(conn, recv_bufs);
	}

	buffer_t *file_buf = NULL;
	if (file_buffer_create(&file_buf, "/var/www/html/1.txt") == MRT_ERR) {
		log_error("file_buffer_create error");
		return MRT_ERR;
	}

	buffer_t *head_buf = NULL;
	if (buffer_create(&head_buf, MAX_LINE) == MRT_ERR) {
		log_error("buffer_create size:%d, error", MAX_LINE);
		return MRT_ERR;
	}

	if (buffer_cats(&head_buf,
			"HTTP/1.1 200 OK\r\nDate: Thu, 07 Jul 2016 15:40:21 GMT\r\nServer: Apache/2.4.6 (CentOS)\r\nContent-Length: %d\r\nConnection: Keep-Alive\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n",
			file_buf->size) == MRT_ERR) {
		log_error("buffer_cats error");
		return MRT_ERR;
	}

	LIST_INSERT_TAIL(conn, send_bufs, head_buf, node);

	LIST_INSERT_TAIL(conn, send_bufs, file_buf, node);

	return MRT_SUC;
}

int on_response(void *dat)
{
	conn_t *conn = (conn_t *) dat;
	log_debug("%x response ok", conn->id);

	return MRT_OK;
}

//每个连接关闭之前会运行这个函数
int on_close(void *dat)
{
	conn_t *conn = (conn_t *) dat;
	log_debug("%x close", conn->id);
	return MRT_SUC;
}
