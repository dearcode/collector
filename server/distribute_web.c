#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

#include "global.h"
#include "cwind.h"
#include "event.h"

extern cw_config_t server;

#define JSP_class_url "http://codecn.org:8080/game/createHtmlByFirClassId?firClassId="
#define JSP_game_url "http://codecn.org:8080/game/createGameHtml?gameId="
#define JSP_new_url "http://codecn.org:8080/game/createNewHtml"
#define JSP_hot_url "http://codecn.org:8080/game/createHotHtml"
#define JSP_score_url "http://codecn.org:8080/game/createScoreHtml"
#define JSP_index_url "http://codecn.org:8080/game/createIndexHtml"

int distribute_begin(T_ftp_session * ftp, T_ftp_host * host)
{
	printf("host:%s:%d\n", host->ip, host->port);
	if (ftp_session_init(ftp, host->ip, host->port, host->user, host->pass)) {
		log_error
		    ("ftp session init errror, addr:%s:%d, user:%s, pass:%s",
		     host->ip, host->port, host->user, host->pass);
		return MRT_ERR;
	}

	ftp->root = host->root;

	return MRT_SUC;
}

int distribute_end(T_ftp_session * fs)
{
	return ftp_session_destroy(fs);
}

int call_java_action(char *url, char **buf)
{
	T_page *page = NULL;
	if (http_recv_page(url, &page, 0) == MRT_ERR) {
		log_error("http_recv_page error, %s", get_error());
		return MRT_ERR;
	}

	if (!(*buf = str_newcpy(page->pbody))) {
		log_error("str_newcpy error, %s", get_error());
		http_free_page(page);
		return MRT_ERR;
	}

	http_free_page(page);

	return MRT_SUC;
}

int java_create_html(char *prefix, int first_id, T_jsp_list * jlst)
{
	char *buf = NULL;
	char url[MAX_URL];

	if (first_id == MRT_ERR)
		sprintf(url, "%s", prefix);
	else
		sprintf(url, "%s%d", prefix, first_id);

	if (call_java_action(url, &buf) == MRT_ERR) {
		log_error("call java action error, first id:%d.", first_id);
		return MRT_ERR;
	}

	if ((parse_html_list(buf, jlst)) == MRT_ERR) {
		log_error("parse html list error, buf:%s", buf);
		M_free(buf);
		return MRT_ERR;
	}

	M_free(buf);

	return MRT_SUC;
}

int send_html_list(T_jsp_list * jlst, T_ftp_session * ftp)
{
	T_jsp_page *jsp;
	char target_folder[MAX_LINE];
	char local_file[MAX_LINE];

	M_list_foreach(jsp, jlst) {
		sprintf(local_file, "%s/%s/%s", jsp->path, jsp->folder, jsp->file);
		ftp->local_file = local_file;
		ftp->target_file = jsp->file;
		sprintf(target_folder, "%s/%s", ftp->root, jsp->folder);
		ftp->target_folder = target_folder;
		if (ftp_process_file(ftp, jsp->oprate)) {
			log_error
			    ("ftp process file error, local:%s, remote:%s/%s",
			     local_file, ftp->target_folder, jsp->file);
			return MRT_ERR;
		}

		log_info("ftp process file success, local:%s, remote:%s/%s", local_file, target_folder, jsp->file);
	}

	return MRT_SUC;
}

int distribute_proc_game(T_ftp_session * ftp, T_jsp_list * jlst)
{
	T_jsp_page *jsp;
	M_list_foreach(jsp, server.jsp_list) {
		if ((java_create_html(JSP_game_url, jsp->game_id, jlst))) {
			log_error("create game html content error, url:%s%d", JSP_game_url, jsp->game_id);
			return MRT_ERR;
		}
	}

	return MRT_SUC;
}

int distribute_proc_class(T_ftp_session * ftp, T_jsp_list * jlst)
{
	int i = 0;

	while (server.class_ary[i] != 0) {
		if (java_create_html(JSP_class_url, server.class_ary[i], jlst)
		    == MRT_ERR) {
			log_error("distribute class jsp list error");
			return MRT_ERR;
		}

		i++;
	}

	return MRT_SUC;
}

int distribute_proc_index(T_ftp_session * ftp, T_jsp_list * jlst)
{

	if (java_create_html(JSP_index_url, -1, jlst) == MRT_ERR) {
		log_error("distribute index jsp list error");
		return MRT_ERR;
	}

	if (java_create_html(JSP_new_url, -1, jlst) == MRT_ERR) {
		log_error("distribute new game jsp list error");
		return MRT_ERR;
	}

	if (java_create_html(JSP_hot_url, -1, jlst) == MRT_ERR) {
		log_error("distribute hot game jsp list error");
		return MRT_ERR;
	}

	if (java_create_html(JSP_score_url, -1, jlst) == MRT_ERR) {
		log_error("distribute score game jsp list error");
		return MRT_ERR;
	}

	return MRT_SUC;
}

int distribute_proc(T_ftp_host * host, T_jsp_list * jlst, int onetime)
{
	T_ftp_session *ftp = M_alloc(sizeof(T_ftp_session));

	if (distribute_begin(ftp, host) == MRT_ERR) {
		log_error("distribute_begin error");
		return MRT_ERR;
	}

	if (!onetime) {
		if (distribute_proc_game(ftp, jlst) == MRT_ERR) {
			log_error("distribute_proc_game error.");
			distribute_end(ftp);
			return MRT_ERR;
		}

		if (server.class_ary[0] != 0) {
			if (distribute_proc_class(ftp, jlst) == MRT_ERR) {
				log_error("distribute_proc_class error.");
				distribute_end(ftp);
				return MRT_ERR;
			}

			if (distribute_proc_index(ftp, jlst) == MRT_ERR) {
				log_error("distribute_proc_class error.");
				distribute_end(ftp);
				return MRT_ERR;
			}
		}

	}

	if (send_html_list(jlst, ftp) == MRT_ERR) {
		log_error("send html list error.");
		return MRT_ERR;
	}

	distribute_end(ftp);

	return MRT_SUC;
}

int distribute_loop()
{
	T_ftp_host *host;
	T_jsp_list jlst;
	int onetime = 0;

	server.jsp_list = M_alloc(sizeof(T_jsp_list));
	M_list_init(server.jsp_list);
	server.ftp_list = M_alloc(sizeof(T_ftp_host_list));
	M_list_init(server.ftp_list);

	while (1) {
		log_info("process game modify list begin.");
		server.jsp_list->size = 0;
		if (load_modify_content(server.game_db) == MRT_ERR) {
			log_error("load_modify_list error");
			return MRT_ERR;
		}

		if (server.jsp_list->size > 0) {
			if (load_ftp_list(server.cwind_db)) {
				log_error("load ftp list error");
				return MRT_ERR;
			}

			onetime = 0;
			M_zero(&jlst);
			M_list_init(&jlst);

			M_list_foreach(host, server.ftp_list) {
				if (distribute_proc(host, &jlst, onetime) == MRT_ERR) {
					log_error("distribute process error, ftp user:%s.", host->user);
					return MRT_ERR;
				}
				onetime++;
			}

			jsp_list_free(&jlst);

			if (unload_modify_list(server.game_db)) {
				log_error("unload modify list error.");
				return MRT_ERR;
			}

			if (unload_ftp_list()) {
				log_error("unload ftp list error");
				return MRT_ERR;
			}

		}

		log_info("process game modify list success, will sleep.");
		sleep(60);
	}
	return MRT_SUC;
}
