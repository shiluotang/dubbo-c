#include "../src/dubbo-client.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* *argv) {
    int rc = 0;
    DubboResult r = {0};
    DubboClient *client = NULL;

    client = DubboClientCreate("127.0.0.1", 54321);
    if (!DUBBO_SUCCEEDED(r = DubboExecute(client, "invoke org.sqg.HelloWorld.greetings(\"abc\")\n")))
        goto failure;
    printf("%s\n", (char const*) r._M_data);

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
    goto exit;
}
