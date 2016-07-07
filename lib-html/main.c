#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "global.h"

int main(int argc, char *argv[])
{
	if (logger_init("./var/log/", "cwind", 1) == MRT_ERR) {
		printf("%s init logger error.\n", __func__);
		return MRT_ERR;
	}

	if (memory_pool_init() == MRT_ERR) {
		printf("%s:%d mem pool init error.\n", __func__, __LINE__);
		return MRT_ERR;
	}

	html_page_t page = { 0 };
	char url[1000] = { "http://baidu.com/" };
	printf("recv:%d\n", http_recv_page(url, &page, 1));
	printf("data:%s\n", page.data.str);

	string_t *new_path = string_new(1024, "");
	http_recv_file("http://tu.nixiba.com/picture/2014/07/25/3eb2d0b483e8fdf00685624f152c26b5.jpg", "", new_path);
	printf("file:%s\n", new_path->str);

	return MRT_OK;
}
