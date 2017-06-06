#include <stdlib.h>
#include <stdio.h>

#include "../src/dubbo-client.h"
#include "stopwatch.h"
#include "test.h"

int main(int argc, char* *argv) {
    int rc = 0;
    DubboResult r = {0};
    DubboClient *client = NULL;
    Stopwatch *watch = NULL;
    size_t const N = 0xfffff;
    size_t i = 0;

    TEST_NNULL(watch = StopwatchCreate());
    TEST_NNULL(client = DubboClientCreate("127.0.0.1", 54321));
    TEST_ASSERT(StopwatchStart(watch));
    for (i = 0; i < N; ++i) {
        DubboResultCleanup(&r);
        r = DubboExecute(client,
                "invoke org.sqg.HelloWorld.greetings(\"abc\")\n");
        if (!DUBBO_SUCCEEDED(r))
            goto failure;
    }
    TEST_ASSERT(StopwatchStop(watch));
    printf("%s\n", (char const*) r._M_data);
    printf("N = %lu, R = %lu, Total = %g s, Average = %g s, QPS = %g\n",
            (unsigned long) N,
            (unsigned long) i,
            StopwatchElapsedSecs(watch),
            StopwatchElapsedSecs(watch) / i,
            i / StopwatchElapsedSecs(watch)
            );

    goto success;
exit:
    return rc;
success:
    rc = EXIT_SUCCESS;
    goto cleanup;
failure:
    rc = EXIT_FAILURE;
    goto cleanup;
cleanup:
    if (client) {
        DubboClientDestroy(client);
        client = NULL;
    }
    DubboResultCleanup(&r);
    if (watch) {
        StopwatchDestroy(watch);
        watch = NULL;
    }
    goto exit;
}
