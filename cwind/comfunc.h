#ifndef _COMFUN_H_
#define _COMFUN_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int load_file(const char *filename, char **buf);

int parse_db_conf();

int load_list_tag(mydb_t *, cw_site_t * site);

int load_content_tag(mydb_t *, cw_site_t * site);

int load_filter(mydb_t *, cw_site_t * site);

int load_remote_mysql_info(mydb_t *, cw_site_t * site);

int load_class_info(mydb_t *, cw_site_t * site);

int load_site_list(mydb_t *, cw_site_list_t ** list);

int mysql_info_init();

int mysql_info_open(mydb_t *);

int recv_html(char *url, char *data);

int site_log_init(cw_site_t * site);

int parse_cmd(int argc, char* argv[]);

#endif
