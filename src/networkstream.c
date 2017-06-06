#include "networkstream.h"
#include "debug.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
#   include <config.h>
#   ifdef HAVE__BOOL
#       include <stdbool.h>
#   else
        typedef enum tagbool { false = 0, true } bool;
#   endif
#   ifdef HAVE_SYS_TYPES_H
#       include <sys/types.h>
#   endif
#   ifdef HAVE_SYS_SOCKET_H
#       include <sys/socket.h>
#   endif
#   ifdef HAVE_NETDB_H
#       include <netdb.h>
#   endif
#   ifdef HAVE_UNISTD_H
#       include <unistd.h>
#   endif
#   ifdef HAVE_WINDOWS_H
#       ifndef _WIN32_WINNT
#          define _WIN32_WINNT 0x501
#       elif _WIN32_WINNT < 0x501
#          undef _WIN32_WINNT
#          define  _WIN32_WINNT 0x501
#       endif
#       include <winsock2.h>
#       include <windows.h>
#       include <ws2tcpip.h>
#       if !defined(ECONNRESET) && defined(WSAECONNRESET)
#           define ECONNRESET WSAECONNRESET
#       endif
#   endif
#else
typedef enum tagbool { false = 0, true } bool;
#	ifndef _WIN32_WINNT
#	   define _WIN32_WINNT 0x501
#	elif _WIN32_WINNT < 0x501
#	   undef _WIN32_WINNT
#	   define  _WIN32_WINNT 0x501
#	endif
#	include <winsock2.h>
#	include <windows.h>
#	include <ws2tcpip.h>
#	if !defined(ECONNRESET) && defined(WSAECONNRESET)
#	    define ECONNRESET WSAECONNRESET
#	endif
#	ifndef _SSIZE_T
		typedef signed int ssize_t;
#	endif
#endif

#define ALLOC(N, TYPE) (TYPE *) calloc(N, sizeof(TYPE))
/* #define ALLOC(N, TYPE) (TYPE *) malloc((N) * sizeof(TYPE)) */

SOCKETHANDLE const SOCKET_NULL_HANDLE = -1;

static bool to_sockaddr(char const *const ip, int const port,
        struct sockaddr *addr, int *addrLen) {
	struct addrinfo hint = {0};
	struct addrinfo *pAddrInfo = 0;
	char service_name[0xf];
	struct addrinfo *pAddr = NULL;
    int rc;

	sprintf(&service_name[0], "%d", port & 0xffff);
	service_name[sizeof(service_name) / sizeof(service_name[0]) - 1] = 0;
	hint.ai_family = PF_UNSPEC;
	hint.ai_flags = AI_PASSIVE;
	if((rc = getaddrinfo(ip, &service_name[0], &hint, &pAddrInfo)) != 0) {
        TRACE_ERROR(gai_strerror(rc));
		return false;
    }
	if (!pAddrInfo)
		return false;
	for(pAddr = pAddrInfo; pAddr; pAddr = pAddr->ai_next) {
		memcpy(addr, pAddr->ai_addr, pAddr->ai_addrlen);
		*addrLen = pAddr->ai_addrlen;
		break;
	}
	freeaddrinfo(pAddrInfo);
	return true;
}

#if defined(HAVE_WINDOWS_H) || !defined(HAVE_CONFIG_H)
char const* win32_error_msg(DWORD code) {
    char *msg = NULL;
    /* "Unknown Error"; */
    LPSTR text = 0;
    DWORD ret = FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            0,
            code,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR) &text,
            0,
            0);
    if(ret < 0 || !text) {
        return msg;
    }
    msg = (char*) malloc(ret * sizeof(TCHAR) + 1);
    memset(msg, 0, ret * sizeof(TCHAR) + 1);
    memcpy(msg, text, ret * sizeof(TCHAR));
    LocalFree(text);
    return msg;
}

static bool socket_environment_initialize() {
	WSADATA data;
	char const *err_msg = NULL;
    memset(&data, 0, sizeof(data));
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        TRACE_DEBUG(err_msg = win32_error_msg(WSAGetLastError()));
        free((void*) err_msg);
        return false;
    }
    TRACE_DEBUG("Socket Version = %d.%d", HIBYTE(data.wVersion), LOBYTE(data.wVersion));
    TRACE_DEBUG("Socket HighVersion = %d.%d", HIBYTE(data.wHighVersion), LOBYTE(data.wHighVersion));
    TRACE_DEBUG("Socket VendorInfo = %s", data.lpVendorInfo);
    TRACE_DEBUG("Socket Description = %s", data.szDescription);
    TRACE_DEBUG("Socket SystemStatus = %s", data.szSystemStatus);
    TRACE_DEBUG("Socket MaxSockets = %d", data.iMaxSockets);
    TRACE_DEBUG("Socket MaxUDPSize = %d", data.iMaxUdpDg);
    return true;
}

static bool socket_environment_release() {
	char const *err_msg = NULL;
    if (WSACleanup() != 0) {
        TRACE_DEBUG(err_msg = win32_error_msg(WSAGetLastError()));
        free((void*) err_msg);
        return false;
    }
    return true;
}
#endif

