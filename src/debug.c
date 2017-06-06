#include "debug.h"
#include "stringbuilder.h"

#include <stdlib.h>
#include <stdio.h>

#include <stdarg.h>
#include <time.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#   include <config.h>
#   ifdef HAVE__BOOL
#       include <stdbool.h>
#   else
        typedef enum tagbool { false = 0, true } bool;
#   endif
#else
typedef enum tagbool { false = 0, true } bool;
#endif

#define TRACE_LEVEL_DEBUG 1
#define TRACE_LEVEL_INFO (TRACE_LEVEL_DEBUG + 1)
#define TRACE_LEVEL_WARN (TRACE_LEVEL_INFO + 1)
#define TRACE_LEVEL_ERROR (TRACE_LEVEL_WARN + 1)

#define DEBUG
#define DEBUG_LEVEL TRACE_LEVEL_WARN

#ifdef DEBUG
#   ifdef HAVE_LOCALTIME_R
static bool getCurrentTimeString(char *const buf, size_t const n, char const *const fmt) {
    char timebuf[80] = "<FAILED FETCH TIME>";
    time_t rawtime;
    struct tm timeinfo = {0};
    size_t len = 0;
    if (time(&rawtime) == -1) {
        len = strlen(&timebuf[0]);
        if (n > len)
            strcpy(buf, &timebuf[0]);
        return false;
    }
    localtime_r(&rawtime, &timeinfo);
    len = strftime(&timebuf[0], sizeof(timebuf), fmt, &timeinfo);
    if (n > len) {
        strcpy(buf, &timebuf[0]);
        return true;
    }
    return false;
}
#   else
static bool getCurrentTimeString(char *const buf, size_t const n, char const *const fmt) {
    char timebuf[80] = "<FAILED FETCH TIME>";
    time_t rawtime;
    struct tm *timeinfo;
    size_t len = 0;
    if (time(&rawtime) == -1) {
        len = strlen(&timebuf[0]);
        if (n > len)
            strcpy(buf, &timebuf[0]);
        return false;
    }
    timeinfo = localtime(&rawtime);
    len = strftime(&timebuf[0], sizeof(timebuf), fmt, timeinfo);
    if (n > len) {
        strcpy(buf, &timebuf[0]);
        return true;
    }
    return false;
}
#   endif
#endif

static void ns_vtrace(FILE *stream,
        char const *file, size_t const line,
        char const *function,
        char const *const tag,
        char const *const fmt, va_list args) {
#ifdef DEBUG
    char timebuf[80];
    StringBuilder *builder = NULL;
    builder = StringBuilderCreate();
    getCurrentTimeString(&timebuf[0], sizeof(timebuf), "%Y-%m-%d %H:%M:%S");
    StringBuilderAppend(builder, "[%s][%s][%s:%d][%s] ", &timebuf[0], tag, file, line, function);
    vStringBuilderAppend(builder, fmt, args);
    fprintf(stream, "%s\n", StringBuilderInternalString(builder));
    fflush(stream);
    StringBuilderDestroy(builder);
#endif
}

void ns_trace(FILE *stream,
        char const *file, size_t const line,
        char const *function,
        char const *const tag,
        char const *const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ns_vtrace(stream, file, line, function, tag, fmt, args);
    va_end(args);
}

#define TRACE_DEFINE(LEVEL_NAME, TAG, STREAM, LEVEL) \
    void ns_trace_##LEVEL_NAME(char const *file, size_t const line, char const *function, char const *const fmt, ...) { \
        va_list args; \
		if (LEVEL >= DEBUG_LEVEL) { \
			va_start(args, fmt); \
			ns_vtrace(STREAM, file, line, function, TAG, fmt, args); \
			va_end(args); \
		} \
    }

TRACE_DEFINE(warn,  "WARN",     stdout, TRACE_LEVEL_WARN)
TRACE_DEFINE(debug, "DEBUG",    stdout, TRACE_LEVEL_DEBUG)
TRACE_DEFINE(info,  "INFO",     stdout, TRACE_LEVEL_INFO)
TRACE_DEFINE(error, "ERROR",    stderr, TRACE_LEVEL_ERROR)
