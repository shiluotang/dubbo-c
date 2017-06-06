#ifndef STOPWATCH_H_INCLUDED
#define STOPWATCH_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

    struct tagStopwatch;
    typedef struct tagStopwatch Stopwatch;

    extern Stopwatch* StopwatchCreate();
    extern void StopwatchDestroy(Stopwatch*);

    extern int StopwatchStart(Stopwatch*);
    extern int StopwatchStop(Stopwatch*);
    extern int StopwatchReset(Stopwatch*);

    extern double StopwatchElapsedSecs(Stopwatch const*);
    extern double StopwatchElapsedMillis(Stopwatch const*);
    extern double StopwatchElapsedMicros(Stopwatch const*);
    extern double StopwatchElapsedNanos(Stopwatch const*);

#ifdef __cplusplus
}
#endif

#endif /* STOPWATCH_H_INCLUDED */
