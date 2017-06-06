#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#define TEST_ASSERT(CONDITION) \
    if (!(CONDITION)) { \
        fprintf(stderr, "[Assert] %s failed\n", #CONDITION); \
        goto failure; \
    }

#define TEST_EQUAL(A, B)    TEST_ASSERT((A) == (B))
#define TEST_NEQUAL(A, B)   TEST_ASSERT((A) != (B))
#define TEST_LESS(A, B)     TEST_ASSERT((A) <  (B))
#define TEST_NLESS(A, B)    TEST_ASSERT((A) >= (B))
#define TEST_GREATER(A, B)  TEST_ASSERT((A) >  (B))
#define TEST_NGREATER(A, B) TEST_ASSERT((A) <= (B))
#define TEST_NULL(X)        TEST_EQUAL(X, NULL)
#define TEST_NNULL(X)       TEST_NEQUAL(X, NULL)

#define FPRINTF(FP, FMT, VALUE)  fprintf(FP, "%s = " FMT "\n", #VALUE, VALUE);
#define PRINTF(FMT, VALUE) FPRINTF(stdout, FMT, VALUE)

#define TEST_START(rc) \
    int main(int argc, char* *argv) { \
        int rc = 0;
#define TEST_STOP_AND_CLEAN(rc, CLEANUP) \
        goto success; \
        exit: return rc; \
        failure: rc = EXIT_FAILURE; goto cleanup; \
        success: rc = EXIT_SUCCESS; goto cleanup; \
        cleanup: { CLEANUP } goto exit; \
    }
#define TEST_STOP(rc) TEST_STOP_AND_CLEAN(rc, {})

#endif /* TEST_H_INCLUDED */
