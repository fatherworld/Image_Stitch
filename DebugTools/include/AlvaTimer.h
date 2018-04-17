#ifndef _TIMER_H_FL
#define _TIMER_H_FL

#include <Windows.h>
#include <time.h>

typedef struct _Timer_{
    long int start;
    long int end;
}Timer;

#ifdef __cplusplus
extern "C"
{
#endif
void startTime(Timer* timer);
void stopTime(Timer* timer);
long diffTime(Timer timer);
#ifdef __cplusplus
}
#endif

#endif
