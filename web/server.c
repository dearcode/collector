#include "macro_const.h"
#include "macro_func.h"
#include "global.h"
#include "config.h"
#include "process.h"

config_t server;

int g_loglevel;

int server_init(int argc, char *argv[])
{
	char *var = NULL;
	callback_t cb_accept, cb_request, cb_response, cb_close;

	callback_set(cb_accept, on_accept);
	callback_set(cb_request, on_request);
	callback_set(cb_response, on_response);
	callback_set(cb_close, on_close);

	if (!(var = getenv(HOME_ENV))) {
		printf("%s no found Environment Variable:%s\n", __func__, HOME_ENV);
		return MRT_ERR;
	}

	if (chdir(var)) {
		printf("%s chdir(%s) error:%m\n", __func__, var);
		return MRT_ERR;
	}

	strcpy(server.home, var);

	if (parse_cmd(argc, argv) == MRT_ERR) {
		printf("%s parase command line error.\n", __func__);
		return MRT_ERR;
	}

	if (memory_pool_init() == MRT_ERR) {
		printf("%s memory_pool_init error.\n", __func__);
		return MRT_ERR;
	}

	if (load_config(var) == MRT_ERR) {
		printf("%s load profile error.\n", __func__);
		return MRT_ERR;
	}

	if (logger_init("./log/", "account_center", server.log_level) == MRT_ERR) {
		printf("%s logger_init error.\n", __func__);
		return MRT_ERR;
	}

	if (event_center_init(server.conn_max, server.conn_timeout, server.host, server.port, cb_accept, cb_request, cb_response, cb_close) == MRT_ERR) {
		log_error("init event center error.");
		return MRT_ERR;
	}

	if (factory_init(server.worker_max, server.worker_min) == MRT_ERR) {
		printf("factory_init error\n");
		return MRT_ERR;
	}

	if (server.daemon == 1) {
		//如果daemon_home有值就切换到daemon_home目录，否则以当前目录为中心
		if (daemon_init(server.home ? server.home : "./")) {
			printf("daemon init error:%m\n");
			return MRT_ERR;
		}
	}

	return MRT_OK;
}

int main(int argc, char *argv[])
{
	s_zero(server);

	if (server_init(argc, argv) == MRT_ERR) {
		printf("%s load config error.\n", __func__);
		return MRT_ERR;
	}

	if (factory_start() == MRT_ERR) {
		printf("factory_start error\n");
		return MRT_ERR;
	}

	event_loop();

	return MRT_SUC;
}
