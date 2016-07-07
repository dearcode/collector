#ifndef __GLOBAL_H_
#define __GLOBAL_H__


#define MAX_USER_SIZE 10000
#define MAX_SOCKET_BUFFER 102400

#define HOME_ENV "MRTD_HOME"
#define HOME_PATH "/home/mrtd"
#define CONF_PATH "mrtd.conf"

#define ERR_PROTOCOL -1001

#define ERR_USER_NOFOUND -1101
#define ERR_USER_EXIST -1102
#define ERR_USER_STATUS -1103

#define VERSION "1.0.0"


typedef struct
{
    uint32_t    userid;
    char        remote_mail[MAX_ID];
    char        local_mail[MAX_ID];
    char        password[MAX_PASS];
    char        mobile[MAX_PASS];
    int         status;
    char        register_ip[MAX_IP];
    char        register_time[MAX_DATE];
    char        question[MAX_ID];
    char        answer[MAX_ID];
}user_info_t;

typedef struct
{
    uint32_t    userid;
    char        username[MAX_ID];
    char        password[MAX_PASS];
    uint32_t    status;
    int         mmid;
}account_t;

//----------------------

typedef struct
{
    //配置项目
    char                host[MAX_ADDR];
    int                 port;
    int8_t              daemon;
    char                home[256];
    int                 log_level;          //log level
    int                 conf_sync_time;     //config sync time
    int                 max_task;           //最大任务数
    int                 conn_max;           //最大连接数
    int8_t              worker_max;         //最大处理线程数
    int8_t              worker_min;         //最小处理线程数
    int8_t              conn_timeout;       //socket write Timeout

    //动态内容

    int8_t              state; //全局状态
    mydb_t              userdb; //userinfo数据库连接

    pthread_mutex_t     conf_mtx; //配置文件同步锁


}config_t;


int parse_cmd(int argc, char* argv[]);

int load_config(char *src);

#endif
