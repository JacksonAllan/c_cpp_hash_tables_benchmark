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

#include "askl_mmap_compat.h"

/* -------------------------------------------------------------------------- */
#ifdef WIN32 /* WIN32 mmap() compatibility module */
/* -------------------------------------------------------------------------- */

ASKL_API int get_page_size(void)
{
    #ifndef __WINE__
    SYSTEM_INFO info;
    GetSystemInfo(& info);
    return info.dwPageSize;
    #else
    /* FIXME dwPageSize is set to 0 under winelib ? */
    return 4096;
    #endif
}

/* -------------------------------------------------------------------------- */

/*
  _align_malloc and friends, implemented using Microsoft's public
  interfaces and with the help of the algorithm description provided
  by Wu Yongwei: http://sourceforge.net/mailarchive/message.php?msg_id=3847075

  I hereby place this implementation in the public domain.
               -- Steven G. Johnson (stevenj@alum.mit.edu)
*/

#define NOT_POWER_OF_TWO(n) (((n) & ((n) - 1)))
#define UI(p) ((uintptr_t) (p))
#define CP(p) ((char *) p)

#define PTR_ALIGN(p0, alignment, offset)                                      \
                  ((void *) (((UI(p0) + (alignment + sizeof(void*)) + offset) \
                              & (~UI(alignment - 1))) - offset))

/* pointer must sometimes be aligned; assume sizeof(void*) is a power of two */
#define ORIG_PTR(p) (*(((void **) (UI(p) & (~UI(sizeof(void*) - 1)))) - 1))

/* -------------------------------------------------------------------------- */

static void *_aligned_offset_alloc(size_t size, size_t alignment, size_t offset)
{
    void *p0 = NULL, *p = NULL;

    if (NOT_POWER_OF_TWO(alignment)) {
        errno = EINVAL;
        return NULL;
    }

    if (! size) return NULL;

    if (alignment < sizeof(void *)) alignment = sizeof(void *);

    /* including the extra sizeof(void*) is overkill on a 32-bit
       machine, since malloc is already 8-byte aligned, as long
       as we enforce alignment >= 8 ...but oh well */
    if (! (p0 = malloc(size + (alignment + sizeof(void *)))) )
        return NULL;

    p = PTR_ALIGN(p0, alignment, offset); ORIG_PTR(p) = p0;

    return p;
}

/* -------------------------------------------------------------------------- */

ASKL_API int posix_memalign(void **p, size_t alignment, size_t size)
{
    if ( (*p = _aligned_offset_alloc(size, alignment, 0)) )
        return 0;
    return -1;
}

/* -------------------------------------------------------------------------- */

ASKL_API void posix_memfree(void *memblock)
{
    if (memblock) free(ORIG_PTR(memblock));
}

/* -------------------------------------------------------------------------- */

ASKL_API void *mmap(
    void *start,
    size_t len,
    int prot,
    int flags,
    int fd,
    off_t offset
)
{
    /*
      This is a minimal implementation of the *NIX mmap syscall for WIN32.
      It supports the following protections flags :
      PROT_NONE, PROT_READ, PROT_WRITE, PROT_EXEC
      It handles the following mapping flags:
      MAP_FIXED, MAP_SHARED, MAP_PRIVATE (POSIX) and MAP_ANONYMOUS (SVID)
    */

    void *ret = MAP_FAILED;
    HANDLE hmap = INVALID_HANDLE_VALUE;
    long wprot = 0, wflags = 0;

    if (~flags & MAP_ANONYMOUS) {
        if ( (hmap = (HANDLE) _get_osfhandle(fd)) == INVALID_HANDLE_VALUE) {
            /* non-file-backed mapping is only allowed with MAP_ANONYMOUS */
            errno = EBADF;
            return MAP_FAILED;
        }
    }

    /* map *NIX protections and flags to their WIN32 equivalents */
    if ( (prot & PROT_READ) && (~prot & PROT_WRITE) ) {
        /* read only, maybe exec */
        wprot = (prot & PROT_EXEC) ? (PAGE_EXECUTE_READ) : (PAGE_READONLY);
        wflags = (prot & PROT_EXEC) ? (FILE_MAP_EXECUTE) : (FILE_MAP_READ);
    } else if (prot & PROT_WRITE) {
        /* read/write, maybe exec */
        if ( (flags & MAP_SHARED) && (~flags & MAP_PRIVATE) ) {
            /* changes are committed to the file */
            wprot = (prot & PROT_EXEC) ?
                    (PAGE_EXECUTE_READWRITE) : (PAGE_READWRITE);
            wflags = (prot & PROT_EXEC) ?
                    (FILE_MAP_EXECUTE) : (FILE_MAP_WRITE);
        } else if ( (flags & MAP_PRIVATE) && (~flags & MAP_SHARED) ) {
            /* does not affect the original file */
            wprot = PAGE_WRITECOPY; wflags = FILE_MAP_COPY;
        } else {
            /* MAP_PRIVATE + MAP_SHARED is not allowed, abort */
            errno = EINVAL;
            return MAP_FAILED;
        }
    }

    /* create the windows map object */
    hmap = CreateFileMapping(hmap, NULL, wprot, 0, len, NULL);

    if (! hmap) {
        /* the fd was checked before, so it must have bad access rights */
        errno = EPERM;
        return MAP_FAILED;
    }

    /* create a view */
    ret = MapViewOfFileEx(
        hmap,
        wflags,
        0,
        offset,
        len,
        (flags & MAP_FIXED) ? (start) : (NULL)
    );

    /* drop the map, it will not be deleted until last 'view' is closed */
    CloseHandle(hmap);

    if (! ret) {
        /* if MAP_FIXED was set, the address was probably wrong */
        errno = (flags & MAP_FIXED) ? (EINVAL) : (ENOMEM);
        return MAP_FAILED;
    }

    return ret;
}