NetworkStream* NetworkStreamCreate(char const *host, int const port) {
    NetworkStream *stream = NULL;
    struct sockaddr addr;
    int addrLen;
#if defined(HAVE_WINDOWS_H) || !defined(HAVE_CONFIG_H)
    char const* err_msg;

    /*
     * As the MSDN said, WSAStartup can be called multiple times.
     * We didn't intend to invoke this method, but without choice.
     * Code must stay clean!!!
     */
    socket_environment_initialize();
#endif

    TRACE_DEBUG("NetworkStreamCreate(char const * = %s, int const = %d)", host, port);
    stream = ALLOC(1, NetworkStream);
    if (stream == NULL) {
#if defined(HAVE_WINDOWS_H) || !defined(HAVE_CONFIG_H)
		socket_environment_release();
#endif
        TRACE_ERROR("Out of memory!!!");
        return stream;
    }
    TRACE_DEBUG("socket(int = PF_INET(%d), int = SOCK_STREAM(%d), int = IPPROTO_TCP(%d))", PF_INET, SOCK_STREAM, IPPROTO_TCP);
    stream->_M_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    TRACE_DEBUG("socket(int = PF_INET(%d), int = SOCK_STREAM(%d), int = IPPROTO_TCP(%d)) = %d", PF_INET, SOCK_STREAM, IPPROTO_TCP, stream->_M_socket);
    if (stream->_M_socket == SOCKET_NULL_HANDLE) {
#if defined(HAVE_WINDOWS_H) || !defined(HAVE_CONFIG_H)
        TRACE_ERROR(err_msg = win32_error_msg(WSAGetLastError()));
        free((void*) err_msg);
        err_msg = NULL;
#else
        TRACE_ERROR(gai_strerror(errno));
#endif
        NetworkStreamDestroy(stream);
        stream = NULL;
        TRACE_DEBUG("NetworkStreamCreate(char const * = %s, int const = %d) = %p", host, port, stream);
        return stream;
    }
    if (!to_sockaddr(host, port, &addr, &addrLen)) {
        TRACE_DEBUG("Failed to translate %s:%d into struct sockaddr", host, port);
        NetworkStreamDestroy(stream);
        stream = NULL;
        TRACE_DEBUG("NetworkStreamCreate(char const * = %s, int const = %d) = %p", host, port, stream);
        return stream;
    }
    if (connect(stream->_M_socket, &addr, addrLen) == -1) {
        TRACE_DEBUG("Failed to conect to %s:%d", host, port);
        NetworkStreamDestroy(stream);
        stream = NULL;
        TRACE_DEBUG("NetworkStreamCreate(char const * = %s, int const = %d) = %p", host, port, stream);
        return stream;
    }
    TRACE_DEBUG("Connected to %s:%d", host, port);
    TRACE_DEBUG("NetworkStreamCreate(char const * = %s, int const = %d) = %p", host, port, stream);
    return stream;
}

void NetworkStreamDestroy(NetworkStream *stream) {
    if (stream != NULL) {
        TRACE_DEBUG("NetworkStreamDestroy(NetworkStream* = %p)", stream);
        if (stream->_M_socket != SOCKET_NULL_HANDLE) {
#if !defined(HAVE_WINDOWS_H) && defined(HAVE_UNISTD_H) && defined(HAVE_CLOSE)
			shutdown(stream->_M_socket, SHUT_RDWR);
            close(stream->_M_socket);
#elif (defined(HAVE_WINDOWS_H) && defined(HAVE_LIBWS2_32)) || !defined(HAVE_CONFIG_H)
			shutdown(stream->_M_socket, SD_BOTH);
            closesocket(stream->_M_socket);
#else
            /* TODO What shoud be used here to close connection. */
#endif
            stream->_M_socket = SOCKET_NULL_HANDLE;
        }
        free(stream);
#if defined(HAVE_WINDOWS_H) || !defined(HAVE_CONFIG_H)
    socket_environment_release();
#endif
    }
}

int NetworkStreamRead(NetworkStream *stream, void *buf, size_t n) {
    ssize_t numOfReadBytes;
    if (NetworkStreamIsClosed(stream))
        return -1;
    TRACE_DEBUG("NetworkStreamRead(NetworkStream* = %p, void* = %p, size_t = %u)", stream, buf, n);
    numOfReadBytes = recv(stream->_M_socket, buf, n, 0);
    TRACE_DEBUG("NetworkStreamRead(NetworkStream* = %p, void* = %p, size_t = %u) = %d", stream, buf, n, numOfReadBytes);
    if (numOfReadBytes == 0)
        stream->_M_isClosed = 1;
    if (numOfReadBytes == -1) {
        TRACE_ERROR("NetworkStream Failed: %s", gai_strerror(errno));
    }
    return numOfReadBytes;
}

int NetworkStreamWrite(NetworkStream *stream, void const *buf, size_t n) {
    ssize_t numOfWrittenBytes;
    if (NetworkStreamIsClosed(stream))
        return -1;
    TRACE_DEBUG("NetworkStreamWrite(NetworkStream* = %p, void* = %p, size_t = %u)", stream, buf, n);
    numOfWrittenBytes = send(stream->_M_socket, buf, n, 0);
    TRACE_DEBUG("NetworkStreamWrite(NetworkStream* = %p, void* = %p, size_t = %u) = %d", stream, buf, n, numOfWrittenBytes);
    if (numOfWrittenBytes == -1) {
        TRACE_ERROR("NetworkStream Failed: %s", gai_strerror(errno));
        if (errno == ECONNRESET) {
            stream->_M_isClosed = 1;
        }
    }
    return numOfWrittenBytes;
}

int NetworkStreamIsClosed(NetworkStream const *stream) {
    return stream->_M_socket == SOCKET_NULL_HANDLE || stream->_M_isClosed;
}
