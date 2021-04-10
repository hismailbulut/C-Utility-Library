#include "Timer.h"

#include <string.h>
#include <time.h>

#include "Debug.h"

#ifdef __cplusplus
extern "C" {
#endif

Timer TimerCreate(const char* name, bool start) {
    Timer timer;
    memset(&timer, 0, sizeof(timer));
    timer.name = name;
    if (start) {
        TimerStart(&timer);
    }
    return timer;
}

void TimerStart(Timer* timer) {
    timer->startTime = clock();
}

void TimerPause(Timer* timer) {
    timer->pausedTime = clock();
}

void TimerResume(Timer* timer) {
    timer->startTime += (clock() - timer->pausedTime);
}

double TimerGetElapsed(Timer* timer) {
    long time = clock();
    return (double)(time - timer->startTime) / CLOCKS_PER_SEC;
}

void TimerLogElapsed(Timer* timer) {
    DEBUG_LOG_INFO("Timer '%s' elapsed time is %f seconds.", timer->name, TimerGetElapsed(timer));
}

#ifdef __cplusplus
}
#endif
