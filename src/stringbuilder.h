#ifndef NETWORKSTREAM_STRINGBUILDER_H_INCLUDED
#define NETWORKSTREAM_STRINGBUILDER_H_INCLUDED

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct tagStringBuilder {
        char *_M_data;
        size_t _M_used;
        size_t _M_capacity;
    } StringBuilder;

    extern StringBuilder*   StringBuilderCreate();
    extern void             StringBuilderDestroy(StringBuilder*);

    extern int              StringBuilderAppend(StringBuilder*, char const*, ...);

    extern char const*      StringBuilderInternalString(StringBuilder const*);
    extern void             StringBuilderClear(StringBuilder*);

    extern int              vStringBuilderAppend(StringBuilder*, char const*, va_list);

#ifdef __cplusplus
}
#endif

#endif /* NETWORKSTREAM_STRINGBUILDER_H_INCLUDED */
