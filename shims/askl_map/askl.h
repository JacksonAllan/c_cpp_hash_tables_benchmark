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

#ifndef ASKL_H

#define ASKL_H

/** ASKL version string */
#define ASKL_VERSION "0.3.9"

/** ASKL API revision */
#define __ASKL__ 1390

/* check the compiler settings */
#ifdef __GNUC__
    /* gcc settings */
    #ifdef _WIN32
        #ifndef WIN32
            #define WIN32
        #endif
    #else
        #ifndef __APPLE__
            #ifndef _POSIX_C_SOURCE
                #define _POSIX_C_SOURCE 200809L
            #endif
            #ifndef _XOPEN_SOURCE
                #define _XOPEN_SOURCE 700
            #endif
            #define _SVID_SOURCE 1
        #else
            #define _DARWIN_C_SOURCE
        #endif
        #define _DEFAULT_SOURCE
        #define _THREAD_SAFE
        #define _REENTRANT
    #endif
#endif

/* standard ISO C99/POSIX includes */
#include <assert.h>
#include <math.h>
#include <wchar.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <wctype.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <locale.h>
#include <pthread.h>
#include <time.h>

/* check system endianness */
#if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && \
     __BYTE_ORDER == __LITTLE_ENDIAN) || \
    (defined(__APPLE__) && defined(__LITTLE_ENDIAN__)) || \
    (defined(__i386) || defined(__x86_64__) || defined(__ia64) || \
     defined(_M_IX86) || defined(_M_X64) || defined(_M_IA64) || \
     defined(__ARMEL__) || defined(_M_ARM) || defined(__MIPSEL__))
    #define LITTLE_ENDIAN_HOST
#elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && \
       __BYTE_ORDER == __BIG_ENDIAN) || \
      (defined(__APPLE__) && defined(__BIG_ENDIAN__)) || \
      (defined(__sparc) || defined(__powerpc__) || defined(__ppc__) || \
       defined(__mc68000) || defined(__ARMEB__) || defined(__MIPSEB__))
    #define BIG_ENDIAN_HOST
#endif

#if ! defined(_MSC_VER) || _MSC_VER > 1300
/* Microsoft Visual C++ 6.0 is missing these includes */
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#if _MSC_VER >= 1400
#include <intrin.h>
#endif
#endif

/* specific macros cleanup */
#undef format
#undef UNUSED
#undef INTERNAL
#undef likely
#undef unlikely

#ifndef UNUSED
    #ifdef __GNUC__
        #define UNUSED __attribute__ ((unused))
    #else
        #define UNUSED
    #endif
#endif

#if (! defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L))
    #ifdef _MSC_VER
        #if (_MSC_VER >= 1400)
            #if (_MSC_VER < 1900)
                #define restrict __restrict
            #endif
        #else
            #define restrict
        #endif
    #elif (defined(__GNUC__))
        #define restrict __restrict__
    #else
        #define restrict
    #endif
#endif

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112))
    #define ALIGNOF(type) _Alignof(type)
#elif (defined(__GNUC__) || defined(__clang__))
    #define ALIGNOF(type) __alignof__(type)
#elif defined(_MSC_VER)
    #define ALIGNOF(type) __alignof(type)
#endif

#ifdef __GNUC__
    /* specific macros */
    #define format(f, v) __attribute__ ((format(printf, (f), (v))))
    #ifndef WIN32
        #define ASKL_API
        #ifdef DEBUG
            #define INTERNAL
            #ifndef __APPLE__
            #include <execinfo.h>
            #define backtrace()                                 \
            do {                                                \
                void *array[10];                                \
                size_t size;                                    \
                char **strings;                                 \
                size_t i;                                       \
                                                                \
                size = backtrace(array, 10);                    \
                strings = backtrace_symbols(array, size);       \
                                                                \
                fprintf(stderr, "======= BACKTRACE =======\n"); \
                for (i = 0; i < size; i++)                      \
                    printf ("%s\n", strings[i]);                \
                fprintf(stderr, "=========================\n"); \
                                                                \
                free (strings);                                 \
            } while (0);
            #else
            #define backtrace()
            #endif
            typedef uint32_t unaligned_uint32_t __attribute__((aligned(1)));
        #else
            #ifndef __APPLE__
            #define INTERNAL __attribute__((visibility("internal")))
            #else
            #define INTERNAL __attribute__((visibility("hidden")))
            #endif
            #define backtrace()
            typedef uint32_t unaligned_uint32_t;
        #endif
    #endif
    #define likely(x)    __builtin_expect(!!(x), 1)
    #define unlikely(x)  __builtin_expect(!!(x), 0)

    /* handle the dllimport related noise in pthread.h with winegcc */
    #if (defined(__WINE__) || defined(__MINGW32__))
        #define dllimport
    #endif
