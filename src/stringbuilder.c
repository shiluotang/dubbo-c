#include "stringbuilder.h"

#include <stdlib.h>
#include <stdio.h>
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

#ifndef va_copy
#	define va_copy(dest, src) dest = src
#endif

size_t const INITIAL_STRINGBUILDER_CAPACITY = 16U;

static int StringBuilderRemainingSpace(StringBuilder const *builder) {
    return builder->_M_capacity - builder->_M_used;
}

static char* StringBuilderAddress(StringBuilder const *builder) {
    return builder->_M_data + builder->_M_used;
}

static bool StringBuilderEnlarge(StringBuilder *builder) {
    char *newAddress;
    size_t newCapacity;
	char *oldAddress = builder->_M_data;
    newCapacity = builder->_M_capacity * 2;
    newAddress = (char*) malloc(newCapacity);
    if (newAddress == NULL)
        return false;
    memcpy(newAddress, builder->_M_data, builder->_M_used);
	free(oldAddress);
    builder->_M_capacity = newCapacity;
    builder->_M_data = newAddress;
    return true;
}

StringBuilder* StringBuilderCreate() {
    StringBuilder *builder = NULL;
    builder = (StringBuilder*) malloc(sizeof(StringBuilder));
    if (builder == NULL)
        return builder;
    memset(builder, 0, sizeof(StringBuilder));
    builder->_M_data = (char*) malloc(INITIAL_STRINGBUILDER_CAPACITY);
    builder->_M_capacity = INITIAL_STRINGBUILDER_CAPACITY;
    if (builder->_M_data == NULL) {
        StringBuilderDestroy(builder);
        builder = NULL;
    } else
        memset(builder->_M_data, 0, INITIAL_STRINGBUILDER_CAPACITY);
    return builder;
}

void StringBuilderDestroy(StringBuilder *builder) {
    if (builder != NULL) {
        if (builder->_M_data != NULL) {
            free(builder->_M_data);
            builder->_M_data = NULL;
            builder->_M_used = 0U;
            builder->_M_capacity = 0U;
        }
        free(builder);
    }
}

int StringBuilderAppend(StringBuilder *builder, char const *fmt, ...) {
    va_list args;
    int n;
    va_start(args, fmt);
    n = vStringBuilderAppend(builder, fmt, args);
    va_end(args);
    return n;
}

int vStringBuilderAppend(StringBuilder *builder, char const *fmt, va_list args) {
    va_list copy;
    int n;
    int max;
    do {
        max = StringBuilderRemainingSpace(builder);
        va_copy(copy, args);
        n = vsnprintf(StringBuilderAddress(builder), max, fmt, copy);
		va_end(copy);
        /* if (n < 0) return n; */
        if (n < max && n >= 0)
            break;
        if (!StringBuilderEnlarge(builder))
            return -1;
    } while (true);
    builder->_M_used += n;
    return n;
}

char const* StringBuilderInternalString(StringBuilder const *builder) {
    return builder->_M_data;
}

void StringBuilderClear(StringBuilder *builder) {
    builder->_M_used = 0U;
}
