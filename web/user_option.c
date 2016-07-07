#include <mysql.h>
#include "global.h"
#include "config.h"
#include "process.h"


extern config_t server;

//参数：
//      type: 0：注册远程用户， 1：注册本地用户

int email_register(user_info_t *uinfo, int type)
{
    string_t sql;
    int iret = MRT_SUC;

    s_zero(sql);

    iret = string_printf(&sql, "call %s_mail_add('%s', '%s', '%s', %d)",
                         type == 0? "remote": "local",
                         type == 0?uinfo->remote_mail:uinfo->local_mail,
                         uinfo->password, uinfo->register_ip, uinfo->status);
    if(iret == MRT_ERR)
    {
        log_error("string_printf error:%s", get_error());
        return MRT_ERR;
    }

    if(mydb_query_uint32(&server.userdb, &sql, &uinfo->userid))
    {
        log_error("mydb_query_long error:%s, sql:%s", get_error(), sql.str);
        string_free(&sql);
        return MRT_ERR;
    }
    string_free(&sql);

    iret = cache_insert(uinfo->userid, type == 0?uinfo->remote_mail:uinfo->local_mail, uinfo->password, uinfo->status);

    log_debug("mysql insert success, cache insert %s, email:%s, userid:%u, password:%s, status:%d",
              iret == MRT_ERR ? "fail":"success",
              type?uinfo->local_mail:uinfo->remote_mail,
              uinfo->userid, uinfo->password, uinfo->status);

    return MRT_SUC;
}

//功能：
//      检测邮件地址是否存在
//参数：
//      type: 0：注册远程用户， 1：注册本地用户
//返回：
//      MRT_SUC：不存在
//      MRT_ERR: 临时出错
//      ERR_USER_EXIST：存在

int email_check_exist(user_info_t *uinfo, int type)
{
    string_t sql;
    int iret = MRT_SUC;
    account_t *ac;

    //如果cache找到直接返回
    if((ac = cache_find(type?uinfo->local_mail:uinfo->remote_mail)))
    {
        uinfo->status = ac->status;
        log_debug("cache find mail:%s status:%d", type?uinfo->local_mail:uinfo->remote_mail, uinfo->status);
        return ERR_USER_EXIST;
    }

    s_zero(sql);

    iret = string_printf(&sql, "select status from user_info where %s_mail='%s'",
                         type == 0 ? "remote": "local",
                         type == 0 ? uinfo->remote_mail: uinfo->local_mail);
    if(iret == MRT_ERR)
    {
        log_error("string_printf error:%s", get_error());
        return MRT_ERR;
    }

    iret = mydb_query_exist(&server.userdb, &sql);
    if(iret == MRT_ERR)
    {
        log_error("mydb_query_int error:%s, sql:%s", get_error(), sql.str);
        string_free(&sql);
        return MRT_ERR;
    }
    string_free(&sql);

    if(iret == 0)
    {
        log_info("mysql no found mail:%s", type?uinfo->local_mail:uinfo->remote_mail);
        return ERR_USER_NOFOUND;
    }

    log_info("mysql find mail:%s", type?uinfo->local_mail:uinfo->remote_mail);

    return ERR_USER_EXIST;
}


//功能：
//      查找账号信息，如果内存中不存在，加载到内存中
//参数：
//      type: 0：注册远程用户， 1：注册本地用户

int account_find(user_info_t *uinfo, int type)
{
    string_t sql;
    int iret = MRT_SUC;
    MYSQL_RES* res = NULL;
    MYSQL_ROW row;
    account_t *pact = NULL;

    //如果cache找到直接返回
    if((pact = cache_find(type?uinfo->local_mail:uinfo->remote_mail)))
    {
        uinfo->status = pact->status;
        uinfo->userid = pact->userid;
        snprintf(uinfo->password, sizeof(uinfo->password), "%s", pact->password);
        log_debug("cache find mail:%s status:%d", type?uinfo->local_mail:uinfo->remote_mail, uinfo->status);
        return MRT_SUC;
    }

    s_zero(sql);

    iret = string_printf(&sql,
                         "select userid, password, status from user_info where %s_mail='%s'",
                         type == 0 ? "remote" : "local",
                         type == 0 ? uinfo->remote_mail : uinfo->local_mail);
    if(iret == MRT_ERR)
    {
        log_error("string_printf error:%s", get_error());
        return MRT_ERR;
    }

    if(mydb_query(&server.userdb, &sql))
    {
        log_error("mydb_query error:%s, sql:%s", get_error(), sql.str);
        string_free(&sql);
        return MRT_ERR;
    }
    string_free(&sql);

    if(!(res=mysql_store_result(&server.userdb.srv)) || !(row=mysql_fetch_row(res)))
    {
        log_error("mysql no found mail:%s", type?uinfo->local_mail:uinfo->remote_mail);
        return MRT_ERR;
    }

    uinfo->userid = row[0] && *row[0] ? atoll(row[0]) : 0;

    snprintf(uinfo->password, sizeof(uinfo->password), "%s", row[1]);

    uinfo->status = row[2] && *row[2] ? atoll(row[2]) : 0;

    mysql_free_result(res);

    if(*uinfo->local_mail)
        iret = cache_insert(uinfo->userid, uinfo->local_mail, uinfo->password, uinfo->status);
    else
        iret = cache_insert(uinfo->userid, uinfo->remote_mail, uinfo->password, uinfo->status);

    log_info("cache insert %s userid:%u, mail:%s, password:%s, status:%u",
              iret == MRT_SUC?"success":"fail",
              uinfo->userid,
              type == 0 ? uinfo->remote_mail : uinfo->local_mail,
              uinfo->password, uinfo->status);

    return MRT_SUC;
}



