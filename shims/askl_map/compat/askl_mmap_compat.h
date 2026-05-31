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

#ifndef ASKL_MMAP_COMPAT_H

#define ASKL_MMAP_COMPAT_H

/* -------------------------------------------------------------------------- */
#ifdef WIN32 /* WIN32 mmap() compatibility */
/* -------------------------------------------------------------------------- */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <errno.h>

#ifndef off_t
    #ifdef _off_t
        #define off_t _off_t
    #else
        #define off_t long
    #endif
#endif

/* standard mmap() definitions - shamelessly ripped off bits/mman.h */

#define MAP_FAILED ((void *) -1)

/* Protections are chosen from these bits, OR'd together.  The
   implementation does not necessarily support PROT_EXEC or PROT_WRITE
   without PROT_READ.  The only guarantees are that no writing will be
   allowed without PROT_WRITE and no access will be allowed for PROT_NONE. */
#define PROT_READ       0x1             /* Page can be read.  */
#define PROT_WRITE      0x2             /* Page can be written.  */
#define PROT_EXEC       0x4             /* Page can be executed.  */
#define PROT_NONE       0x0             /* Page can not be accessed.  */
/* Sharing types (must choose one and only one of these).  */
#define MAP_SHARED      0x01            /* Share changes.  */
#define MAP_PRIVATE     0x02            /* Changes are private.  */
/* Other flags.  */
#define MAP_FIXED       0x10            /* Interpret addr exactly.  */
#define MAP_ANONYMOUS   0x20            /* Don't use a file.  */
#define MAP_ANON        MAP_ANONYMOUS

#ifndef FILE_MAP_EXECUTE
    #define FILE_MAP_EXECUTE 0x0
#endif

ASKL_API int posix_memalign(void **p, size_t alignment, size_t size);

ASKL_API void *mmap(void *start, size_t len, int prot, int flags, int fd,
                  off_t offset                                         );
ASKL_API int munmap(void *start, UNUSED size_t _dummy);

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
#else /* Standard mmap() on SVID compliant systems */
/* -------------------------------------------------------------------------- */

#define _SVID_SOURCE 1
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> /* S_IRUSR etc... */
#include <fcntl.h> /* O_* consts in shm_open(2) */

#ifndef MAP_ANON
#define MAP_ANON MAP_ANONYMOUS
#endif

#ifndef MAP_POPULATE
#define MAP_POPULATE 0x0    /* Linux-specific optimization */
#endif

#if ! defined(_SC_PAGESIZE) && defined(_SC_PAGE_SIZE)
#define _SC_PAGESIZE _SC_PAGE_SIZE
#endif

/* -------------------------------------------------------------------------- */
#ifdef __APPLE__
/* -------------------------------------------------------------------------- */

#if (! defined(MAC_OS_X_VERSION_10_6) && ! defined(__MAC_10_6))
/* OS X lacked posix_memalign() before Snow Leopard */
ASKL_API int posix_memalign(void **p, UNUSED size_t alignment, size_t size);
#endif

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

/* common definitions */
ASKL_API int get_page_size(void);

/**
 * @fn int get_page_size(void)
 * @param void
 * @return the operating system memory page size in bytes
 *
 * This function returns the memory page size used by the underlying
 * operating system.
 *
 * On POSIX systems, it wraps @c sysconf(_SC_PAGESIZE). On Windows, it
 * wraps @c GetSystemInfo() and returns @c dwPageSize.
 */

/* -------------------------------------------------------------------------- */

ASKL_API void posix_memfree(void *memblock);

/**
 * @fn void posix_memfree(void *memblock)
 * @param memblock a pointer returned by @ref posix_memalign()
 * @return void
 *
 * This function frees a memory block previously allocated with
 * @ref posix_memalign().
 *
 * On POSIX systems it simply calls @c free(). On Windows it uses the
 * internal bookkeeping required by the emulated @ref posix_memalign()
 * implementation.
 */

/* -------------------------------------------------------------------------- */

ASKL_API void *shm_alloc(const char *name, size_t size);

/**
 * @fn void *shm_alloc(const char *name, size_t size)
 * 
 * @param name name of the shared memory object
 * @param size size of the shared memory segment in bytes
 * 
 * @return a pointer to the mapped shared memory, or @c NULL on error
 *
 * This function allocates a new named shared memory segment and maps it
 * into the current process address space.
 *
 * On POSIX systems, it uses @c shm_open(3), @c ftruncate(2), and
 * @c mmap(2). On Windows, it uses @c CreateFileMapping() with
 * @c INVALID_HANDLE_VALUE and @c MapViewOfFileEx().
 *
 * The returned mapping must be detached with @ref shm_detach() and the
 * shared memory object must be destroyed with @ref shm_free() when no
 * longer needed.
 */

/* -------------------------------------------------------------------------- */

ASKL_API void *shm_attach(const char *name, size_t size);

/**
 * @fn void *shm_attach(const char *name, size_t size)
 * 
 * @param name name of an existing shared memory object
 * @param size expected size of the shared memory segment
 * 
 * @return a pointer to the mapped shared memory, or @c NULL on error
 *
 * This function opens an existing shared memory segment previously created
 * with @ref shm_alloc() and maps it into the current process address space.
 *
 * The returned mapping must be detached with @ref shm_detach().
 */

/* -------------------------------------------------------------------------- */

ASKL_API void shm_detach(void *start, size_t size);

/**
 * @fn void shm_detach(void *start, size_t size)
 * 
 * @param start address of a mapped shared memory segment
 * @param size  size of the mapping in bytes (ignored on some platforms)
 * 
 * @return void
 *
 * This function unmaps a shared memory segment from the current process,
 * without destroying the underlying shared memory object.
 */

/* -------------------------------------------------------------------------- */

ASKL_API void shm_free(const char *name, void *start, size_t size);

/**
 * @fn void shm_free(const char *name, void *start, size_t size)
 * 
 * @param name  name of the shared memory object
 * @param start address of a mapped shared memory segment
 * @param size  size of the mapping in bytes
 * 
 * @return void
 *
 * This function destroys a shared memory object and unmaps the associated
 * memory from the current process. After this call, other processes will
 * no longer be able to attach to the shared segment using @ref shm_attach().
 */

/* -------------------------------------------------------------------------- */

#endif
