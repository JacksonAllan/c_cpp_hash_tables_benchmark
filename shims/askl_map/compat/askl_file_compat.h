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

#ifndef ASKL_FILE_COMPAT_H

#define ASKL_FILE_COMPAT_H

/* -------------------------------------------------------------------------- */
#ifdef WIN32 /* WIN32 file I/O compatibility */
/* -------------------------------------------------------------------------- */

#ifndef __MINGW32__
    #ifndef stat
        #define stat _stat
    #endif

    #ifndef fstat
        #define fstat _fstat
    #endif
#endif

/* open */
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <direct.h>
#include <sys/stat.h>

#ifndef O_RDONLY
    #define O_RDONLY _O_RDONLY
#endif

#ifndef O_WRONLY
    #define O_WRONLY _O_WRONLY
#endif

#ifndef O_RDWR
    #define O_RDWR _O_RDWR
#endif

#ifndef O_CREAT
    #define O_CREAT _O_CREAT
#endif

#ifndef O_APPEND
    #define O_APPEND _O_APPEND
#endif

#ifndef O_EXCL
    #define O_EXCL _O_EXCL
#endif

#ifndef O_TRUNC
    #define O_TRUNC _O_TRUNC
#endif

/* user permissions */
#ifndef S_IRWXU
    #define S_IRWXU (_S_IREAD | _S_IWRITE)
#endif
#ifndef S_IRUSR
    #define S_IRUSR _S_IREAD
#endif
#ifndef S_IWUSR
    #define S_IWUSR _S_IWRITE
#endif
#ifndef S_IXUSR
    #define S_IXUSR 0
#endif
/* group */
#ifndef S_IRWXG
    #define S_IRWXG (_S_IREAD | _S_IWRITE)
#endif
#ifndef S_IRGRP
    #define S_IRGRP _S_IREAD
#endif
#ifndef S_IWGRP
    #define S_IWGRP _S_IWRITE
#endif
#ifndef S_IXGRP
    #define S_IXGRP 0
#endif
/* others */
#ifndef S_IRWXO
    #define S_IRWXO (_S_IREAD | _S_IWRITE)
#endif
#ifndef S_IROTH
    #define S_IROTH _S_IREAD
#endif
#ifndef S_IWOTH
    #define S_IWOTH _S_IWRITE
#endif
#ifndef S_IXOTH
    #define S_IXOTH 0
#endif

/* access(2) */
#ifndef access
    #define access _access
#endif

#ifndef F_OK
    #define F_OK 0x0
#endif

#ifndef R_OK
    #define R_OK 0x4
#endif

#ifndef W_OK
    #define W_OK 0x2
#endif

/* X_OK is not portable, fallback to existence */
#ifndef X_OK
    #define X_OK F_OK
#endif

/* unlink(2) */
#ifndef unlink
    #define unlink _unlink
#endif

/* utime(2) */
#include <sys/utime.h>

#ifndef utime
    #define utime _utime
    #define utimbuf _utimbuf
#endif

/* mkdir(2) */
#ifndef mkdir
    #define mkdir(p, m) _mkdir(p)
#endif

/* open(2) */
#undef open
#define open posix_open

ASKL_API int posix_open(const char *pathname, int flags, ...);

/**
 * @fn int posix_open(const char *pathname, int flags, ...)
 * @param pathname path to the file to open
 * @param flags    same semantics as POSIX @c open(2)
 * @param ...      optional @c mode_t when @c O_CREAT is present in @p flags
 * @return a file descriptor on success, or @c -1 if an error occurs
 *
 * This function provides a POSIX-like @c open(2) interface on Windows.
 *
 * It calls @_sopen() and:
 * - forces binary mode by OR-ing @c _O_BINARY into @p flags;
 * - uses @c _SH_DENYNO sharing mode so other processes may access the file.
 *
 * When @c O_CREAT is present in @p flags, an additional @c mode argument
 * must be provided, exactly as for POSIX @c open(2).
 */

ASKL_API ssize_t file_getxattr(
    const char *file,
    const char *attrname,
    void *attrbuf,
    size_t len
);

ASKL_API int file_setxattr(
    const char *file,
    const char *attrname,
    const void *buf,
    size_t len,
    int flags
);

/* -------------------------------------------------------------------------- */
#else
/* -------------------------------------------------------------------------- */

#include <fcntl.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/xattr.h>

#ifdef __APPLE__
ASKL_API ssize_t file_getxattr(
    const char *file,
    const char *attrname,
    void *attrbuf,
    size_t len
);

ASKL_API int file_setxattr(
    const char *file,
    const char *attrname,
    const void *buf,
    size_t len,
    int flags
);
#else
#define file_getxattr getxattr
#define file_setxattr setxattr
#endif

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

/**
 * @fn ssize_t file_getxattr(const char *file, const char *attrname,
 *                           void *attrbuf, size_t len)
 * 
 * @param file     path to the file whose extended attribute should be read
 * @param attrname name of the extended attribute
 * @param attrbuf  buffer where the attribute value will be copied
 * @param len      size of @p attrbuf in bytes
 * 
 * @return the number of bytes copied into @p attrbuf on success, or @c -1
 *         if an error occurs
 *
 * This function provides a portable wrapper for retrieving extended file
 * attributes.
 *
 * On Linux and other systems providing @c getxattr(2), it is a direct
 * alias of that system call. On Mac OS X, it wraps @c getxattr(2) with
 * the appropriate arguments for named attributes.
 *
 * On Windows, this function emulates extended attributes using NTFS EA
 * data streams and the BackupRead API. It searches the EA stream for an
 * entry whose name matches @p attrname and, if found, copies its value
 * into @p attrbuf.
 *
 * If the attribute’s stored size exceeds @p len, the function fails with
 * @c errno set to @c ERANGE. Callers may use this to size their buffer.
 *
 * On platforms where extended attributes are not supported, the function
 * fails with @c errno set to @c ENOSYS.
 */

/**
 * @fn int file_setxattr(const char *file, const char *attrname,
 *                       const void *buf, size_t len, int flags)
 * 
 * @param file     path to the file whose extended attribute should be set
 * @param attrname name of the extended attribute
 * @param buf      pointer to the attribute value to store
 * @param len      size of the attribute value in bytes
 * @param flags    platform-specific flags (e.g., @c XATTR_CREATE,
 *                 @c XATTR_REPLACE on systems that support them)
 * 
 * @return 0 on success, or @c -1 if an error occurs
 *
 * This function provides a portable wrapper for setting extended file
 * attributes.
 *
 * On Linux and other systems providing @c setxattr(2), it is a direct
 * alias of that system call. On Mac OS X, it wraps @c setxattr(2) with
 * the appropriate arguments for named attributes.
 *
 * On Windows, this function emulates extended attributes using NTFS EA
 * data streams and the BackupWrite API. It writes or replaces the EA
 * entry whose name matches @p attrname with the value found in @p buf.
 *
 * Depending on the platform, @p flags may be ignored. On systems that
 * support @c XATTR_CREATE and @c XATTR_REPLACE, callers can use these
 * to control creation versus replacement semantics.
 *
 * On platforms where extended attributes are not supported, the function
 * fails with @c errno set to @c ENOSYS.
 */

#endif
