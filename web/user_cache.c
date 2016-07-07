#include <mysql.h>
#include "global.h"
#include "config.h"
#include "process.h"

extern config_t server;


//本地变量,用于缓存所有用户
static hashmap_t *user_map = NULL;


static void key_free(void *key)
{
    M_free(key);
}

static void val_free(void *val)
{
    M_free(val);
}




int cache_insert(uint32_t userid, char *username, char *password, int status)
{
    account_t *ac = NULL;

    M_cpsril(username);
    M_cpsril(password);

    if(!(ac = M_alloc(sizeof(*ac))))
    {
        log_error("malloc error:%s", get_error());
        return MRT_ERR;
    }

    ac->userid = userid;
    snprintf(ac->username, sizeof(ac->username), "%s", username);
    snprintf(ac->password, sizeof(ac->password), "%s", password);
    ac->status = status;

    if(!hashmap_insert(user_map, ac->username, strlen(ac->username), ac))
    {
        log_error("insert account to user map error:%s" , get_error());
        M_free(ac);
        return MRT_ERR;
    }

    log_info("insert userid:%u, username:%s, status:%d", userid, username, status);

    return MRT_SUC;

}

account_t *cache_find(char *username)
{
    account_t *ac = NULL;
    if((ac = hashmap_find(user_map, username, strlen(username))))
    {
        return ac;
    }

    log_info("cache no found user:%s", username);

    return NULL;
}

//如果remote和local都有就都改
int cache_set(user_info_t *uinfo)
{
    account_t *ac = NULL;
    int ret = 0;

    if(*uinfo->remote_mail)
    {
        if((ac = hashmap_find(user_map, uinfo->remote_mail, strlen(uinfo->remote_mail))))
        {
            memcpy(ac->password, uinfo->password, sizeof(uinfo->password));
            ac->status = uinfo->status;
            log_debug("remote_mail:%s set password:%s, status:%u",
                      uinfo->remote_mail, uinfo->password, uinfo->status);
            ret++;
        }
        else
        {
            cache_insert(uinfo->userid, uinfo->remote_mail, uinfo->password, uinfo->status);
        }
    }

    if(*uinfo->local_mail)
    {
        if((ac = hashmap_find(user_map, uinfo->local_mail, strlen(uinfo->local_mail))))
        {
            memcpy(ac->password, uinfo->password, sizeof(uinfo->password));
            ac->status = uinfo->status;
            log_debug("local_mail:%s set password:%s, status:%u",
                      uinfo->local_mail, uinfo->password, uinfo->status);
            ret++;
        }
        else
        {
            cache_insert(uinfo->userid, uinfo->local_mail, uinfo->password, uinfo->status);
        }
    }

    return ret;
}

int cache_set_status(user_info_t *uinfo)
{
    account_t *ac = NULL;
    int ret = 0;

    if(*uinfo->remote_mail)
    {
        if((ac = hashmap_find(user_map, uinfo->remote_mail, strlen(uinfo->remote_mail))))
        {
            ac->status = uinfo->status;
            log_debug("remote_mail:%s set status:%u",
                      uinfo->remote_mail, uinfo->status);
            ret++;
        }
    }

    if(*uinfo->local_mail)
    {
        if((ac = hashmap_find(user_map, uinfo->local_mail, strlen(uinfo->local_mail))))
        {
            ac->status = uinfo->status;
            log_debug("local_mail:%s set status:%u",
                      uinfo->local_mail, uinfo->status);
            ret++;
        }
    }

    return ret;
}

int cache_set_password(user_info_t *uinfo)
{
    account_t *ac = NULL;
    int ret = 0;

    if(*uinfo->remote_mail)
    {
        if((ac = hashmap_find(user_map, uinfo->remote_mail, strlen(uinfo->remote_mail))))
        {
            memcpy(ac->password, uinfo->password, sizeof(uinfo->password));
            log_debug("remote_mail:%s set password:%s", uinfo->remote_mail, uinfo->password);
            ret++;
        }
    }

    if(*uinfo->local_mail)
    {
        if((ac = hashmap_find(user_map, uinfo->local_mail, strlen(uinfo->local_mail))))
        {
            memcpy(ac->password, uinfo->password, sizeof(uinfo->password));
            log_debug("local_mail:%s set password:%s", uinfo->local_mail, uinfo->password);
            ret++;
        }
    }

    return ret;
}


int cache_create(int max)
{
    if(hashmap_create(max, &user_map) == MRT_ERR)
    {
        log_error("hashmap create error.");
        return MRT_ERR;
    }
    return MRT_SUC;
}

void cache_destroy()
{
    hashmap_free(user_map, key_free, val_free);
}



int cache_init()
{
    string_t sql;
    MYSQL_RES* res;
    MYSQL_ROW row;

    s_zero(sql);

    if(string_printf(&sql, "select userid, status, password, remote_mail, local_mail from user_info") == MRT_ERR)
    {
        log_error("string_printf sql error:%s", get_error());
        return MRT_ERR;
    }

    if(mydb_query(&server.userdb, &sql))
    {
        log_error("mydb_query error:%s, sql:%s", get_error(), sql.str);
        string_free(&sql);
        return MRT_ERR;
    }
    string_free(&sql);

    cache_create(MAX_USER_SIZE);

    res=mysql_use_result(&server.userdb.srv);

    while((row=mysql_fetch_row(res)))
    {
        if(row[3] && *row[3])
        {
            if(cache_insert(atoi(row[0]), row[3], row[2], atoi(row[1])) == MRT_ERR)
            {
                log_error("load remote_mail to account map error.");
                return MRT_ERR;
            }
        }

        if(row[4] && *row[4])
        {
            if(cache_insert(atoi(row[0]), row[4], row[2], atoi(row[1])) == MRT_ERR)
            {
                log_error("load local_mail to account map error.");
                return MRT_ERR;
            }
        }

    }

    mysql_free_result(res);

    return MRT_SUC;
}

