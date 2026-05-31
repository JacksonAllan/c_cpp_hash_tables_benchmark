/*******************************************************************************
 *  ASKL.                                                                      *
 *  Copyright (c) 2026 Raphael Prevost <raph@el.bzh>                           *
 *                                                                             *
 *  This software is a computer program whose purpose is to provide a          *
 *  framework for developing and prototyping network services.                 *
 *                                                                             *
 *  This software is governed by the CeCILL  license under French law and      *
 *  abiding by the rules of distribution of free software.  You can  use,      *
 *  modify and/ or redistribute the software under the terms of the CeCILL     *
 *  license as circulated by CEA, CNRS and INRIA at the following URL          *
 *  "http://www.cecill.info".                                                  *
 *                                                                             *
 *  As a counterpart to the access to the source code and  rights to copy,     *
 *  modify and redistribute granted by the license, users are provided only    *
 *  with a limited warranty  and the software's author,  the holder of the     *
 *  economic rights,  and the successive licensors  have only  limited         *
 *  liability.                                                                 *
 *                                                                             *
 *  In this respect, the user's attention is drawn to the risks associated     *
 *  with loading,  using,  modifying and/or developing or reproducing the      *
 *  software by the user in light of its specific status of free software,     *
 *  that may mean  that it is complicated to manipulate,  and  that  also      *
 *  therefore means  that it is reserved for developers  and  experienced      *
 *  professionals having in-depth computer knowledge. Users are therefore      *
 *  encouraged to load and test the software's suitability as regards their    *
 *  requirements in conditions enabling the security of their systems and/or   *
 *  data to be ensured and,  more generally, to use and operate it in the      *
 *  same conditions as regards security.                                       *
 *                                                                             *
 *  The fact that you are presently reading this means that you have had       *
 *  knowledge of the CeCILL license and that you accept its terms.             *
 *                                                                             *
 ******************************************************************************/

#include "askl_socket_compat.h"

#ifdef _POSIX_EMULATION
/* this mutex is used to avoid corruption when using inet_ntoa */
static pthread_mutex_t _not_reentrant = PTHREAD_MUTEX_INITIALIZER;
#endif

/* -------------------------------------------------------------------------- */
#ifdef WIN32 /* WIN32 Winsock2 */
/* -------------------------------------------------------------------------- */

/* winsock2 error codes */
static int err[] = {
    0, WSAEINTR, WSAEBADF, WSAEACCES, WSAEFAULT, WSAEINVAL,
    WSAEMFILE, WSAEWOULDBLOCK, WSAEINPROGRESS, WSAEALREADY,
    WSAENOTSOCK, WSAEDESTADDRREQ, WSAEMSGSIZE, WSAEPROTOTYPE,
    WSAENOPROTOOPT, WSAEPROTONOSUPPORT, WSAESOCKTNOSUPPORT,
    WSAEOPNOTSUPP, WSAEPFNOSUPPORT, WSAEAFNOSUPPORT,
    WSAEADDRINUSE, WSAEADDRNOTAVAIL, WSAENETDOWN,
    WSAENETUNREACH, WSAENETRESET, WSAECONNABORTED, 
    WSAECONNRESET, WSAENOBUFS, WSAEISCONN, WSAENOTCONN,
    WSAESHUTDOWN, WSAETOOMANYREFS, WSAETIMEDOUT,
    WSAECONNREFUSED, WSAELOOP, WSAENAMETOOLONG, WSAEHOSTDOWN,
    WSAEHOSTUNREACH, WSAENOTEMPTY, WSAEPROCLIM, WSAEUSERS,
    WSAEDQUOT, WSAESTALE, WSAEREMOTE, WSASYSNOTREADY,
    WSAVERNOTSUPPORTED, WSANOTINITIALISED, WSAEDISCON,
    WSAHOST_NOT_FOUND, WSANO_DATA
};

