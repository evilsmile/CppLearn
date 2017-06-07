#ifndef __LOG_H__
#define __LOG_H__

#include <string>

enum LOG_LEVEL {
	ERROR=1,
	WARNING=2,
	TRACE=3,
	DEBUG=4
};

int init_log(const std::string& filename);

extern int log(LOG_LEVEL level, const char* file, int line, const char *function, const char* msg, ...);

#define log_error(...) log(ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define log_trace(...) log(TRACE, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define log_warning(...)  log(WARNING, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#endif