//功能：
//      验证密码
//参数：
//      mask：掩码，与密码合并后验证
int email_check_password(user_info_t *uinfo, int type, char *mask)
{
    char temp_password[65] = {0}, local_password[65] = {0}, recv_password[65] = {0};

    snprintf(recv_password, sizeof(recv_password), "%s", uinfo->password);

    if(account_find(uinfo, type) != MRT_SUC)
    {
        log_error("no found email:%s", type?uinfo->local_mail:uinfo->remote_mail);
        return MRT_ERR;
    }

    if(uinfo->status != 1)
    {
        log_error("invalid user status:%d, username:%s",
                  uinfo->status, type?uinfo->local_mail:uinfo->remote_mail);
        return MRT_ERR;
    }

    snprintf(local_password, sizeof(local_password), "%s%s", uinfo->password, mask);

//    log_debug("local_password:%s", local_password);
    make_md5((unsigned char *)local_password, strlen(local_password), temp_password);
 //   log_debug("temp_password:%s", temp_password);

    if(!strcmp(temp_password, recv_password))
    {
        log_debug("password ok, email:%s, recv password:%s",
                  type?uinfo->local_mail:uinfo->remote_mail, recv_password);
        return MRT_SUC;
    }

    log_error("password error, email:%s, recv password:%s, local password:%s, mask:%s, temp password:%s",
              type?uinfo->local_mail:uinfo->remote_mail, recv_password, uinfo->password, mask, temp_password);

    return MRT_ERR;
}

//功能：
//      修改用户状态
//参数：
//      type: 0：注册远程用户， 1：注册本地用户
int email_change_status(user_info_t *uinfo, int type)
{
    string_t *sql = NULL;

    if(type)
    {
        M_cvril((sql = string_new(SHORT_STR,
                                  "update user_info set status=%d where local_mail='%s'",
                                  uinfo->status, uinfo->local_mail)),
                "create sql string error.");
    }
    else
    {
        M_cvril((sql = string_new(SHORT_STR,
                                  "update user_info set status=%d where remote_mail='%s'",
                                  uinfo->status, uinfo->remote_mail)),
                "create sql string error.");
    }

    if(mydb_exec(&server.userdb, sql) != 1)
    {
        log_error("mysql change status error.");
        string_free(sql);
        return MRT_ERR;
    }

    string_free(sql);

    log_info("email:%s new status:%u, cache return:%d",
             type?uinfo->local_mail:uinfo->remote_mail, uinfo->status, cache_set_status(uinfo));

    return MRT_SUC;
}




int email_change_password(user_info_t *uinfo, int type)
{
    string_t *sql = NULL;

    if(type)
    {
        M_cvril((sql = string_new(SHORT_STR,
                                  "update user_info set password='%s' where local_mail='%s'",
                                  uinfo->password, uinfo->local_mail)),
                "create sql string error.");
    }
    else
    {
        M_cvril((sql = string_new(SHORT_STR,
                                  "update user_info set password='%s' where remote_mail='%s'",
                                  uinfo->password, uinfo->remote_mail)),
                "create sql string error.");
    }

    if(mydb_exec(&server.userdb, sql) != 1)
    {
        log_error("mysql change password error.");
        string_free(sql);
        return MRT_ERR;
    }

    string_free(sql);
    log_info("email:%s new password:%s, cache return:%d",
             type?uinfo->local_mail:uinfo->remote_mail, uinfo->password, cache_set_status(uinfo));

    return MRT_SUC;
}