/* -------------------------------------------------------------------------- */

ASKL_API int munmap(void *start, UNUSED size_t _dummy)
{
    /*
      This is a minimal implementation of the *NIX munmap syscall for WIN32.
      The size parameter is ignored under Win32.
    */

    if (start == NULL) { errno = EINVAL; return -1; }

    return UnmapViewOfFile(start) ? 0 : -1;
}

/* -------------------------------------------------------------------------- */

ASKL_API void *shm_alloc(const char *name, size_t size)
{
    HANDLE hmap = NULL;
    void *ret = NULL;

    if (! name || ! size) return NULL;

    /* create a named mapping object */
    hmap = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        size,
        name
    );

    /* we don't want to inadvertently open an existing shared memory object */
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hmap);
        return NULL;
    }

    if (! hmap) { perror(ERR(shm_alloc, CreateFileMapping)); return NULL; }

    /* map the shared memory */
    ret = MapViewOfFileEx(hmap, FILE_MAP_WRITE, 0, 0, size, NULL);

    CloseHandle(hmap);

    if (! ret) { perror(ERR(shm_alloc, MapViewOfFileEx)); return NULL; }

    return ret;
}

/* -------------------------------------------------------------------------- */

ASKL_API void *shm_attach(const char *name, size_t size)
{
    HANDLE hmap = NULL;
    void *ret = NULL;

    if (! name || ! size) return NULL;

    hmap = OpenFileMapping(PAGE_READWRITE, 0, name);
    if (! hmap) { perror(ERR(shm_alloc, OpenFileMapping)); return NULL; }

    /* map the shared memory */
    ret = MapViewOfFileEx(hmap, FILE_MAP_WRITE, 0, 0, size, NULL);

    CloseHandle(hmap);

    if (! ret) { perror(ERR(shm_alloc, MapViewOfFileEx)); return NULL; }

    return ret;
}

/* -------------------------------------------------------------------------- */

ASKL_API void shm_detach(void *start, UNUSED size_t _dummy)
{
    /* we already dropped the reference of the map object, just unmap it */
    UnmapViewOfFile(start);
}

/* -------------------------------------------------------------------------- */

ASKL_API void shm_free(UNUSED const char *_name, void *start, UNUSED size_t _dummy)
{
    /* we already dropped the reference of the map object, just unmap it */
    UnmapViewOfFile(start);
}

/* -------------------------------------------------------------------------- */
#else /* POSIX compliant systems */
/* -------------------------------------------------------------------------- */

ASKL_API int get_page_size(void)
{
    return sysconf(_SC_PAGESIZE);
}

/* -------------------------------------------------------------------------- */
#ifdef __APPLE__
/* -------------------------------------------------------------------------- */

#if ! defined(MAC_OS_X_VERSION_10_6) && ! defined(__MAC_10_6)

ASKL_API int posix_memalign(void **p, UNUSED size_t alignment, size_t size)
{
    /* malloc returns 16-byte aligned memory addresses on OS X */
    return ( (*p = malloc(size)) ) ? 0 : -1;
}

#endif

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

ASKL_API void posix_memfree(void *memblock)
{
    free(memblock);
}

/* -------------------------------------------------------------------------- */

/* mmap(2) and munmap(2) are already there, wrap around shm_open(2) */

ASKL_API void *shm_alloc(const char *name, size_t size)
{
    int shm = 0;
    void *ret = NULL;

    if (! name || ! size) return NULL;

    shm = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (shm == -1) {
        perror(ERR(shm_alloc, shm_open));
        return NULL;
    }

    if (ftruncate(shm, size) == -1) {
        perror(ERR(shm_alloc, ftruncate));
        goto _err_shm;
    }

    /* map the shared memory to the process address space */
    ret = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0x0);
    if (ret == MAP_FAILED) {
        perror(ERR(shm_alloc, mmap));
        goto _err_shm;
    }

    close(shm);

    return ret;

_err_shm:
    close(shm); shm_unlink(name);
    return NULL;
}

/* -------------------------------------------------------------------------- */

ASKL_API void *shm_attach(const char *name, size_t size)
{
    int shm = 0;
    void *ret = NULL;

    if (! name || ! size) return NULL;

    /* try to get the shared memory descriptor */
    shm = shm_open(name, O_RDWR, 0);
    if (shm == -1) {
        perror(ERR(shm_attach, shm_open));
        return NULL;
    }

    /* map the shared memory to the process address space */
    ret = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0x0);
    if (ret == MAP_FAILED) {
        perror(ERR(shm_attach, mmap));
        ret = NULL;
    }

    close(shm);

    return ret;
}

/* -------------------------------------------------------------------------- */

ASKL_API void shm_detach(void *start, size_t size)
{
    munmap(start, size);
}

/* -------------------------------------------------------------------------- */

ASKL_API void shm_free(const char *name, void *start, size_t size)
{
    /* unmap and unlink */
    munmap(start, size);
    shm_unlink(name);
}

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */
