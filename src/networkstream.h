#ifndef NETWORKSTREAM_NETWORKSTREAM_H_INCLUDED
#define NETWORKSTREAM_NETWORKSTREAM_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef int SOCKETHANDLE;

    typedef struct tagNetworkStream {
        SOCKETHANDLE _M_socket;
        int _M_isClosed;
    } NetworkStream;

    extern NetworkStream*   NetworkStreamCreate(char const*, int const);
    extern int              NetworkStreamRead(NetworkStream*, void*, size_t);
    extern int              NetworkStreamWrite(NetworkStream*, void const*, size_t);
    extern void             NetworkStreamDestroy(NetworkStream*);
    extern int              NetworkStreamIsClosed(NetworkStream const*);

#ifdef __cplusplus
}
#endif

#endif /* NETWORKSTREAM_NETWORKSTREAM_H_INCLUDED */
