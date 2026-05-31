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

#ifndef ASKL_SOCKET_COMPAT_H

#define ASKL_SOCKET_COMPAT_H

/* -------------------------------------------------------------------------- */
#ifdef WIN32 /* WIN32 Winsock2 compatibility module */
/* -------------------------------------------------------------------------- */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <errno.h>
#include <winsock2.h>
#undef socklen_t
#include <ws2tcpip.h>
#include <mswsock.h>
#include <io.h>
#if defined(_MSC_VER) && (_MSC_VER < 1300)
#include <process.h>
#endif

#ifndef off_t
    #ifdef _off_t
        #define off_t _off_t
    #else
        #define off_t long
    #endif
#endif

#ifndef socklen_t
    #define socklen_t size_t
#endif

#ifndef INVALID_FILE_HANDLE
    #define INVALID_FILE_HANDLE ((HANDLE)INVALID_HANDLE_VALUE)
#endif

#ifndef ERR
    #ifndef STRINGIFY
        #define STRINGIFY(x)  #x
    #endif
    #ifndef STR
        #define STR(x)  STRINGIFY(x)
    #endif
    #define ERR(c, f) #c "()::" #f  "() @ " __FILE__ ":" STR(__LINE__)
#endif

/* map the WIN32 API functions to their BSD counterparts */
#define ioctl(s, i, l) ioctlsocket((s), (i), (l))

/* map useful Winsock2 error codes to the standard BSD constants */
#undef EINTR
#define EINTR WSAEINTR
#undef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#undef EAGAIN
#define EAGAIN EWOULDBLOCK
#undef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#undef EALREADY
#define EALREADY WSAEALREADY
#undef ESPIPE
#define ESPIPE EWOULDBLOCK
#undef EISCONN
#define EISCONN WSAEISCONN

/* Winsock2 does not use errno - work around with some macros */
#define ERRNO ( (errno = WSAGetLastError()) )
#define _socket_perror(s) \
(fprintf(stderr, "%s: %s\n", (s), _socket_win32_strerror()))

#if defined(_MSC_VER)
    /* include the needed libs */
    #pragma comment      ( lib, "ws2_32.lib" )
    #pragma comment      ( lib, "pthreadVC2.lib" )
    #if (_MSC_VER < 1300)
        /* Microsoft Visual C++ 6 does not support POSIX networking functions */
        #define _POSIX_EMULATION
    #endif
#elif defined(__GNUC__)
    /* for some reasons, gai_strerror() does not link in Dev-C++ */
    #undef gai_strerror
    #define gai_strerror(i) ("unknown error")
#endif

#if defined(_WIN32_WINNT) && _WIN32_WINNT < 0x0501
    /* Windows 2k ws2_32.dll does not provide the POSIX networking functions */
    #ifndef _POSIX_EMULATION
        #define _POSIX_EMULATION
    #endif
#endif

/* -------------------------------------------------------------------------- */

ASKL_API int socketpair(
    UNUSED int d,
    UNUSED int t,
    UNUSED int p,
    SOCKET sv[2]
);

/**
 * @ingroup socket
 * @fn int socketpair(int d, int t, int p, int sv[2])
 * 
 * @param d  the communication domain (ignored on Windows)
 * @param t  the socket type (ignored on Windows)
 * @param p  the protocol (ignored on Windows)
 * @param sv an array of two integers that will receive the created sockets
 * 
 * @return 0 on success, -1 if an error occurs
 *
 * This function creates a pair of connected sockets, storing them in
 * @p sv[0] and @p sv[1], emulating @c socketpair(AF_UNIX, SOCK_STREAM, 0, sv)
 * using a loopback TCP connection.
 *
 * The caller is responsible for closing both sockets when they are no
 * longer needed.
 */

/* -------------------------------------------------------------------------- */

ASKL_API SOCKET dupsocket(SOCKET s);

/**
 * @ingroup socket
 * @fn SOCKET dupsocket(SOCKET s)
 * @param s an existing socket descriptor or handle
 * @return on success, a new socket descriptor/handle referring to the
 *         same underlying endpoint; on error, INVALID_SOCKET (Windows)
 *         or -1 (POSIX) is returned
 *
 * This function duplicates a socket descriptor.
 *
 * On POSIX systems, it is an alias for @c dup().
 * On Windows, it uses DuplicateHandle() to create a new Winsock-compatible
 * handle referring to the same underlying socket.
 *
 * The returned socket must eventually be closed independently with
 * @c closesocket() (Windows) or @c close() (POSIX).
 */

/* -------------------------------------------------------------------------- */

INTERNAL const char *_socket_win32_strerror(void);

/**
 * @ingroup socket
 * @fn const char *_socket_win32_strerror(void)
 * 
 * @param void
 * 
 * @return a pointer to a static, human-readable error message string,
 *         or NULL if the current error code is unknown
 *
 * This private helper translates the current Winsock error code stored
 * in @c errno into a human-readable error string.
 *
 * It is only available on Windows builds and is used internally by
 * the @_socket_perror macro to report socket-related errors.
 *
 * The returned pointer refers to static storage and must not be freed
 * or modified by the caller.
 */

/* -------------------------------------------------------------------------- */
#else /* POSIX compatibility */
/* -------------------------------------------------------------------------- */

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#if defined(_USE_BIG_FDS) && defined(HAS_POLL)
#include <poll.h>
#endif

