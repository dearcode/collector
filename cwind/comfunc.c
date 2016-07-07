#include<openssl/md5.h>
#include "global.h"
#include "cwind.h"
#include "comfunc.h"
#include "event.h"

extern cw_config_t server;

void serv_usage(char *pname)
{
	printf("Usage: %s -h host -p port [-d]\n"
	       "\n  -h host\tbind ip." "\n  -p port\tbind port." "\n  -d     \trun as daemon.\n", pname);

	return;
}

int parse_cmd(int argc, char *argv[])
{
	const char *options = "h:p:d";
	int opt;

	if (argc < 3) {
		serv_usage(argv[0]);
		return MRT_ERR;
	}

	server.daemon = 0;

	while ((opt = getopt(argc, argv, options)) != EOF) {
		switch (opt) {
		case 'h':
			memset(server.serv_host, 0, MAX_IP);
			strcpy(server.serv_host, optarg);
			break;

		case 'p':
			server.serv_port = atoi(optarg);
			break;

		case 'd':
			server.daemon = 1;
			break;

		default:
			serv_usage(argv[0]);

			break;
		}
	}

	return MRT_OK;
}

int affect_class_add(int first_id)
{
	int i = 0;

	while (server.class_ary[i]) {
		if (first_id == server.class_ary[i++])
			return MRT_SUC;
	}

	server.class_ary[i] = first_id;

	return MRT_SUC;
}

int load_profile(char *file)
{
	hashmap_t hmp = { 0 };
	char *pstr = NULL;

	if (ini_file_load(file, &hmp) == MRT_ERR) {
		printf("%s load profile error, file:%s.\n", __func__, file);
		return MRT_ERR;
	}

	if ((pstr = ini_get_str(&hmp, "cwind_db", "db_ip", NULL)) == NULL) {
		printf("%s no found cwind_db:db_ip\n", __func__);
		return MRT_ERR;
	}
	sprintf(server.cwind_db->ip, "%s", pstr);

	if ((server.cwind_db->port = ini_get_int(&hmp, "cwind_db", "db_port", -1)) == MRT_ERR) {
		printf("%s no found cwind_db:db_port\n", __func__);
		return MRT_ERR;
	}

	if ((pstr = ini_get_str(&hmp, "cwind_db", "db_user", NULL)) == NULL) {
		printf("%s no found cwind_db:db_user\n", __func__);
		return MRT_ERR;
	}
	sprintf(server.cwind_db->user, "%s", pstr);

	if ((pstr = ini_get_str(&hmp, "cwind_db", "db_pass", NULL)) == NULL) {
		printf("%s no found cwind_db:db_pass\n", __func__);
		return MRT_ERR;
	}
	sprintf(server.cwind_db->pass, "%s", pstr);

	if ((pstr = ini_get_str(&hmp, "cwind_db", "db_name", NULL)) == NULL) {
		printf("%s no found cwind_db:db_name\n", __func__);
		return MRT_ERR;
	}
	sprintf(server.cwind_db->name, "%s", pstr);

	ini_file_unload(&hmp);

	return MRT_OK;
}

int load_config()
{
	char file_name[MAX_PATH] = { 0 };

	server.home = getenv(HOME_ENV);
	if (!server.home) {
		printf("%s no find  environment variable:%s\n", __func__, HOME_ENV);
		return MRT_ERR;
	}

	sprintf(file_name, "%s/var/etc/%s", server.home, CONFILE);

	if ((server.cwind_db))
		M_free(server.cwind_db);

	server.cwind_db = M_alloc(sizeof(mydb_t));

	return load_profile(file_name);

}