/* winsock2 error strings */
static const char *str[] = {
    "No error", "Interrupted system call",
    "Bad file number", "Permission denied",
    "Bad address", "Invalid argument",
    "Too many open sockets", "Operation would block",
    "Operation now in progress",
    "Operation already in progress",
    "Socket operation on non-socket",
    "Destination address required",
    "Message too long",
    "Protocol wrong type for socket",
    "Bad protocol option", "Protocol not supported",
    "Socket type not supported",
    "Operation not supported on socket",
    "Protocol family not supported",
    "Address family not supported",
    "Address already in use",
    "Can't assign requested address",
    "Network is down", "Network is unreachable",
    "Net connection reset",
    "Software caused connection abort",
    "Connection reset by peer",
    "No buffer space available",
    "Socket is already connected",
    "Socket is not connected",
    "Can't send after socket shutdown",
    "Too many references, can't splice",
    "Connection timed out",
    "Connection refused",
    "Too many levels of symbolic links",
    "File name too long", "Host is down",
    "No route to host", "Directory not empty",
    "Too many users", "Disc quota exceeded",
    "Stale NFS file handle",
    "Too many level of remote in path",
    "Network system is unavailable",
    "Winsock version out of range",
    "WSAStartup not yet called",
    "Graceful shutdown in progress",
    "Host not found",
    "No host data of that type was found"
};

/* -------------------------------------------------------------------------- */

INTERNAL const char *_socket_win32_strerror(void)
{
    unsigned int i = 0, err_count = sizeof(err) / sizeof(err[0]);

    for (i = 0; i < err_count; i ++)
        if (errno == err[i]) return str[i];

    return NULL;
}

/* -------------------------------------------------------------------------- */

INTERNAL ssize_t _socket_sendfile(SOCKET out, int in, off_t *off, size_t len)
{
    HANDLE handle = INVALID_FILE_HANDLE;
    off_t current = 0;

    if (off) {
        current = _lseek(in, 0, SEEK_CUR);
        /* try seeking to the given offset */
        if (_lseek(in, *off, SEEK_SET) == -1) { errno = EINVAL; return -1; }
    }

    /* get the windows API file handle */
    if ( (handle = (HANDLE) _get_osfhandle(in)) == INVALID_FILE_HANDLE) {
        errno = EBADF; return -1;
    }

    /* don't use the TCP_CORK-like feature, since TransmitFile() is blocking;
       that way it's still possible to stop writing if the header blocks */
    if (! TransmitFile(out, handle, len, 0, NULL, NULL, 0)) {
        errno = EIO; return -1;
    }

    if (off) {
        /* restore the file offset and increment the given one */
        _lseek(in, current, SEEK_SET);
        *off += len;
    }

    return len;
}

/* -------------------------------------------------------------------------- */
#elif (defined (__linux__))
/* -------------------------------------------------------------------------- */

INTERNAL ssize_t _socket_sendfile(SOCKET out, int in, off_t *off, size_t len)
{
    return sendfile(out, in, off, len);
}

/* -------------------------------------------------------------------------- */
#elif (defined (__FreeBSD__))
/* -------------------------------------------------------------------------- */

INTERNAL ssize_t _socket_sendfile(SOCKET out, int in, off_t *off, size_t len)
{
    off_t written = 0, current = 0, p = 0;
    int ret = -1;

    if (off) { current = lseek(in, 0, SEEK_CUR); p = *off; }

    if ( (ret = sendfile(in, out, p, len, NULL, & written, 0)) == 0) {
        ret = written;
        if (off) {
            /* restore the file offset and increment the given one */
            lseek(in, current, SEEK_SET);
            *off += written;
        }
    }

    return ret;
}

/* -------------------------------------------------------------------------- */
#elif (defined(MAC_OS_X_VERSION_10_5) || defined(__MAC_10_5))
/* -------------------------------------------------------------------------- */

