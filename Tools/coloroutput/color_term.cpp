#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <stdarg.h>

const std::string LEVEL_COLOR[] =
{
    "\033[0;36m",/* clan */
        "\033[01;34m",/* light blue */
        "\033[01;34m",/* light blue */
        "\033[01;32m",/* light green */
        "\033[01;31m",/* light red */
        "\033[01;31m",/* light red */
        "\033[01;31m",/* light red */
        "\033[01;35m",/* light magenta */
        "\033[01;35m" /* light magenta */
};

const std::string LEVEL_NAME[] =
{
    "DEBUG",
    "INFO",
    "NOTICE",
    "WARN",
    "ALERT",
    "ERROR",
    "CRIT",
    "EMERG",
    "FATAL"
};

enum Level
{   
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_NOTICE,
    LEVEL_WARN,
    LEVEL_ALERT,
    LEVEL_ERROR,
    LEVEL_CRIT,
    LEVEL_EMERG,
    LEVEL_FATAL
}; 

#define MAX_LOG_LENGTH 4096

void Log(Level level, const char *file, const int line, const char *func, const char *format, ...)
{
    const std::string &levelName = LEVEL_NAME[level];
    std::string strClour = LEVEL_COLOR[level];

    char strLog[MAX_LOG_LENGTH + 1] = {0};
    char filename[128] = {0};

    char *p = strrchr(const_cast<char*>(file), '/');
    if(p == NULL)
    {
        snprintf(filename, 128,"%s", file);
    }
    else
    {
        snprintf(filename, 128,"%s", p + 1);
    }
    snprintf(strLog, MAX_LOG_LENGTH, "%s[%s][%s:%d][%s] ",  strClour.data(), levelName.data(), filename, line, func);

    va_list argp;
    va_start(argp,format);
    /////vsnprintf(&strLog[strlen(strLog)], MAX_LOG_LENGTH - strlen(strLog), format, argp);
    vsnprintf(strLog+strlen(strLog), MAX_LOG_LENGTH - strlen(strLog), format, argp);
    va_end(argp);

    snprintf(strLog+strlen(strLog), MAX_LOG_LENGTH - strlen(strLog), "\033[0m");     //for colour
    std::cout << strLog << std::endl;
}

#define DebugLog(...)    \
    Log(LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define NoticeLog(...)    \
    Log(LEVEL_NOTICE, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define InfoLog(...)    \
    Log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define WarnLog(...)    \
    Log(LEVEL_WARN, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define AlertLog(...)    \
    Log(LEVEL_ALERT, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define ErrorLog(...)    \
    Log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define CritLog(...)    \
    Log(LEVEL_CRIT, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define EmergLog(...)    \
    Log(LEVEL_EMERG, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define FatalLog(...)    \
    Log(LEVEL_FATAL, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

int main()
{
    DebugLog("This is debug!");
    NoticeLog("This is Notice!");
    InfoLog("This is Info!");
    WarnLog("This is Warn!");
    AlertLog("This is Alert!");
    ErrorLog("This is Error!");
    CritLog("This is Crit!");
    EmergLog("This is Emerg!");
    FatalLog("This is Fatal!");

    return 0;
}
