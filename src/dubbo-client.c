#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dubbo-client.h"
#include "networkstream.h"
#include "stringbuilder.h"

#define DUBBO_RESULT_END "\r\ndubbo>"
#define DUBBO_RESPONSE_END "\r\nelapsed: "

static char* strrstr(char const *search, char const *needle) {
    size_t nlen = 0;
    char *prev = NULL;
    char *cur = NULL;

    nlen = strlen(needle);

    while ((cur = strstr(search, needle))) {
        search = cur + nlen;
        prev = cur;
    }
    return prev;
}

static int IsEndWiths(char const *search, char const *suffix) {
    char *p = strrstr(search, suffix);
    return p && *(p + strlen(suffix)) == '\0';
}

#define DUBBO_EXIT_CMD "exit"

static int DubboClientIsConnected(DubboClient const *client) {
    return client->_M_stream != NULL && !NetworkStreamIsClosed(client->_M_stream);
}

static int DubboClientConnect(DubboClient *client) {
    return (client->_M_stream = NetworkStreamCreate(&client->_M_host[0],
            client->_M_port)) != NULL;
}

static int DubboClientDisconnect(DubboClient *client) {
    if (DubboClientIsConnected(client))
        NetworkStreamWrite(client->_M_stream, DUBBO_EXIT_CMD, strlen(DUBBO_EXIT_CMD));
    NetworkStreamDestroy(client->_M_stream);
    client->_M_stream = NULL;
    return 1;
}

DubboClient* DubboClientCreate(char const *host, size_t port) {
    DubboClient *client = NULL;

    client = (DubboClient*) calloc(1, sizeof(*client));
    if (!client)
        return client;
    strncpy(&client->_M_host[0], host, sizeof(client->_M_host) - 1);
    client->_M_host[sizeof(client->_M_host) - 1] = '\0';
    client->_M_port = port;

    if (!DubboClientConnect(client)) {
        DubboClientDestroy(client);
        client = NULL;
    }
    return client;
}

void DubboClientDestroy(DubboClient *client) {
    if (client) {
        DubboClientDisconnect(client);
        free(client);
        client = NULL;
    }
}

DubboResult DubboExecute(DubboClient *client, char const *cmd) {
    char buf[0xff];
    int rc = 0;
    DubboResult r = {0};
    StringBuilder *builder = NULL;
    char *p = NULL;
    char const *result = NULL;

    if (NetworkStreamWrite(client->_M_stream, cmd, strlen(cmd)) != strlen(cmd)) {
        r._M_status = DUBBO_FAIL_NETWORK;
        goto failure;
    }
    builder = StringBuilderCreate();
    if (!builder) {
        r._M_status = DUBBO_FAIL_OOM;
        goto failure;
    }
    do {
        rc = NetworkStreamRead(client->_M_stream, &buf[0], sizeof(buf) - 1);
        if (rc < 0) {
            r._M_status = DUBBO_FAIL_NETWORK;
            goto failure;
        }
        buf[rc] = '\0';
        if (StringBuilderAppend(builder, "%s", &buf[0]) < 0) {
            r._M_status = DUBBO_FAIL_NETWORK;
            goto failure;
        }
        // check reach the end?
        result = StringBuilderInternalString(builder);
        if (IsEndWiths(result, DUBBO_RESULT_END)) {
            p = strrstr(result, DUBBO_RESPONSE_END);
            if (p && (result + strlen(result) - strlen(DUBBO_RESULT_END))
                    == strstr(p, DUBBO_RESULT_END)) {
                break;
            }
        }
    } while (1);
    r._M_size = p - StringBuilderInternalString(builder);
    r._M_data = calloc(1, r._M_size + 1);
    if (!r._M_data) {
        r._M_status = DUBBO_FAIL_OOM;
        goto failure;
    }
    memcpy(r._M_data, StringBuilderInternalString(builder), r._M_size);

    goto success;
exit:
    return r;
success:
    r._M_status = DUBBO_SUCCESS;
    goto cleanup;
failure:
    goto cleanup;
cleanup:
    if (builder) {
        StringBuilderDestroy(builder);
        builder = NULL;
    }
    goto exit;
}

void DubboResultCleanup(DubboResult *result) {
    if (result) {
        if (result->_M_data) {
            free(result->_M_data);
            result->_M_data = NULL;
            result->_M_size = 0;
        }
    }
}