INTERNAL ssize_t _socket_sendfile(SOCKET out, int in, off_t *off, size_t len)
{
    off_t l = len;
    int ret = 0;

    if ( (ret = sendfile(out, in, ((off) ? *off : 0), & l, NULL, 0)) == 0) {
        if (off) *off += l;
        ret = l;
    }

    return ret;
}

/* -------------------------------------------------------------------------- */
#elif (defined (__sun))
/* -------------------------------------------------------------------------- */

INTERNAL ssize_t _socket_sendfile(SOCKET out, int in, off_t *off, size_t len)
{
    size_t written = 0, current = 0;
    int ret = 0;
    struct sendfilevec vector;

    if (off) current = lseek(in, 0, SEEK_CUR);

    memset(& vector, 0, sizeof(vector));
    vector.sfv_fd = in;
    vector.sfv_off = (off) ? *off : 0;
    vector.sfv_len = len;

    if ( (ret = sendfilev(out, & vector, 1, & written)) == 0) {
        ret = written;
        if (off) {
            /* restore the file offset and increment the given one */
            lseek(in, current, SEEK_SET);
            *off += written;
        }
    }

    return ret;
}

/* -------------------------------------------------------------------------- */
#else
/* -------------------------------------------------------------------------- */

/* use the generic implementation */
INTERNAL ssize_t _socket_sendfile(
    UNUSED SOCKET out,
    UNUSED int in,
    UNUSED off_t *off,
    UNUSED size_t len
)
{
    errno = ENOSYS;
    return -1;
}

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

/*
  This is a small compatibility layer provided for OS or libc which do not
  provide the POSIX protocol independant network functions. The assumption is
  made that such an OS/libc probably does not either properly implement
  IPv6, so only the classic BSD IPv4 API is used for better portability.
*/

/* -------------------------------------------------------------------------- */
#ifdef _POSIX_EMULATION
/* -------------------------------------------------------------------------- */

ASKL_API int getaddrinfo(const char *node, const char *service,
                         const struct addrinfo *hints, struct addrinfo **res)
{
    /** @brief *Very* minimal getaddrinfo() implementation */
    /*
      This is a *very* minimal implementation of getaddrinfo(), to be able
      to compile and run with an OS/libc which is not POSIX compliant.
      This small implementation only meets the needs of ASKL and use the
      most common BSD functions. It only supports IPv4.
    */

    struct addrinfo *ai = NULL;
    int len = sizeof(struct sockaddr_in);
    struct sockaddr_in *sa = NULL;

    /* not a full implementation, bail out if no hint was given */
    if (! res || ! hints) return EAI_FAIL;

    /* only support AI_NUMERICHOST */
    if (! (hints->ai_flags & AI_NUMERICHOST)) return EAI_FAIL;

    /* only support IPv4 */
    if (hints->ai_family != AF_INET) return EAI_FAMILY;

    /* exit conforming to the specs if no node nor service was given */
    if (! node && ! service) return EAI_NONAME;

    if ( ! (ai = malloc(sizeof(*ai))) ) return EAI_MEMORY;
    ai->ai_family   = hints->ai_family; ai->ai_socktype = hints->ai_socktype;
    ai->ai_protocol = hints->ai_protocol; ai->ai_addrlen = len;

    if (! (ai->ai_addr = malloc(len)) ) { free(ai); return EAI_MEMORY; }
    sa = (void *) ai->ai_addr; memset(sa, 0, len);

    sa->sin_family = AF_INET;
    sa->sin_addr.s_addr = (hints->ai_flags & AI_PASSIVE) ?
                          (0x0) : (node ? inet_addr(node) : -1);
    if (service) sa->sin_port = htons((short) atoi(service));

    ai->ai_next = NULL; *res = ai;

    return 0;
}

/* -------------------------------------------------------------------------- */