#endif

/* basic fixes for WIN32 includes (Wine) */
#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <Shlwapi.h>
    #include <errno.h>
    #if ! defined(off_t) && defined(__WINE__)
        #define off_t _off_t
    #endif

    #if defined(BUILDING_DLL) || defined(_USRDLL) || defined(_WINDLL)
        #define ASKL_API __declspec(dllexport)
    #else
        #define ASKL_API __declspec(dllimport)
    #endif

    #define INTERNAL

    #ifndef socklen_t
        #define socklen_t size_t
    #endif

    #ifndef ssize_t
        #define ssize_t long
    #endif
#endif

/* project specific macros */
#define STR(x)  STRINGIFY(x)
#define ERR(c, f) #c "()::" #f  "() @ " __FILE__ ":" STR(__LINE__)
#define die(s) do { fprintf(stderr, (s)); abort(); } while (0)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define STATIC_ASSERT(cond, name) typedef int assert_##name[(cond) ? 1 : -1]

#ifdef DEBUG
    #define debug(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
    #if defined(_MSC_VER) && _MSC_VER < 1300
        #pragma warning(disable:4002)
        #define debug(__VA_ARGS__) do { } while (0)
    #else
        #define debug(...) do { } while (0)
    #endif
#endif

#ifndef CALLBACK
    #ifdef __GNUC__
        #ifdef __i386
            #define CALLBACK __attribute__ ((regparm(1)))
        #else
            /* x86_64 already uses registers to pass function parameters */
            #define CALLBACK
        #endif
    #else
        /* XXX the WIN32 __fastcall calling convention uses too many
           registers to be suitable for callbacks */
        #define CALLBACK
    #endif
#endif

/* various OS dependant definitions */
#include "compat/askl_compat_layer.h"

/* external packages */
#ifndef ASKL_MINIMAL

/* OpenSSL */
#if (_ENABLE_SSL && HAS_SSL)
    #include <openssl/bio.h>
    #include <openssl/ssl.h>
    #include <openssl/err.h>
#else
    #undef _ENABLE_SSL
#endif

/* MySQL */
#if (_ENABLE_MYSQL && HAS_MYSQL)
    #include <mysql/mysql.h>
#else
    #undef _ENABLE_MYSQL
#endif

/* SQLite */
#if (_ENABLE_SQLITE && HAS_SQLITE)
    #include <sqlite3.h>
#else
    #undef _ENABLE_SQLITE
#endif

#endif

/* privileges separation */
#ifdef _ENABLE_PRIVILEGE_SEPARATION
    /* force inclusion of the server code if we use privilege separation */
    #ifndef _ENABLE_SERVER
        #define _ENABLE_SERVER
    #endif
    #define OP_AUTH 0x0A
    #define OP_BIND 0x0B
    #define OP_CONF 0x0C
    #define OP_EXIT 0x0E
    extern int server_privileged_call(int opcode, const void *cmd, size_t len);
#endif

/* environment */
ASKL_API extern char *working_directory;

#ifndef PREFIX
    #define PREFIX (working_directory)
#endif

#ifndef CONFDIR
    #define CONFDIR (working_directory)
#endif

#ifndef SHAREDIR
    #define SHAREDIR (working_directory)
#endif

#endif
