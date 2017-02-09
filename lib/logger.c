#include "global.h"
#include "macro_const.h"
#include "macro_func.h"
#include "comm_func.h"
#include <stdarg.h>
#include <syslog.h>
#include <execinfo.h>

extern char    *error_msg;

logger_t        logger;

logger_level_t  log_level_ary[] = {
	{.level = MRT_DEBUG,.desc = "DEBUG",.color = "\033[0;37m"},
	{.level = MRT_INFO,.desc = "INFO",.color = "\033[0;32m"},
	{.level = MRT_WARNING,.desc = "WARNING",.color = "\033[0;33m"},
	{.level = MRT_ERROR,.desc = "ERROR",.color = "\033[0;31m"},
	{.level = MRT_FATAL,.desc = "FATAL",.color = "\033[0;31m"}
};

static inline int level_check(int err_code)
{
	return (err_code < logger.level) ? MRT_ERR : MRT_OK;
}

static int path_check(char *path)
{
	if (access(path, W_OK) == MRT_ERR) {
		set_error("%s path:%s can't write, %m.", __func__, path);
		return MRT_ERR;
	}

	return MRT_OK;
}

int open_log(struct tm *ntm)
{
	mode_t          old_umask;

	if (!logger.start) {
		if (logger.fp == NULL) {
			logger.fp = stdout;
            logger.enable_color = 1;
		}
		return MRT_OK;
	}

	if (logger.fp != NULL && logger.current_day == ntm->tm_mday) {
		return MRT_OK;
	}

	if (logger.fp != NULL)
		fclose(logger.fp);

	strftime(logger.path + logger.prefix_len, sizeof(logger.path) - logger.prefix_len, "_%Y%m%d.log", ntm);

	old_umask = umask(0111);

	if ((logger.fp = fopen(logger.path, "a+")) == NULL) {
		set_error("%s open file:[%s] error, %m.", __func__, logger.path);
		umask(old_umask);
		return MRT_ERR;
	}

	umask(old_umask);

	return MRT_OK;
}

int logger_init(char *path, char *prefix, int level)
{
//    char hostname[MAX_USER] = {0};

	if (!path || !*path || !prefix || !*prefix) {
		set_error("%s parameter error.\n", __func__);
		return MRT_ERR;
	}

	s_zero(logger);

	M_cirinl(path_check(path));
	logger.level = level;

	/*
	   if(gethostname(hostname, sizeof(hostname) - 1) == MRT_ERR)
	   {
	   set_error("%s get host name error, %m.\n", __func__);
	   return MRT_ERR;
	   }

	 */

	logger.prefix_len = sprintf(logger.path, "%s/%s", path, prefix);
	//snprintf(logger.prefix, sizeof(logger.prefix), "%s_%s", hostname, prefix);

	logger.start = 1;
    logger.enable_color = 0;

	return MRT_OK;
}

int logger_write(int type, char *level, const char *fmt, ...)
{
	va_list         args;
	time_t          now;
	struct tm       t;

	pthread_mutex_lock(&logger.mtx);

	time(&now);
	localtime_r(&now, &t);

	open_log(&t);

    if (logger.enable_color) {
        fprintf(logger.fp, log_level_ary[type - 1].color);
    }

	fprintf(logger.fp, "%.2d:%.2d:%.2d [%u] [%s] ", t.tm_hour, t.tm_min, t.tm_sec, getpid(), level);

	va_start(args, fmt);
	vfprintf(logger.fp, fmt, args);
	va_end(args);

    if (logger.enable_color) {
        fprintf(logger.fp, "\033[0m");
    }

	pthread_mutex_unlock(&logger.mtx);

	return MRT_SUC;
}

int logger_destroy()
{
	fclose(logger.fp);
	s_zero(logger);
	return MRT_OK;
}

void log_backtrace()
{
	void           *array[16];
	size_t          size;
	size = backtrace(array, 16);
	backtrace_symbols_fd(array, size, fileno(logger.fp));
}
