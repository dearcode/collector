#include "global.h"
#include "macro_const.h"
#include "macro_func.h"
#include "comm_func.h"
#include <stdarg.h>
#include <syslog.h>
#include <execinfo.h>

extern char *error_msg;

S_logger logger;

S_level log_level_ary[] = {
	{.level = MRT_DEBUG,.desc = "DEBUG"},
	{.level = MRT_INFO,.desc = "INFO"},
	{.level = MRT_WARNING,.desc = "WARNING"},
	{.level = MRT_ERROR,.desc = "ERROR"},
	{.level = MRT_FATAL,.desc = "FATAL"}
};

static int logger_start = 0;

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

int open_log()
{
	time_t now;
	struct tm *ntm;
	mode_t old_umask;
	int logfd = -1;
	char ctm[MAX_TIME] = { 0 };

	time(&now);
	ntm = localtime(&now);

	strftime(ctm, sizeof(ctm), "%Y%m%d", ntm);

	if ((logger.nfd > 0) && !strcmp(logger.otm, ctm)) {
		logfd = logger.nfd;
	} else {
		if (logger.nfd > 0)
			close(logger.nfd);

		char nstr[MAX_PATH] = { 0 };

		sprintf(nstr, "%s/%s_%s.log", logger.path, logger.prefix, ctm);

		old_umask = umask(0111);

		if ((logfd = open(nstr, O_WRONLY | O_APPEND | O_CREAT, 0666)) == MRT_ERR) {
			set_error("%s open file:[%s] error, %m.", __func__, nstr);
			umask(old_umask);
			return MRT_ERR;
		}

		s_zero(logger.otm);
		sprintf(logger.otm, "%s", ctm);
		logger.nfd = logfd;
		umask(old_umask);
	}

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

	strcpy(logger.path, path);
	//snprintf(logger.prefix, sizeof(logger.prefix), "%s_%s", hostname, prefix);
	snprintf(logger.prefix, sizeof(logger.prefix), "%s", prefix);

	if ((open_log() == MRT_ERR)) {
		set_error("%s open logfile error.\n", __func__);
		return MRT_ERR;
	}

	logger_start = 1;

	return MRT_OK;
}


int logger_write(int type, char *level, const char *fmt, ...)
{
	static int body_size = 0;
	static char *body = NULL;

	va_list ap;
	int h = 0, b = 0;
	time_t now;
	struct tm t;

#define POSTFIX 2

	time(&now);
	localtime_r(&now, &t);

	do {
		if (b > body_size - POSTFIX) {
			if (!(body = realloc(body, b + POSTFIX))) {
				set_error("realloc buffer size:%d, error:%m", b + 1);
				return MRT_ERR;
			}
			body_size = b + POSTFIX;
		}

		h = snprintf(body, body_size, "%.2d:%.2d:%.2d [%u] [%s] ", t.tm_hour, t.tm_min, t.tm_sec, getpid(), level);
		if (h >= body_size) {
			b = h;
			continue;
		}

		va_start(ap, fmt);
		b = vsnprintf(body + h, body_size - h, fmt, ap);
		va_end(ap);

		b += h;

	} while (b > body_size - POSTFIX);

	body[body_size - POSTFIX] = '\n';
	body[body_size - 1] = 0;

	if (!logger_start) {
		printf("%s", body);
		return MRT_SUC;
	}

	if (open_log() == MRT_OK) {
		if (write(logger.nfd, body, body_size) < 0) {
			set_error("Write log file error, file:%s, %m.", logger.prefix);
			return MRT_ERR;
		}
	}

	return MRT_SUC;
}

int logger_destroy()
{
	close(logger.nfd);
	s_zero(logger);

	return MRT_OK;
}

void log_backtrace()
{
	void *array[16];
	size_t size;

	size = backtrace(array, 16);

	backtrace_symbols_fd(array, size, logger.nfd);
}

