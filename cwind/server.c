#include "cwind.h"
#include "event.h"
#include "comfunc.h"

cw_config_t server;

int load_site_info(mydb_t * mdb, cw_site_t * site)
{
	if (load_list_tag(mdb, site) == MRT_ERR) {
		log_error("Load game lisite->tags error, site id:%d, name:%s.", site->site_id, site->name);
		return MRT_ERR;
	}

	if (load_content_tag(mdb, site) == MRT_ERR) {
		log_error("Load game info tags error, site id:%d, name:%s.", site->site_id, site->name);
		return MRT_ERR;
	}

	if (load_filter(mdb, site) == MRT_ERR) {
		log_error("Load filter info tags error, site id:%d, name:%s.", site->site_id, site->name);
		return MRT_ERR;
	}

	if (load_remote_mysql_info(mdb, site) == MRT_ERR) {
		log_error("Load remote mysql info error, site id:%d, name:%s.", site->site_id, site->name);
		return MRT_ERR;
	}

	return MRT_OK;
}

int server_start(void *dat)
{
	cw_site_t *site = NULL;

	if (load_site_list(server.cwind_db, &server.site_list) == MRT_ERR) {
		log_error("%s load site list error.", __func__);
		return MRT_ERR;
	}

	LIST_FOREACH(site, node, server.site_list, head) {
		if (load_site_info(server.cwind_db, site)) {
			log_error("%s load tags error.", __func__);
			return MRT_ERR;
		}

		if (site_log_init(site) == MRT_ERR) {
			log_error("site_log_init error");
			return MRT_ERR;
		}

		printf("%s\n", site->url);

		if (check_web(site)) {
			log_error("%s check current web site error, site:[%s].", __func__, site->name);
			continue;
		}

		log_info("%s check web site success, site:[%s].", __func__, site->name);
	}

	log_info("%s check alll site success.", __func__);

	mysql_info_close(server.cwind_db);

	return MRT_OK;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("%s parameter argv error\n"
		       "%s X Y\n" "X max process \n" "Y current process number\n", __func__, argv[0]);
		return -1;
	}

	if (logger_init("./var/log/", "cwind", 1) == MRT_ERR) {
		printf("%s init logger error.\n", __func__);
		return MRT_ERR;
	}

	if (memory_pool_init() == MRT_ERR) {
		printf("%s:%d mem pool init error.\n", __func__, __LINE__);
		return MRT_ERR;
	}

	if (load_config() == MRT_ERR) {
		printf("%s load config error.\n", __func__);
		return MRT_ERR;
	}

	server.max_proc = atoi(argv[1]);
	server.cur_proc = atoi(argv[2]);

	printf("%s max process:%d, current number:%d\n", __func__, server.max_proc, server.cur_proc);

	strcpy(server.html_root, ".");

	pthread_key_create(&(factory.key), NULL);
	pthread_setspecific(factory.key, &(factory.master));

	server_start(NULL);

	return MRT_OK;
}
