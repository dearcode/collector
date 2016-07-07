#include <ctype.h>
#include "global.h"
#include "config.h"
#include "process.h"

extern config_t server;




#define OPERATE_PARSE_REQUIRED(src, key) \
    do { \
        if(operate_parse(src, #key, uinfo.key, sizeof(uinfo.key))) \
        { \
            log_error("no found '%s'", #key); \
            string_printf(buf, "-ERR No found '%s'"CRLF, #key); \
            return MRT_ERR; \
        } \
    } while(0)


//remote_register remote_mail=test1@testmail.com password=111111 register_ip=127.0.0.1 status=1

int remote_register(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;
    char val[SMALL_STR] = {0};

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, remote_mail);
    OPERATE_PARSE_REQUIRED(src, password);
    OPERATE_PARSE_REQUIRED(src, register_ip);

    p_zero(val);
    if(operate_parse(src, "status", val, sizeof(val)))
    {
        log_info("no found `status`, use default:`0`");
        uinfo.status = 0;
    }
    else
    {
        uinfo.status = atoi(val);
    }

    if(email_register(&uinfo, 0) == MRT_ERR)
    {
        log_error("account register error, cmd:%s, arg:%s", rq->opt, rq->dat);
        string_printf(buf, "-ERR %s."CRLF, "register error");
        return MRT_SUC;
    }

    string_printf(buf, "+OK userid=%u"CRLF, uinfo.userid);

    return MRT_OK;
}

int local_register(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;
    char val[SMALL_STR] = {0};

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, local_mail);
    OPERATE_PARSE_REQUIRED(src, password);
    OPERATE_PARSE_REQUIRED(src, register_ip);

    p_zero(val);
    if(operate_parse(src, "status", val, sizeof(val)))
    {
        log_info("no found `status`, use default:`0`");
        uinfo.status = 0;
    }
    else
    {
        uinfo.status = atoi(val);
    }

    if(email_register(&uinfo, 1) == MRT_ERR)
    {
        log_error("account register error, cmd:%s, arg:%s", rq->opt, rq->dat);
        string_printf(buf, "-ERR %s."CRLF, "register error");
        return MRT_SUC;
    }

    string_printf(buf, "+OK userid=%u"CRLF, uinfo.userid);

    return MRT_OK;
}


//remote_auth password=d1697089a9cc5cfce19e23f3e0f0f84c mask=abcdefg remote_mail=test1@testmail.com
int remote_auth(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;
    char mask[SMALL_STR] = {0};

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, remote_mail);
    OPERATE_PARSE_REQUIRED(src, password);

    if(operate_parse(src, "mask", mask, sizeof(mask)))
    {
        log_error("no found 'mask'");
        string_printf(buf, "-ERR No found 'mask'"CRLF);
        return MRT_ERR;
    }

    if(email_check_password(&uinfo, 0, mask) == MRT_ERR)
    {
        log_error("email_check_password error, cmd:%s, arg:%s", rq->opt, rq->dat);
        string_printf(buf, "-ERR %s."CRLF, "check password error");
        return MRT_SUC;
    }

    string_printf(buf, "+OK userid=%u"CRLF, uinfo.userid);

    return MRT_OK;
}

int local_auth(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;
    char mask[SMALL_STR] = {0};

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, local_mail);
    OPERATE_PARSE_REQUIRED(src, password);

    if(operate_parse(src, "mask", mask, sizeof(mask)))
    {
        log_error("no found 'mask'");
        string_printf(buf, "-ERR No found 'mask'"CRLF);
        return MRT_ERR;
    }

    if(email_check_password(&uinfo, 1, mask) == MRT_ERR)
    {
        log_error("email_check_password error, cmd:%s, arg:%s", rq->opt, rq->dat);
        string_printf(buf, "-ERR %s."CRLF, "passwrod error");
        return MRT_SUC;
    }

    string_printf(buf, "+OK userid=%u"CRLF, uinfo.userid);

    return MRT_OK;
}


