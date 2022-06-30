#include <errno.h>
#include <stddef.h>
#include "Python.h"
#include "uv.h"

#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif


#ifdef _WIN32
#define PLATFORM_IS_WINDOWS 1
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <stdio.h>
int socketpair(int domain, int type, int protocol, int socket_vector[2])
{
    PyErr_SetString(PyExc_RuntimeError, "Windows does not support socketpair");
    return 1;
}
struct sockaddr_un {
    unsigned short sun_family;
    char*          sun_path;
};
#else
#define PLATFORM_IS_WINDOWS 0
#endif


#ifdef __APPLE__
#define PLATFORM_IS_APPLE 1
#else
#define PLATFORM_IS_APPLE 0
#endif


#if defined(__APPLE__) || defined(__linux__)
#  include <sys/socket.h>
#  include <sys/un.h>
#endif


#ifdef __linux__
#  define PLATFORM_IS_LINUX 1
#  include <sys/epoll.h>
#else
#  define PLATFORM_IS_LINUX 0
#endif

#if defined(__APPLE__) || defined(_WIN32)
#  define EPOLL_CTL_DEL 2
struct epoll_event {
    uint32_t events; // MSVC requires one member; C2016
};
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
    return 0;
};
#endif


PyObject *
MakeUnixSockPyAddr(struct sockaddr_un *addr)
{
#ifdef _WIN32
    PyErr_SetString(PyExc_RuntimeError, "Windows does not support UNIX sockets");
    return NULL;
#endif

    if (addr->sun_family != AF_UNIX) {
        PyErr_SetString(
            PyExc_ValueError, "a UNIX socket addr was expected");
        return NULL;
    }

#ifdef __linux__
    int addrlen = sizeof (struct sockaddr_un);
    size_t linuxaddrlen = addrlen - offsetof(struct sockaddr_un, sun_path);
    if (linuxaddrlen > 0 && addr->sun_path[0] == 0) {
        return PyBytes_FromStringAndSize(addr->sun_path, linuxaddrlen);
    }
    else
#endif /* linux */
    {
        /* regular NULL-terminated string */
        return PyUnicode_DecodeFSDefault(addr->sun_path);
    }
}


#if PY_VERSION_HEX < 0x03070100

PyObject * Context_CopyCurrent(void) {
    return (PyObject *)PyContext_CopyCurrent();
};

int Context_Enter(PyObject *ctx) {
    return PyContext_Enter((PyContext *)ctx);
}

int Context_Exit(PyObject *ctx) {
    return PyContext_Exit((PyContext *)ctx);
}

#else

PyObject * Context_CopyCurrent(void) {
    return PyContext_CopyCurrent();
};

int Context_Enter(PyObject *ctx) {
    return PyContext_Enter(ctx);
}

int Context_Exit(PyObject *ctx) {
    return PyContext_Exit(ctx);
}

#endif