ASKL_API int getnameinfo(const struct sockaddr *s, int salen, char *host,
                         size_t hostlen, char *serv, size_t servlen, int flags)
{
    /** @brief *Very* minimal getnameinfo() implementation */
    /*
      This is a *very* minimal implementation of getnameinfo(), to be able
      to compile and run with an OS/libc which is not POSIX compliant.
      This small implementation only meets the needs of naer and use the
      most common BSD functions. It only supports IPv4.
    */

    const char *ip = NULL;
    struct sockaddr_in *sa = (void *) s;

    /* neither hostname nor service name were requested */
    if (! host && ! serv) return EAI_NONAME;

    /* wrong address */
    if (! sa || salen != sizeof(*sa)) return EAI_FAMILY;

    /* minimal implementation, only support NI_NUMERICHOST & NI_NUMERICSERV */
    if (host && (flags & NI_NUMERICHOST) ) {
        pthread_mutex_lock(& _not_reentrant);
        if ((ip = inet_ntoa(sa->sin_addr))) strncpy(host, ip, hostlen - 1);
        pthread_mutex_unlock(& _not_reentrant);
    } else return EAI_FAIL;

    if (serv && (flags & NI_NUMERICSERV) )
        snprintf(serv, servlen - 1, "%i", ntohs(sa->sin_port));
    else return EAI_FAIL;

    return 0;
}

/* -------------------------------------------------------------------------- */

ASKL_API void freeaddrinfo(struct addrinfo *res)
{
    /** @brief *Very* minimal freeaddrinfo() implementation */
    /*
      This is a *very* minimal implementation of freeaddrinfo(), to be able
      to compile and run with an OS/libc which is not POSIX compliant.
      This small implementation only meets the needs of ASKL and use the
      most common BSD functions. It only supports IPv4.
    */

    if (res && res->ai_addr) free(res->ai_addr); free(res);
}

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

/*
 * The following implements sendfd() and recvfd() to send or receive sockets
 * over Unix domain sockets.
 *
 */

/* -------------------------------------------------------------------------- */
#ifdef WIN32
/* -------------------------------------------------------------------------- */

#ifndef AF_UNIX
#define AF_UNIX 0x0
#endif