int remote_check(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, remote_mail);

    switch(email_check_exist(&uinfo, 0))
    {
    case ERR_USER_NOFOUND:
        log_info("email:%s no exist.", uinfo.remote_mail);
        string_printf(buf, "+OK Email:%s no exist"CRLF, uinfo.remote_mail);
        break;
    case ERR_USER_EXIST:
        log_info("email:%s exist", uinfo.remote_mail);
        string_printf(buf, "-ERR Email:%s exist"CRLF, uinfo.remote_mail);
        break;

    default:
        log_info("email:%s email_check_exist temp error", uinfo.remote_mail);
        string_printf(buf, "-ERR Email:%s temp error"CRLF, uinfo.remote_mail);
        break;
    }

    return MRT_OK;
}

int local_check(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, local_mail);

    switch(email_check_exist(&uinfo, 1))
    {
    case MRT_SUC:
        log_info("email:%s no exist.", uinfo.local_mail);
        string_printf(buf, "+OK Email:%s no exist"CRLF, uinfo.local_mail);
        break;
    case MRT_ERR:
        log_info("email:%s email_check_exist temp error", uinfo.local_mail);
        string_printf(buf, "-ERR Email:%s temp error"CRLF, uinfo.local_mail);
        break;
    case ERR_USER_EXIST:
        log_info("email:%s exist", uinfo.local_mail);
        string_printf(buf, "-ERR Email:%s exist"CRLF, uinfo.local_mail);
        break;
    }

    return MRT_OK;
}


int remote_mod_pwd(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;
    char mask[SMALL_STR] = {0};
    char new_password[SMALL_STR] = {0};

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, remote_mail);
    OPERATE_PARSE_REQUIRED(src, password);

    if(operate_parse(src, "mask", mask, sizeof(mask)))
    {
        log_error("no found 'mask'");
        string_printf(buf, "-ERR No found 'mask'"CRLF);
        return MRT_ERR;
    }

    if(operate_parse(src, "new_password", new_password, sizeof(new_password)))
    {
        log_error("no found 'new_passwrod'");
        string_printf(buf, "-ERR No found 'new_password'"CRLF);
        return MRT_ERR;
    }

    if(email_check_password(&uinfo, 0, mask) == MRT_ERR)
    {
        log_error("mail password error, mail:%s, password:%s, mask:%s",
                  uinfo.remote_mail, uinfo.password, mask);
        string_printf(buf, "-ERR Password error."CRLF);
        return MRT_SUC;
    }

    snprintf(uinfo.password, sizeof(uinfo.password), "%s", new_password);

    if(email_change_password(&uinfo, 0) == MRT_ERR)
    {
        log_error("mail change password error, mail:%s, new password:%s", uinfo.remote_mail, new_password);
        string_printf(buf, "-ERR Change password error."CRLF);
        return MRT_SUC;
    }

    log_info("mail change password success, mail:%s, new password:%s", uinfo.remote_mail, new_password);

    string_printf(buf, "+OK Change password success."CRLF);

    return MRT_OK;
}







int local_mod_pwd(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;
    char val[SMALL_STR] = {0};

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, local_mail);
    OPERATE_PARSE_REQUIRED(src, password);

    p_zero(val);
    if(operate_parse(src, "status", val, sizeof(val)))
    {
        log_info("no found `status`, use default:`0`");
        uinfo.status = 0;
    }
    else
    {
        uinfo.status = atoi(val);
    }

    if(email_change_status(&uinfo, 0) == MRT_ERR)
    {
        log_error("mail change status error, mail:%s, new status:%d", uinfo.local_mail, uinfo.status);
        string_printf(buf, "-ERR Change status error."CRLF);
        return MRT_SUC;
    }

    log_info("mail change status success, mail:%s, new status:%d", uinfo.local_mail, uinfo.status);

    string_printf(buf, "+OK Change status success."CRLF);

    return MRT_OK;
}





