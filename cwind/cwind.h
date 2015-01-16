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




typedef struct cw_content_s cw_content_t;
struct cw_content_s
{
    uint8_t             stat;
    string_t            first_class;
    string_t            second_class;
    string_t            url_preview;
    string_t            url_real;

    uint16_t            date;

    string_t            caption;
    string_t            desc;
    string_t            content;

    string_t            path_pic;
    string_t            path_swf;

    int32_t             size;                       //flash文件大小

    M_list_entry        (cw_content_t);
};

typedef struct
{
    uint32_t            list_sum;
    M_list_head         (cw_content_t);

}cw_content_list_t;


typedef struct cw_class_s cw_class_t;
struct cw_class_s
{
    string_t            url;
    string_t            first_class;
    string_t            second_class;
    /*
    char                first[MAX_ID]; //一级分类
    char                second[MAX_ID]; //二级分类
    */

    M_list_entry         (cw_class_t);
};


typedef struct
{
    uint16_t            list_sum;
    M_list_head          (cw_class_t);

}cw_class_list_t;


//内容，最终显示页
typedef struct
{
    char                part_begin[SMALL_STR];
    char                part_end[SMALL_STR];

    char                href_begin[SMALL_STR];
    char                href_end[SMALL_STR];

    char                caption_begin[SMALL_STR];
    char                caption_end[SMALL_STR];

    char                date_begin[SMALL_STR];
    char                date_end[SMALL_STR];

    char                oprate_begin[SMALL_STR];
    char                oprate_end[SMALL_STR];

    char                pic_begin[SMALL_STR];
    char                pic_end[SMALL_STR];

    char                desc_begin[SMALL_STR];
    char                desc_end[SMALL_STR];

    char                content_begin[SMALL_STR];
    char                content_end[SMALL_STR];

}cw_content_tag_t;


typedef struct
{
    char                list_part_begin[SMALL_STR];
    char                list_part_end[SMALL_STR];

    char                href_part_begin[SMALL_STR];
    char                href_part_end[SMALL_STR];

    char                href_begin[SMALL_STR];
    char                href_end[SMALL_STR];

    char                caption_begin[SMALL_STR];
    char                caption_end[SMALL_STR];

    char                page_part_begin[SMALL_STR];
    char                page_part_end[SMALL_STR];

    char                next_caption[SMALL_STR];

}cw_list_tag_t;

typedef struct
{
    char                part_begin[SMALL_STR];
    char                part_end[SMALL_STR];

}cw_class_tag_t;


typedef struct cw_site_s cw_site_t;
struct cw_site_s
{
    uint8_t             stat;
    int                 site_id;
    char                name[MAX_CAPTION];

    //下面是关键区域开始及结束的标记
    char                *hot_part_begin;
    char                *hot_part_end;
    char                md5[MAX_MD5];
    //计算此区域内容的md5并与数据库中的md5对比确定此网页是否更新

    char                base[MAX_URL];
    char                url[MAX_URL];
    char                charset[MAX_CAPTION];
    uint8_t             level;

    int                 list_tag_id;
    int                 content_id;
    int                 html_filter_id;
    int                 remote_mysql_id;

    uint16_t            all_sum;
    uint16_t            recv_num;

    mydb_t              *mysql_info;

    cw_content_list_t   *content_list;
    cw_class_list_t     *class_list;

    cw_class_tag_t      *class_tag;
    cw_list_tag_t       *list_tag;
    cw_content_tag_t    *content_tag;
    mrt_filter_t        *filter;

    M_list_entry        (cw_site_t);
};



typedef struct
{
    uint8_t             stat;
    uint16_t            btime;
    uint16_t            etime;

    uint16_t            all_sum;
    M_list_head         (cw_site_t);

    pthread_mutex_t     mtx;

}cw_site_list_t;


typedef struct
{
    uint16_t            sync_time;
    uint16_t            stat;

    pthread_mutex_t     mtx;
    mydb_t              *cwind_db;
    cw_site_list_t      *site_list;
    cw_class_list_t     *class_list;

    int                 class_ary[32];
    int                 max_proc;
    int                 cur_proc;

    char                serv_host[MAX_ADDR];
    int                 serv_port;

    char                html_root[MAX_ADDR];
    char                *home;
    int                 daemon;

}cw_config_t;

#endif
