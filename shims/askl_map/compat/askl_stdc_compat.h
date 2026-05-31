/*******************************************************************************
 *  ASKL.                                                                      *
 *  Copyright (c) 2025 Raphael Prevost <raph@el.bzh>                           *
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

#ifndef ASKL_STDC_COMPAT_H

#define ASKL_STDC_COMPAT_H

#include <errno.h>

/* math.h */

#ifndef NAN

#if defined(_MSC_VER) && (_MSC_VER < 1300)
    #include <float.h>
    #include <ymath.h>
    #define NAN _Nan._D
    #define isnan _isnan
#else
    #define NAN (0.0 / 0.0)
#endif

#endif

#ifndef INF
#define INF (1e+999)
#if defined(_MSC_VER) && (_MSC_VER < 1300)
#define isinf(x) (!_finite((x)) && !_isnan((x)))
#endif
#endif

/* long long support */

#ifndef LLONG_MAX
    /* Minimum and maximum values a `signed long long int' can hold.  */
    #ifdef _I64_MAX
        #define LLONG_MAX _I64_MAX
    #else
        #define LLONG_MAX 9223372036854775807LL
    #endif
#endif

#ifndef LLONG_MIN
    #ifdef _I64_MIN
        #define LLONG_MIN ((__int64) _I64_MIN)
    #else
        #define LLONG_MIN (-LLONG_MAX - 1LL)
    #endif
#endif

#ifndef ULLONG_MAX
    /* Maximum value an `unsigned long long int' can hold.  (Minimum is 0.)  */
    #ifdef _UI64_MAX
        #define ULLONG_MAX _UI64_MAX
    #else
        #define ULLONG_MAX   18446744073709551615ULL
    #endif
#endif

/* -------------------------------------------------------------------------- */
#ifdef WIN32 /* WIN32 compatibility for common functions or keywords */
/* -------------------------------------------------------------------------- */

#define DIR_SEP_CHR '\\'
#define DIR_SEP_STR "\\"

/* on Win32, both \ and / are valid path separators */
#define isdirsepchr(c) ( ((c) == '\\' || (c) == '/') )

#ifndef snprintf
    #define snprintf _snprintf
#endif

#ifndef win32error
    #define win32error(s)                                                \
    do {                                                                 \
        DWORD __win32_error = GetLastError();                            \
        LPSTR __win32_errmsg = NULL;                                     \
        FormatMessageA(                                                  \
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, \
            NULL,                                                        \
            __win32_error,                                               \
            MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),                    \
            (LPSTR) & __win32_errmsg,                                    \
            256,                                                         \
            NULL                                                         \
        );                                                               \
        fprintf(stderr, s ": %s\n", __win32_errmsg);                     \
        LocalFree(__win32_errmsg);                                       \
    } while (0)
#endif

#if (defined(_MSC_VER) && (_MSC_VER < 1300))
    /* C99 booleans */
    #define bool     int
    #define true     1
    #define false    0
    /* C99 int types */
    #define int8_t   char
    #define uint8_t  unsigned char
    #define int16_t  short
    #define uint16_t unsigned short
    #define int32_t  int
    #define uint32_t unsigned int
    #define int64_t  __int64
    #define uint64_t unsigned __int64
    /* C99 limits */
    #define INT8_MIN     SCHAR_MIN
    #define INT8_MAX     SCHAR_MAX
    #define UINT8_MAX    UCHAR_MAX
    #define INT16_MIN    SHRT_MIN
    #define INT16_MAX    SHRT_MAX
    #define UINT16_MAX   USHRT_MAX
    #define INT32_MIN    INT_MIN
    #define INT32_MAX    INT_MAX
    #define UINT32_MAX   UINT_MAX
    #define INT64_MIN    (-9223372036854775807i64 - 1)
    #define INT64_MAX    9223372036854775807i64
    #define UINT64_MAX   18446744073709551615ui64

    #ifdef _WIN64
        #define intptr_t  __int64
        #define uintptr_t unsigned __int64
        #define INTPTR_MIN   INT64_MIN
        #define INTPTR_MAX   INT64_MAX
        #define UINTPTR_MAX  UINT64_MAX
    #else
        #define intptr_t  int
        #define uintptr_t unsigned int
        #define INTPTR_MIN   INT_MIN
        #define INTPTR_MAX   INT_MAX
        #define UINTPTR_MAX  UINT_MAX
    #endif

    #define intmax_t  __int64
    #define uintmax_t unsigned __int64
    #define INTMAX_MIN   INT64_MIN
    #define INTMAX_MAX   INT64_MAX
    #define UINTMAX_MAX  UINT64_MAX
#endif

typedef uint32_t unaligned_uint32_t;

#ifndef _strtoi64
    ASKL_API int64_t strtoll(const char *nptr, char **endptr, int base);
    #define strtoull (uint64_t) strtoll
#else
    #define strtoll _strtoi64
    #define strtoull (uint64_t) _strtoi64
#endif

#if (defined(_MSC_VER) && (_MSC_VER < 1800))
    /* va_copy is available from MSVC2013 onward */
    #define va_copy(a, b) do { a = (b); } while(0)
#endif

#define INVALID_FILE_HANDLE ((HANDLE)INVALID_HANDLE_VALUE)

/* -------------------------------------------------------------------------- */
#else
/* -------------------------------------------------------------------------- */

#include <sys/resource.h>

#define DIR_SEP_CHR '/'
#define DIR_SEP_STR "/"

#define isdirsepchr(c) ( ((c) == '/') )

#if (! defined(strtoll) && defined(strtoq))
    #define strtoll strtoq
#endif

#if (! defined(strtoull) && defined(strtouq))
    #define strtoull strtouq
#endif

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

#ifdef _MAX_PATH
    #define FILNAMSIZ (_MAX_PATH)
#else
    #define FILNAMSIZ 1025
#endif

#ifndef PATH_MAX
    #ifdef MAXPATHLEN
        #define PATH_MAX MAXPATHLEN
    #else
        #if FILENAME_MAX > 1024
            #define PATH_MAX FILENAME_MAX
        #else
            #define PATH_MAX (FILNAMSIZ - 1)
        #endif
    #endif
#endif

#if defined(_MSC_VER) || defined(_WIN32) || \
    ! (defined(__APPLE__) || defined(_DARWIN_C_SOURCE) || \
       defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE) || \
       defined(_BSD_SOURCE) || defined(_SVID_SOURCE) || \
       (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L) || \
       (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 700))
ASKL_API char *strndup(const char *s, size_t len);
#endif

#endif
