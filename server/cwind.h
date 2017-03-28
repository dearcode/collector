#ifndef _MAIN_H_
#define _MAIN_H_

#include "global.h"
#include "macro_func.h"
#include "macro_const.h"
#include "http_func.h"

#define MAX_CONTENT 2*M_1MB
#define MAX_SQL 2048

#define CONFILE "cwind.conf"
#define HOME_ENV "CWIND_HOME"
#define DEFAULT_HOME "/data/cwind"

typedef struct content_s content_t;
struct content_s {
	uint8_t         stat;
	string_t        url_preview;
	string_t        url_real;

	uint16_t        date;

	string_t        caption;
	string_t        desc;
	string_t        content;

	string_t        path_pic;
	string_t        path_swf;

	int32_t         size;	//flash文件大小

	list_node_t     node;
};

typedef struct {
	uint32_t        list_sum;
	list_head_t     head;

} cw_content_list_t;

//内容，最终显示页
typedef struct {
	char            body_begin[SMALL_STR];
	char            body_end[SMALL_STR];

	char            title_begin[SMALL_STR];
	char            title_end[SMALL_STR];

	char            date_begin[SMALL_STR];
	char            date_end[SMALL_STR];

	char            oprate_begin[SMALL_STR];
	char            oprate_end[SMALL_STR];

	char            desc_begin[SMALL_STR];
	char            desc_end[SMALL_STR];

	char            content_begin[SMALL_STR];
	char            content_end[SMALL_STR];

} tag_content_t;

typedef struct {
	char            body_begin[SMALL_STR];
	char            body_end[SMALL_STR];

	char            item_begin[SMALL_STR];
	char            item_end[SMALL_STR];

	char            url_begin[SMALL_STR];
	char            url_end[SMALL_STR];

	char            title_begin[SMALL_STR];
	char            title_end[SMALL_STR];

	char            page_begin[SMALL_STR];
	char            page_end[SMALL_STR];

	char            next_label[SMALL_STR];

} tag_list_t;

typedef struct site_s site_t;
struct site_s {
	uint8_t         stat;
	int             id;
	char            name[MAX_CAPTION];

	//下面是关键区域开始及结束的标记
	char           *area_begin;
	char           *area_end;
	char            md5[MAX_MD5];
	//计算此区域内容的md5并与数据库中的md5对比确定此网页是否更新

	char            base[MAX_URL];
	char            url[MAX_URL];
	char            charset[MAX_CAPTION];
	uint8_t         level;

	int             list_id;
	int             content_id;
	int             store_id;

	uint16_t        all_sum;
	uint16_t        recv_num;

	mydb_t         *mysql_info;

	cw_content_list_t content_list;

	tag_list_t      list_tag;
	tag_content_t   content_tag;
	mrt_filter_t    filter;

	list_node_t     node;
};

typedef struct {
	uint8_t         stat;
	uint16_t        btime;
	uint16_t        etime;

	uint16_t        all_sum;
	list_head_t     head;

	pthread_mutex_t mtx;

} site_list_t;

typedef struct {
	uint16_t        sync_time;
	uint16_t        stat;

	pthread_mutex_t mtx;
	mydb_t         *cwind_db;
	site_list_t *site_list;

	int             class_ary[32];
	int             max_proc;
	int             cur_proc;

	char            serv_host[MAX_ADDR];
	int             serv_port;

	char            html_root[MAX_ADDR];
	char           *home;
	int             daemon;

} cw_config_t;

#endif
