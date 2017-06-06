#ifndef NETWORKSTREAM_DEBUG_H_INCLUDED
#define NETWORKSTREAM_DEBUG_H_INCLUDED

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern void ns_trace(FILE *stream, char const* file, size_t const line, char const* function, char const *const tag, char const *const fmt, ...);
    extern void ns_trace_info(char const* file, size_t const line, char const* function, char const *const fmt, ...);
    extern void ns_trace_warn(char const* file, size_t const line, char const* function, char const *const fmt, ...);
    extern void ns_trace_debug(char const* file, size_t const line, char const* function, char const *const fmt, ...);
    extern void ns_trace_error(char const* file, size_t const line, char const* function, char const *const fmt, ...);

#define TRACE_INFO(FORMAT, ...)    ns_trace_info(__FILE__, __LINE__, __FUNCTION__, FORMAT, ##__VA_ARGS__)
#define TRACE_WARN(FORMAT, ...)    ns_trace_warn(__FILE__, __LINE__, __FUNCTION__, FORMAT, ##__VA_ARGS__)
#define TRACE_DEBUG(FORMAT, ...)   ns_trace_debug(__FILE__, __LINE__, __FUNCTION__, FORMAT, ##__VA_ARGS__)
#define TRACE_ERROR(FORMAT, ...)   ns_trace_error(__FILE__, __LINE__, __FUNCTION__, FORMAT, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* NETWORKSTREAM_DEBUG_H_INCLUDED */
