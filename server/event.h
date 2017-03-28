#ifndef _EVENT_H__
#define _EVENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <mysql.h>

#include "cwind.h"

int mysql_info_init();

int mysql_open_utf8(mydb_t * mysql_info);

int check_web(site_t * st);

int load_config();

int server_init();

int server_start();

int fix_content_list_info(site_t * site, mrt_page_t * page);

int fix_content_url(site_t * site);

int fix_url(mrt_page_t *, char *);

int fetch_href(char *src, char *url, char *caption);

int fetch_img(char *src, char *url);

void page_free(mrt_page_t * page);

void content_free(content_t * content);

int content_info_save(site_t * site, content_t * ct);

int update_content_info(content_t * content);

int save_preview_list(site_t * site);

int fix_charset_newcpy(char *, char **content);

int fetch_href_ex(char *src, char **url, char **caption);

int fetch_img_ex(char *src, char **dest);

int fix_url_ex(mrt_page_t * page, char **url);

int fix_charset_ex(mrt_page_t * page, char *content, int size);

int fix_desc(mrt_page_t * page, char *caption);

int save_joke_info(site_t * site, content_t * joke);

int find_url_status(site_t * site, char *url, int type);

int set_url_status(site_t * site, char *url, int type, int status);

#define mysql_info_close(mysql_info) mysql_close(&(mysql_info)->srv)

#endif
