#ifndef DUBBO_C_DUBBO_H_INCLUDED
#define DUBBO_C_DUBBO_H_INCLUDED

#include <stddef.h>
#include "networkstream.h"

#ifdef __cplusplus
extern "C" {
#endif

#   define DUBBO_FAIL_NETWORK   (-2)
#   define DUBBO_FAIL_DATA      (-1)
#   define DUBBO_FAIL_OOM       (-1)
#   define DUBBO_SUCCESS        (0)


    struct tagDubboClient;

    typedef struct tagDubboClient DubboClient;

    struct tagDubboResult;

    typedef struct tagDubboResult DubboResult;

    struct tagDubboClient {
        char        _M_host[0xff];
        size_t      _M_port;
        NetworkStream *_M_stream;
    };

    struct tagDubboResult {
        int     _M_status;
        void    *_M_data;
        size_t  _M_size;
    };

#   define DUBBO_SUCCEEDED(rc) ((rc)._M_status >= 0)

    extern DubboClient* DubboClientCreate(char const*, size_t);

    extern void DubboClientDestroy(DubboClient*);

    extern DubboResult DubboExecute(DubboClient *client, char const *cmd);

    extern void DubboResultCleanup(DubboResult *r);

#ifdef __cplusplus
}
#endif

#endif /* DUBBO_C_DUBBO_H_INCLUDED */
