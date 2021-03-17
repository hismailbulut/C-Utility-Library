#include "Timer.h"

#include <string.h>
#include <time.h>

#include "Debug.h"

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

long TimerGetElapsed(Timer* timer) {
    long time = clock();
    return time - timer->startTime;
}

void TimerLogElapsed(Timer* timer) {
    DEBUG_LOG_INFO("Timer %s's elapsed time is %ld ms.",
                   timer->name, TimerGetElapsed(timer));
}

void TimerWait(long ms) {
    long start = clock();
    while (clock() - start < ms) {
    }
}