int remote_set_pwd(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;
    char new_password[SMALL_STR] = {0};

    s_zero(uinfo);

    src = rq->resp->str;

    OPERATE_PARSE_REQUIRED(src, remote_mail);

    if(operate_parse(src, "new_password", new_password, sizeof(new_password)))
    {
        log_error("no found 'new_passwrod'");
        string_printf(buf, "-ERR No found 'new_password'"CRLF);
        return MRT_ERR;
    }
    snprintf(uinfo.password, sizeof(uinfo.password), "%s", new_password);

    if(email_change_password(&uinfo, 0) == MRT_ERR)
    {
        log_error("mail set password error, mail:%s, new password:%s", uinfo.remote_mail, new_password);
        string_printf(buf, "-ERR Set password error."CRLF);
        return MRT_SUC;
    }

    log_info("mail Set password success, mail:%s, new password:%s", uinfo.remote_mail, new_password);

    string_printf(buf, "+OK Set password success."CRLF);

    return MRT_OK;
}

int local_set_pwd(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;
    char new_password[SMALL_STR] = {0};

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, local_mail);

    if(operate_parse(src, "new_password", new_password, sizeof(new_password)))
    {
        log_error("no found 'new_passwrod'");
        string_printf(buf, "-ERR No found 'new_password'"CRLF);
        return MRT_ERR;
    }
    snprintf(uinfo.password, sizeof(uinfo.password), "%s", new_password);

    if(email_change_password(&uinfo, 0) == MRT_ERR)
    {
        log_error("mail set password error, mail:%s, new password:%s", uinfo.local_mail, new_password);
        string_printf(buf, "-ERR Set password error."CRLF);
        return MRT_SUC;
    }

    log_info("mail Set password success, mail:%s, new password:%s", uinfo.local_mail, new_password);

    string_printf(buf, "+OK Set password success."CRLF);

    return MRT_OK;
}





int remote_set_status(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;
    char val[SMALL_STR] = {0};

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, remote_mail);
    OPERATE_PARSE_REQUIRED(src, password);

    p_zero(val);
    if(operate_parse(src, "status", val, sizeof(val)))
    {
        log_info("no found `status`, use default:`0`");
        uinfo.status = 0;
    }
    else
    {
        uinfo.status = atoi(val);
    }

    if(email_change_status(&uinfo, 0) == MRT_ERR)
    {
        log_error("mail change status error, mail:%s, new status:%d", uinfo.remote_mail, uinfo.status);
        string_printf(buf, "-ERR Change status error."CRLF);
        return MRT_SUC;
    }

    log_info("mail change status success, mail:%s, new status:%d", uinfo.remote_mail, uinfo.status);

    string_printf(buf, "+OK Change status success."CRLF);

    return MRT_OK;
}


int local_set_status(request_t *rq)
{
    M_cpvril(rq);
    string_t *buf = rq->resp;
    user_info_t uinfo;
    char *src = NULL;
    char val[SMALL_STR] = {0};

    s_zero(uinfo);

    src = rq->dat;

    OPERATE_PARSE_REQUIRED(src, local_mail);
    OPERATE_PARSE_REQUIRED(src, password);

    p_zero(val);
    if(operate_parse(src, "status", val, sizeof(val)))
    {
        log_info("no found `status`, use default:`0`");
        uinfo.status = 0;
    }
    else
    {
        uinfo.status = atoi(val);
    }

    if(email_change_status(&uinfo, 0) == MRT_ERR)
    {
        log_error("mail change status error, mail:%s, new status:%d", uinfo.local_mail, uinfo.status);
        string_printf(buf, "-ERR Change status error."CRLF);
        return MRT_SUC;
    }

    log_info("mail change status success, mail:%s, new status:%d", uinfo.local_mail, uinfo.status);

    string_printf(buf, "+OK Change status success."CRLF);

    return MRT_OK;
}




int command_noop(request_t *rq)
{
    M_cpvril(rq);
    char rbuf[MAX_LINE] = {0};
    string_t *buf = rq->resp;

    snprintf(rbuf, sizeof(rbuf), "Opt:%s Arg:%s", rq->opt, rq->dat);
    log_info("recv:%s", rbuf);

    string_printf(buf, "+OK recv:(%s)"CRLF, rbuf);

    return MRT_OK;
}


int command_quit(request_t *rq)
{
    M_cpvril(rq);
 //   string_t *buf = rq->resp;

    log_info("operate:%s", rq->opt);

//    string_printf(buf, "+OK operate:%s"CRLF, rq->opt);

    return OPT_QUIT;
}


