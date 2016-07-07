#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <pthread.h>

#define server_enable (server.state != SERVER_OVER)
#define server_disable (server.state == SERVER_OVER)

// -------------------------------

//command stat discription ----------

#define OPT_ANY 1
#define OPT_WAITPASS 2
#define OPT_AUTHORIZATION 4
#define OPT_TRANSACTION 8
#define OPT_QUIT 16

#define OPT_HALF 1
#define OPT_FULL 2

#define OPT_ENCRYPT 1

//------------------------------

typedef struct operate_s operate_t;

typedef struct {
	operate_t *operate;	//指向当前处理的命令声明处
	int state;		//当前命令执行到哪个状态

	string_t *req;		//请求消息
	string_t *resp;		//要返回的消息

	char *opt;		//指向缓存中的命令头
	char *dat;		//指向缓存中的命令内容

} request_t;

struct operate_s {
	char *name;		/* command name */
	int len;		/*当前命令的长度,提高搜索速度用 */
	int (*func) (request_t *);	/* callback function */
	unsigned int in_state;	/* state(s) in which this command is allowed */
	unsigned int min_args;	/* minimum number of arguments */
	unsigned int max_args;	/* maximum number of arguments */

};

int operate_init();		//command event function

//cache相关操作

int cache_init();

account_t *cache_find(char *username);

int cache_set_status(user_info_t * uinfo);

int cache_insert(uint32_t userid, char *username, char *password, int status);

//用户相关操作
//参数：
//      type: 0：注册远程用户， 1：注册本地用户
int email_register(user_info_t * uinfo, int type);

//功能：
//      检测邮件地址是否存在
//参数：
//      type: 0：注册远程用户， 1：注册本地用户

int email_check_exist(user_info_t * uinfo, int type);

//功能：
//      验证密码
//参数：
//      mask：掩码，与密码合并后验证
int email_check_password(user_info_t * uinfo, int type, char *mask);

int email_change_password(user_info_t * uinfo, int type);

int email_change_status(user_info_t * uinfo, int type);

int command_find(task_t * tsk);

int operate_find(request_t * rq, operate_t **);

int operate_parse(char *src, char *key, char *val, int len);

int request_init(task_t * task);
int request_proc(task_t * tsk);
int request_deinit(task_t * task);

//所有命令处理函数
int cmd_noop(request_t * rq);

int on_accept(void *);
int on_response(void *);
int on_request(void *);
int on_close(void *);

#endif
