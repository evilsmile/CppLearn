#include <iostream>
#include <time.h>

int main()
{
#define SEC_PER_DAY 24*60*60
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    int iDaysToMonday = 0;
    if (tm_now.tm_wday == 0)
    {
        iDaysToMonday = 6;
    }
    else
    {
        iDaysToMonday = tm_now.tm_wday - 1;
    }
    time_t secToMonday = iDaysToMonday * SEC_PER_DAY;
    time_t monday = now - secToMonday;
    struct tm tmMonday;
    localtime_r(&monday, &tmMonday);
    char szDate[64] = {0};
    strftime(szDate, sizeof(szDate), "%Y%m%d", &tmMonday);

    std::cout << szDate << std::endl;

    return 0;
}