#define _socket_perror perror
#define ERRNO errno
#define SOCKET int
#define closesocket(s) close((s))
#define dupsocket(s) dup((s))
#define INVALID_SOCKET -1

#if ! defined(TCP_CORK) && defined(TCP_NOPUSH)
    #define TCP_CORK TCP_NOPUSH
#endif

/* sendfile */
#if defined(__linux__)
    #include <sys/sendfile.h>
#else
    #if defined(__FreeBSD__)
        #include <sys/types.h>
        #include <sys/socket.h>
    #elif defined(__sun)
        #include <sys/sendfile.h>
    #endif
#endif

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

INTERNAL ssize_t _socket_sendfile(SOCKET out, int in, off_t *off, size_t len);

/**
 * @ingroup socket
 * @fn ssize_t _socket_sendfile(int out, int in, off_t *off, size_t len)
 * 
 * @param out destination socket file descriptor
 * @param in  source file descriptor (regular file)
 * @param off optional pointer to a file offset, updated on success
 * @param len maximum number of bytes to transfer
 * 
 * @return the number of bytes sent, or -1 if an error occurs
 *
 * This is a low-level, platform-specific helper used to implement sendfile().
 * On platforms without a native sendfile-like system call, it returns -1 and
 * sets errno to ENOSYS.
 */

/* -------------------------------------------------------------------------- */
#ifdef _POSIX_EMULATION
/* -------------------------------------------------------------------------- */

/*
  This is a small compatibility layer provided for OS or libc which do not
  implement the POSIX protocol independant network functions. The assumption is
  made that such an OS/libc probably does not either properly implement
  IPv6, so only the classic BSD IPv4 API is used for a better portability.
*/

#if (defined(_MSC_VER) && (_MSC_VER < 1300))
/* define the addrinfo structure */
struct addrinfo {
    int     ai_flags;
    int     ai_family;
    int     ai_socktype;
    int     ai_protocol;
    socklen_t  ai_addrlen;
    struct sockaddr *ai_addr;
    char   *ai_canonname;
    struct addrinfo *ai_next;
};

/* use the classic sockaddr_in */
#define sockaddr_storage sockaddr_in

/* netdb.h definitions */
#define AI_PASSIVE     0x0001  /* Socket address is intended for `bind'.  */
#define AI_NUMERICHOST 0x0004  /* Don't use name resolution.  */
#define EAI_NONAME       -2    /* NAME or SERVICE is unknown.  */
#define EAI_FAIL         -4    /* Non-recoverable failure in name res.  */
#define EAI_FAMILY       -6    /* `ai_family' not supported.  */
#define EAI_MEMORY       -10   /* Memory allocation failure.  */
#define NI_NUMERICHOST 1       /* Don't try to look up hostname.  */
#define NI_NUMERICSERV 2       /* Don't convert port number to name.  */

/* default size for the host and service buffers */
#define NI_MAXHOST      1025
#define NI_MAXSERV      32
#endif

/* use the minimalist implementation of the POSIX functions */
#undef getaddrinfo
#undef getnameinfo
#undef freeaddrinfo
#define getaddrinfo  _getaddrinfo
#define getnameinfo  _getnameinfo
#define freeaddrinfo _freeaddrinfo

/* remove gai_strerror */
#undef gai_strerror
#define gai_strerror(i) "unknown error."

/* -------------------------------------------------------------------------- */

ASKL_API int getaddrinfo(
    const char *node,
    const char *service,
    const struct addrinfo *hints,
    struct addrinfo **res
);

ASKL_API int getnameinfo(
    const struct sockaddr *s,
    int salen,
    char *host,
    size_t hostlen,
    char *serv,
    size_t servlen,
    int flags
);

ASKL_API void freeaddrinfo(struct addrinfo *res);

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

/* a small private macro for easily printing errors */
#define _gai_perror(s, i) (fprintf(stderr, "%s: %s\n", (s), gai_strerror((i))))

ASKL_API int socket_sendfd(SOCKET sock, SOCKET fd);

/**
 * @ingroup socket
 * @fn int socket_sendfd(int sock, int fd)
 * 
 * @param sock a connected Unix-domain (or emulated) socket
 * @param fd   an open file descriptor to send
 * 
 * @return 1 on success, 0 or -1 on error depending on platform
 *
 * This function sends a file descriptor over a Unix-domain socket using
 * SCM_RIGHTS on POSIX systems or WSADuplicateSocket() on Windows.
 */

ASKL_API SOCKET socket_recvfd(SOCKET sock);

/**
 * @ingroup socket
 * @fn int socket_recvfd(int sock)
 * 
 * @param sock a connected Unix-domain (or emulated) socket
 * 
 * @return on success, the received file descriptor (or socket handle);
 *         on error, a non-positive value is returned
 *
 * This function receives a file descriptor sent over a Unix-domain socket
 * using @ref socket_sendfd().
 *
 * On POSIX systems, the descriptor is received via SCM_RIGHTS ancillary
 * data. On Windows, the function uses the WSADuplicateSocket() mechanism
 * internally to reconstruct a duplicate socket handle in the current
 * process.
 *
 * A strictly positive return value is the received descriptor. Any
 * non-positive value indicates a failure; the exact error reporting
 * conventions are platform-dependent (0 or -1).
 *
 * @see socket_sendfd()
 */

#endif
