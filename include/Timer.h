#pragma once

#include <stdbool.h>

typedef struct Timer {
    const char* name;
    long startTime;
    long pausedTime;
} Timer;

#ifdef __cplusplus
extern "C" {
#endif

Timer TimerCreate(const char* name, bool start);

// Starts or restarts the timer.
void TimerStart(Timer* timer);

void TimerPause(Timer* timer);

void TimerResume(Timer* timer);

// Returns elapsed time in seconds.
double TimerGetElapsed(Timer* timer);

// Logs elapsed time in seconds to stdout.
void TimerLogElapsed(Timer* timer);

#ifdef __cplusplus
}
#endif
