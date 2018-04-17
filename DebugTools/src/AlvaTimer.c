#include "AlvaTimer.h"
#define TIME_us 1.0f
#define TIME_ms 1000.0f
#define TIME_s  1000000.0f

static int gettimeofday(struct timeval* tp, void* tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = (long)clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;

    return 0;
}

void startTime(Timer* timer){
    struct timeval cs;
    gettimeofday(&cs, NULL);
    timer->start = cs.tv_sec * 1000000 + cs.tv_usec;
}

void stopTime(Timer* timer){
    struct timeval ce;
    gettimeofday(&ce, NULL);
    timer->end = ce.tv_sec * 1000000 + ce.tv_usec;
}

long diffTime(Timer timer){
    return (timer.end - timer.start);
}