#include "global.h"
#include "config.h"

extern config_t server;

int load_userdb_info(hashmap_t * hmp)
{
	char *pstr = NULL;

	if ((pstr = ini_get_str(hmp, "userdb", "ip", NULL)) == NULL) {
		set_error("no found userdb ip");
		return MRT_ERR;
	}
	sprintf(server.userdb.ip, "%s", pstr);

	server.userdb.port = ini_get_int(hmp, "userdb", "port", -1);
	if (server.userdb.port == MRT_ERR) {
		set_error("no found userdb port");
		return MRT_ERR;
	}

	if ((pstr = ini_get_str(hmp, "userdb", "name", NULL)) == NULL) {
		set_error("no found userdb name");
		return MRT_ERR;
	}
	sprintf(server.userdb.name, "%s", pstr);

	if ((pstr = ini_get_str(hmp, "userdb", "user", NULL)) == NULL) {
		set_error("no found userdb user");
		return MRT_ERR;
	}
	sprintf(server.userdb.user, "%s", pstr);

	if ((pstr = ini_get_str(hmp, "userdb", "pass", NULL)) == NULL) {
		set_error("no found userdb pass");
		return MRT_ERR;
	}
	sprintf(server.userdb.pass, "%s", pstr);

	return MRT_SUC;
}

int load_server_info(hashmap_t * hmp)
{
	char *pstr = NULL;

	server.log_level = ini_get_int(hmp, "server", "log_level", 2);

	server.conf_sync_time = ini_get_int(hmp, "server", "conf_sync_time", -1);
	if (server.conf_sync_time == MRT_ERR) {
		set_error("no found conf_sync_time");
		return MRT_ERR;
	}

	if ((server.conn_timeout = ini_get_int(hmp, "server", "conn_timeout", -1)) == MRT_ERR) {
		set_error("no found conn_timeout");
		return MRT_ERR;
	}
	//如果启动命令中指定了IP就不从配置文件 读IP
	if (!*server.host) {
		if ((pstr = ini_get_str(hmp, "server", "host", NULL)) == NULL) {
			set_error("no found host");
			return MRT_ERR;
		}
		snprintf(server.host, sizeof(server.host), pstr);
	}

	if (!server.port) {
		if ((server.port = ini_get_int(hmp, "server", "port", -1)) == MRT_ERR) {
			set_error("no found server port");
			return MRT_ERR;
		}
	}

	if ((server.conn_timeout = ini_get_int(hmp, "server", "conn_timeout", -1)) == MRT_ERR) {
		set_error("no found read_timeout");
		return MRT_ERR;
	}

	if ((server.conn_max = ini_get_int(hmp, "server", "conn_max", -1)) == MRT_ERR) {
		set_error("no found max_connect");
		return MRT_ERR;
	}

	if ((server.worker_max = ini_get_int(hmp, "server", "worker_max", -1)) == MRT_ERR) {
		set_error("no found worker_max");
		return MRT_ERR;
	}

	if ((server.worker_min = ini_get_int(hmp, "server", "worker_min", -1)) == MRT_ERR) {
		set_error("no found worker_min");
		return MRT_ERR;
	}

	return MRT_SUC;
}

int load_profile_first(char *file)
{
	hashmap_t hmp;
	s_zero(hmp);

	if (ini_file_load(file, &hmp) == MRT_ERR) {
		printf("%s load profile error, file:%s, error:%s\n", __func__, file, get_error());
		return MRT_ERR;
	}

	if (load_server_info(&hmp) == MRT_ERR) {
		printf("%s load server info error, file:%s, error:%s\n", __func__, file, get_error());
		return MRT_ERR;
	}

	if (load_userdb_info(&hmp) == MRT_ERR) {
		printf("%s load userdb info error, file:%s, error:%s\n", __func__, file, get_error());
		return MRT_ERR;
	}

	ini_file_unload(&hmp);

	return MRT_OK;
}

void server_usage(char *pname)
{
	printf("Usage: %s [-h host -p port] [-d] [--help] [--version]\n"
	       "\n  -h host\tbind ip."
	       "\n  -p port\tbind port." "\n  -d     \trun as daemon.\n" "\n  --version\tdisplay versioin infomation." "\n  --help \tdisplay this message.\n", pname);
	exit(1);
}

int parse_cmd(int argc, char *argv[])
{
	const char *options = "h:p:d";
	int opt;

	if (argc == 2) {
		if (!strncasecmp(argv[1], "--help", 6)) {
			server_usage(argv[0]);
		}

		if (!strncasecmp(argv[1], "--version", 9)) {
			printf("%s version %s, build time %s %s\n", argv[0], VERSION, __DATE__, __TIME__);
			exit(0);
		}
	}

	if (pthread_mutex_init(&server.conf_mtx, NULL) != 0) {
		log_error("%s init load mutex error, %m.", __func__);
		return MRT_ERR;
	}

	while ((opt = getopt(argc, argv, options)) != EOF) {
		switch (opt) {
		case 'h':
			strcpy(server.host, optarg);
			break;

		case 'p':
			server.port = atoi(optarg);
			break;

		case 'd':
			server.daemon = 1;
			break;

		default:
			server_usage(argv[0]);
			break;
		}

	}

	return MRT_SUC;
}

int load_config(char *src)
{
	char home[MAX_PATH] = { 0 };

	if (!src || !*src) {
		printf("%s no found path:%s\n", __func__, src);
		return MRT_ERR;
	}

	sprintf(home, "%s/etc/%s", src, CONF_PATH);

	if (load_profile_first(home) == MRT_ERR) {
		printf("%s config file:%s error.\n", __func__, home);
		return MRT_ERR;
	}
	return MRT_SUC;
}