ASKL_API int socketpair(UNUSED int d, UNUSED int t, UNUSED int p, SOCKET sv[2])
{
    SOCKET s = INVALID_SOCKET;
    struct sockaddr_in addr;
    int len = sizeof(addr);

    sv[0] = sv[1] = INVALID_SOCKET;

    if ( (s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        _socket_perror(ERR(socketpair, socket));
        goto _err_sock;
    }

    memset(& addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(s, (SOCKADDR *) & addr, len) == SOCKET_ERROR) {
        _socket_perror(ERR(socketpair, bind));
        goto _err_pair;
    }

    if (listen(s, 1) == SOCKET_ERROR) {
        _socket_perror(ERR(socketpair, listen));
        goto _err_pair;
    }

    if (getsockname(s, (struct sockaddr *) & addr, & len) == SOCKET_ERROR) {
        _socket_perror(ERR(socketpair, getsockname));
        goto _err_pair;
    }

    if ( (sv[1] = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        _socket_perror(ERR(socketpair, socket));
        goto _err_pair;
    }

    if (connect(sv[1], (struct sockaddr *) & addr, len) == SOCKET_ERROR) {
        _socket_perror(ERR(socketpair, connect));
        goto _err_conn;
    }

    sv[0] = accept(s, (struct sockaddr *) & addr, & len);
    if (sv[0] == INVALID_SOCKET) {
        _socket_perror(ERR(socketpair, accept));
        goto _err_conn;
    }

    closesocket(s);

    return 0;

_err_conn:
    closesocket(sv[1]); sv[1] = INVALID_SOCKET;
_err_pair:
    closesocket(s);
_err_sock:
    return -1;
}

/* -------------------------------------------------------------------------- */

ASKL_API int socket_sendfd(SOCKET socket, SOCKET fd)
{
    WSAPROTOCOL_INFO info;
    DWORD pid = 0;

    /* the receiver must give us a pid */
    if (recv(socket, (char *) & pid, sizeof(pid), 0x0) == -1) {
        _socket_perror(ERR(socket_sendfd, recv));
        return -1;
    }

    if (WSADuplicateSocket(fd, pid, & info) == -1) {
        _socket_perror(ERR(socket_sendfd, WSADuplicateSocket));
        return -1;
    }

    /* send the raw struct over the wire */
    if (send(socket, (char *) & info, sizeof(info), 0x0) == -1) {
        _socket_perror(ERR(socket_sendfd, send));
        return -1;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

ASKL_API SOCKET socket_recvfd(SOCKET socket)
{
    WSAPROTOCOL_INFO info;
    DWORD pid = _getpid();
    SOCKET ret = INVALID_SOCKET;

    if (send(socket, (char *) & pid, sizeof(pid), 0x0) == -1) {
        _socket_perror(ERR(socket_recvfd, send));
        return INVALID_SOCKET;
    }

    if (recv(socket, (char *) & info, sizeof(info), 0x0) == -1) {
        _socket_perror(ERR(socket_recvfd, recv));
        return INVALID_SOCKET;
    }

    if ( (ret = WSASocket(-1, -1, -1, & info, 0, 0x0)) == INVALID_SOCKET) {
        _socket_perror(ERR(socket_recvfd, WSASocket));
        return INVALID_SOCKET;
    }

    return ret;
}

/* -------------------------------------------------------------------------- */

ASKL_API SOCKET dupsocket(SOCKET s)
{
    HANDLE h;
    BOOL ret = DuplicateHandle(
        GetCurrentProcess(),
        (HANDLE) s,
        GetCurrentProcess(),
        & h,
        0,
        FALSE,
        DUPLICATE_SAME_ACCESS
    );

    if (! ret) {
        WSASetLastError(WSAEBADF);
        _socket_perror(ERR(dupsocket, DuplicateHandle));
        return INVALID_SOCKET;
    }

    return (SOCKET) h;
}

/* -------------------------------------------------------------------------- */
#else
/* -------------------------------------------------------------------------- */

/*
 * Copyright (c) 2000 Sampo Kellomaki <sampo@iki.fi>, All Rights Reserved.
 * This module may be copied under the same terms as the perl itself.
 *
 */

/* I test here for __sun for lack of anything better, but I
 * mean Solaris 2.6. The idea of undefining SCM_RIGHTS is
 * to force the headers to behave BSD 4.3 way which I have
 * tested to work.
 *
 * In general, if you have compilation errors, you might consider
 * adding a test for your platform here.
 */
#if defined(__sun)
#undef SCM_RIGHTS
#endif

#ifdef SCM_RIGHTS

/* It seems various versions of glibc headers (i.e.
 * /usr/include/socketbits.h) miss one or more of these */

#ifndef CMSG_DATA
# define CMSG_DATA(cmsg) ((cmsg)->cmsg_data)
#endif

#ifndef CMSG_NXTHDR
# define CMSG_NXTHDR(mhdr, cmsg) __cmsg_nxthdr (mhdr, cmsg)
#endif

#ifndef CMSG_FIRSTHDR
# define CMSG_FIRSTHDR(mhdr) \
  ((size_t) (mhdr)->msg_controllen >= sizeof (struct cmsghdr)          \
   ? (struct cmsghdr *) (mhdr)->msg_control : (struct cmsghdr *) NULL)
#endif

#ifndef CMSG_ALIGN
# define CMSG_ALIGN(len) (((len) + sizeof (size_t) - 1) \
             & ~(sizeof (size_t) - 1))
#endif

#ifndef CMSG_SPACE
# define CMSG_SPACE(len) (CMSG_ALIGN (len) \
             + CMSG_ALIGN (sizeof (struct cmsghdr)))
#endif

#ifndef CMSG_LEN
# define CMSG_LEN(len)   (CMSG_ALIGN (sizeof (struct cmsghdr)) + (len))
#endif

union fdmsg {
    struct cmsghdr h;
    char buf[CMSG_SPACE(sizeof(int))];
};
#endif

ASKL_API int socket_sendfd(int sock, int fd)
{
    int ret = 0;
    struct iovec  iov[1];
    struct msghdr msg;

    iov[0].iov_base = & ret;  /* Don't send any data. Note: der Mouse
                               * <mouse@Rodents.Montreal.QC.CA> says
                               * that might work better if at least one
                               * byte is sent. */
    iov[0].iov_len  = 1;

    msg.msg_iov     = iov;
    msg.msg_iovlen  = 1;
    msg.msg_name    = 0;
    msg.msg_namelen = 0;

    {
        #ifdef SCM_RIGHTS
        /* New BSD 4.4 way (ouch, why does this have to be so convoluted). */
        union  fdmsg  cmsg;
        struct cmsghdr *h;

        msg.msg_control = cmsg.buf;
        msg.msg_controllen = sizeof(union fdmsg);
        msg.msg_flags = 0;

        h = CMSG_FIRSTHDR(&msg);
        h->cmsg_len   = CMSG_LEN(sizeof(int));
        h->cmsg_level = SOL_SOCKET;
        h->cmsg_type  = SCM_RIGHTS;
        *CMSG_DATA(h) = fd;
        #else
        /* Old BSD 4.3 way. Not tested. */
        msg.msg_accrights = & fd;
        msg.msg_accrightslen = sizeof(fd);
        #endif

        ret = (sendmsg(sock, & msg, 0) < 0) ? 0 : 1;
    }

    return ret;
}

/* -------------------------------------------------------------------------- */

ASKL_API int socket_recvfd(int sock)
{
    int count;
    int ret = 0;
    struct iovec  iov[1];
    struct msghdr msg;

    iov[0].iov_base = & ret;  /* don't receive any data */
    iov[0].iov_len  = 1;

    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    {
        #ifdef SCM_RIGHTS
        union fdmsg  cmsg;
        struct cmsghdr *h;

        msg.msg_control = cmsg.buf;
        msg.msg_controllen = sizeof(union fdmsg);
        msg.msg_flags = 0;

        h = CMSG_FIRSTHDR(& msg);
        h->cmsg_len   = CMSG_LEN(sizeof(int));
        h->cmsg_level = SOL_SOCKET;  /* Linux does not set these */
        h->cmsg_type  = SCM_RIGHTS;  /* upon return */
        *CMSG_DATA(h) = -1;

        if ((count = recvmsg(sock, & msg, 0)) < 0) {
            ret = 0;
        } else {
            h = CMSG_FIRSTHDR(& msg);   /* can realloc? */
            if (   h == NULL
                || h->cmsg_len    != CMSG_LEN(sizeof(int))
                || h->cmsg_level  != SOL_SOCKET
                || h->cmsg_type   != SCM_RIGHTS ) {
                /* This should really never happen */
                if (h)
                  fprintf(stderr,
                    "%s:%d: protocol failure: %u %d %d\n",
                    __FILE__, __LINE__,
                    (unsigned int) h->cmsg_len,
                    h->cmsg_level, h->cmsg_type);
                else
                  fprintf(stderr,
                    "%s:%d: protocol failure: NULL cmsghdr*\n",
                    __FILE__, __LINE__);
                ret = 0;
            } else {
                ret = *CMSG_DATA(h);
            }
        }
        #else
        int receive_fd;
        msg.msg_accrights = & receive_fd;
        msg.msg_accrightslen = sizeof(receive_fd);

        ret = (recvmsg(sock, & msg, 0) < 0) ? 0 : receive_fd;
        #endif
    }

    return ret;
}

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */
