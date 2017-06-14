#include <string>
#include <sys/time.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#include "log.h"
#define LOG_BUF_SIZ 4096

static std::string log_dir = "/tmp/log/";
static std::string log_file;

static std::string now_time()
{
	std::string now;
	now.resize(512);

	struct timeval tv;
	struct tm tm;
	gettimeofday(&tv, 0);
	localtime_r(&tv.tv_sec, &tm);
	int len = strftime((char*)now.c_str(), now.size(), "%Y%m%d %H:%M:%S", &tm);
	len += sprintf((char*)now.c_str() + len, "%.06d", (int)(tv.tv_usec));

	now.resize(len);

	return now;
}

int init_log(const std::string& filename)
{
	log_file = filename + ".log";

    log_trace("--------------- %s start ----------", filename.c_str());

	return 0;
}

int log(LOG_LEVEL level, const char* file, int line, const char *function, const char* msg, ...)
{
	std::string s_level;
	switch(level) {
			case ERROR:
					s_level = "ERROR";
					break;
			case WARNING:
					s_level = "WARNING";
					break;
			case TRACE:
					s_level = "TRACE";
					break;
			case DEBUG:
					s_level = "DEBUG";
					break;
			default:
					s_level = "DEBUG";
					break;
	}

	int n = 0;
	char buf[LOG_BUF_SIZ] = {0};

	n += sprintf(buf+n , "[%s][%s][%s:%d][%s][%ld] ", now_time().c_str(), s_level.c_str(), file, line, function, (long)pthread_self());

	va_list arg_ptr;
	va_start(arg_ptr, msg);
	int ret = vsnprintf(buf+n, LOG_BUF_SIZ-n, msg, arg_ptr);
	va_end(arg_ptr);
	if (ret >= LOG_BUF_SIZ) {
			n = LOG_BUF_SIZ;
	} else {
			n += ret;
	}

	buf[n++] = '\n';
	buf[n] = 0;
	

    if (!log_file.empty()) {
        std::string log_file_path = log_dir + "/" + log_file;
        std::ofstream out;
        out.open(log_file_path.c_str(), std::ofstream::out | std::ofstream::app);
        out << buf;
        out.close();
    } else {
        std::cout << buf;
    }

	return 0;
}
