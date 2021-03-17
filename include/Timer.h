#pragma once

#include <stdbool.h>

typedef struct Timer {
    const char* name;
    long startTime;
    long pausedTime;
} Timer;

Timer TimerCreate(const char* name, bool start);

void TimerStart(Timer* timer);

void TimerPause(Timer* timer);

void TimerResume(Timer* timer);

long TimerGetElapsed(Timer* timer);

void TimerLogElapsed(Timer* timer);

void TimerWait(long ms);
