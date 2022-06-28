IF UNAME_SYSNAME == "Windows":
    include "system_win32.pxi"
ELSE:
    include "system_posix.pxi"

cdef extern from "includes/compat.h" nogil:

    cdef int EWOULDBLOCK

    cdef int PLATFORM_IS_APPLE
    cdef int PLATFORM_IS_LINUX
    cdef int PLATFORM_IS_WINDOWS

    struct epoll_event:
        # We don't use the fields
        pass

    int EPOLL_CTL_DEL
    int epoll_ctl(int epfd, int op, int fd, epoll_event *event)
