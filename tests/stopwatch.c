#include "stopwatch.h"

#ifdef HAVE_CONFIG_H
#   include <config.h>
#   ifdef HAVE_STDLIB_H
#       include <stdlib.h>
#   endif
#   ifdef TIME_WITH_SYS_TIME
#       include <sys/time.h>
#       include <time.h>
#   else
#       ifdef HAVE_SYS_TIME_H
#           include <sys/time.h>
#       else
#           include <time.h>
#       endif
#   endif
#   if defined(HAVE_GETTIMEOFDAY)
typedef struct timeval timestamp;
static int time_now(timestamp *ts) {
    return gettimeofday(ts, NULL) == 0;
}
static double difftimestamp_us(timestamp const *t2, timestamp const *t1) {
    return (t2->tv_sec - t1->tv_sec) * 1e6 + (t2->tv_usec - t1->tv_usec);
}
static double difftimestamp_s(timestamp const *t2, timestamp const *t1) {
    return difftimestamp_us(t2, t1) * 1e-6;
}
static double difftimestamp_ms(timestamp const *t2, timestamp const *t1) {
    return difftimestamp_us(t2, t1) * 1e-3;
}
static double difftimestamp_ns(timestamp const *t2, timestamp const *t1) {
    return difftimestamp_us(t2, t1) * 1e3;
}
#   endif
#endif

typedef struct tagStopwatch {
    int _M_running;
    timestamp _M_start;
    timestamp _M_current;
} Stopwatch;

Stopwatch* StopwatchCreate() {
    Stopwatch *watch = NULL;
    watch = (Stopwatch*) calloc(1, sizeof(*watch));
    if (!watch)
        return watch;
    return watch;
}

void StopwatchDestroy(Stopwatch *watch) {
    if (watch) {
        free(watch);
        watch = NULL;
    }
}

int StopwatchStart(Stopwatch *watch) {
    if (watch->_M_running)
        return 1;
    watch->_M_running = 1;
    return time_now(&watch->_M_start);
}

int StopwatchStop(Stopwatch *watch) {
    if (!watch->_M_running)
        return 1;
    watch->_M_running = 0;
    return time_now(&watch->_M_current);
}

int StopwatchReset(Stopwatch *watch) {
    if (watch->_M_running)
        return time_now(&watch->_M_start);
    return 1;
}

double StopwatchElapsedNanos(Stopwatch const *watch) {
    timestamp ts;
    if (watch->_M_running)
        time_now(&ts);
    else
        ts = watch->_M_current;
    return difftimestamp_ns(&ts, &watch->_M_start);
}

double StopwatchElapsedMicros(Stopwatch const *watch) {
    return StopwatchElapsedNanos(watch) * 1e-3;
}

double StopwatchElapsedMillis(Stopwatch const *watch) {
    return StopwatchElapsedNanos(watch) * 1e-6;
}

double StopwatchElapsedSecs(Stopwatch const *watch) {
    return StopwatchElapsedNanos(watch) * 1e-9;
}