#define OPT_FIELD(func, state, min, max) {#func, 0, func, state, min, max}
#define OPT_EMPTY() {NULL, 0, NULL, 0, 0, 0}

operate_t opts[] = {
    OPT_FIELD(command_noop,     OPT_ANY,    1,  2),  //测试用
    OPT_FIELD(remote_register,  OPT_ANY,    1,  2),  //用户注册
    OPT_FIELD(local_register,   OPT_ANY,    1,  2),
    OPT_FIELD(remote_auth,      OPT_ANY,    1,  2),  //用户验证密码
    OPT_FIELD(local_auth,       OPT_ANY,    1,  2),
    OPT_FIELD(remote_check,     OPT_ANY,    1,  2),  //检测用户是否存在
    OPT_FIELD(local_check,      OPT_ANY,    1,  2),
    OPT_FIELD(remote_mod_pwd,   OPT_ANY,    1,  2),  //验证老密码，如果一致修改为密码
    OPT_FIELD(local_mod_pwd,    OPT_ANY,    1,  2),
    OPT_FIELD(remote_set_pwd,   OPT_ANY,    1,  2),  //重置密码，不验证旧密码
    OPT_FIELD(local_set_pwd,    OPT_ANY,    1,  2),
    OPT_FIELD(remote_set_status,OPT_ANY,    1,  2),  //重置用户状态
    OPT_FIELD(local_set_status, OPT_ANY,    1,  2),
    OPT_FIELD(command_quit,     OPT_ANY,    1,  1),  //退出
    OPT_EMPTY()
};

int operate_init()
{
    int i= 0;

    while(opts[i].name)
    {
        opts[i].len = strlen(opts[i].name);
        i++;
    }

    return MRT_OK;
}


int operate_parse(char *src, char *key, char *val, int len)
{
    char nkey[MAX_ID] = {0};
    char *pb = NULL, *pe = NULL;

    sprintf(nkey, "%s=", key);

    if(strncmp(nkey, src, strlen(nkey)) == 0)
    {
        pb = src + strlen(nkey);
    }
    else
    {
        sprintf(nkey, " %s=", key);
        if(!(pb = strstr(src, nkey)))
        {
            log_error("cmd:%s, no found `%s`", src, key);
            return MRT_ERR;
        }

        pb += strlen(nkey);
    }

    if( !pb || !*pb || (!(pe = strchr(pb, ' '))))
    {
        log_error("cmd:%s key:%s value error.", src, key);
        return MRT_ERR;
    }

    if((pe - pb) > len)
    {
        log_error("cmd:%s key:%s, value length error.", src, key);
        return MRT_ERR;
    }

    *pe = 0;
    sprintf(val, "%s", pb);
    *pe = ' ';

    return MRT_SUC;
}

int operate_find(request_t *rq, operate_t **op)
{
    char *pbuf = NULL, *pb = NULL;
    string_t *buf = rq->req;
    int len = 0, blen = 0;
    int cid = 0;

    if(buf->len < 4)
    {
        return MRT_ERR;
    }

    pbuf = buf->str;
    blen = buf->len;

    if(pbuf[blen-2] == '\r' && pbuf[blen-1] == '\n')
    {
        pbuf[blen-2] = ' ';
        pbuf[blen-1] = 0;
    }
    else
    {
        log_error("command:%s error, no found `\\r\\n`.", pbuf);
        return MRT_ERR;
    }

    if((pb = strchr(pbuf, ' ')))
    {
        *pb = 0;
        rq->opt = pbuf;
        rq->dat = pb+1;
    }
    else
    {
        rq->opt = rq->dat = pbuf;
    }

    len = strlen(rq->opt);

    while(opts[cid].name)
    {
        if((opts[cid].len == len) && (!strcmp(opts[cid].name, rq->opt)))
        {
            *op = &opts[cid];
            return MRT_SUC;
        }
        cid++;
    }

    log_error("No found command:[%s].", rq->opt);

    return MRT_ERR;
}


