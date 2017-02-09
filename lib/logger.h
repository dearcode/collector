#ifndef __LOG_FUNC_H__
#define __LOG_FUNC_H__

typedef struct {
	int             start;
	char            path[MAX_PATH];
	int             level;
	int             current_day;
	int             prefix_len;
	int             enable_color;
	FILE           *fp;
	string_t        line;
	pthread_mutex_t mtx;

} logger_t;

typedef struct {
	char           *color;
	uint16_t        level;
	char           *desc;
} logger_level_t;

// ----------- 函数声明 ----------------

// 初始化日志系统
int             logger_init(char *path, char *prefix, int level);

// 记录日志
int             logger_write(int type, char *level, const char *fmt, ...) __attribute__ ((format(printf, 3, 4)));

// 结果日志记录
int             logger_destroy();

void            log_backtrace();
// -------------------------------------

#endif
