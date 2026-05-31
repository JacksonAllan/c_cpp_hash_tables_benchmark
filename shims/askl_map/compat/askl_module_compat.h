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

#ifndef ASKL_MODULE_COMPAT_H

#define ASKL_MODULE_COMPAT_H

/* WIN32 LoadLibrary() API is quite similar to the dlfcn functions */
#ifdef WIN32

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <stdlib.h>

    #define handle_t HANDLE

    /* dlfcn.h compatibility can be achieved with simple macros */
    #define dlopen(f, o) LoadLibrary((f))
    #define dlsym(h, f)  GetProcAddress((h), (f))
    #define dlclose(h)   (! FreeLibrary((h)))

    /* dlerror is implemented using GetLastError() */
    INTERNAL const char *dlerror(void);

/* Mac OS 10.2 does not come with the dlopen api */
#elif (defined(__APPLE__) && ! defined(MAC_OS_X_VERSION_10_3))

    #define handle_t void *

    /* on Mac OS 10.2, use dlcompat */

    /*
    Copyright (c) 2002 Jorge Acereda  <jacereda@users.sourceforge.net> &
                    Peter O'Gorman <ogorman@users.sourceforge.net>

    Portions may be copyright others, see the AUTHORS file included with this
    distribution.

    Maintained by Peter O'Gorman <ogorman@users.sourceforge.net>

    Bug Reports and other queries should go to <ogorman@users.sourceforge.net>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    */

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <stdarg.h>
    #include <limits.h>
    #include <mach-o/dyld.h>

    #if defined (__GNUC__) && __GNUC__ > 3
    #define dl_restrict __restrict
    #else
    #define dl_restrict
    #endif

    /*
    * Structure filled in by dladdr().
    */

    typedef struct dl_info {
        const char *dli_fname;     /* Pathname of shared object */
        void       *dli_fbase;     /* Base address of shared object */
        const char *dli_sname;     /* Name of nearest symbol */
        void       *dli_saddr;     /* Address of nearest symbol */
    } Dl_info;

    extern void *dlopen(const char *path, int mode);
    extern void *dlsym(void * dl_restrict handle, const char *dl_restrict symbol);
    extern const char *dlerror(void);
    extern int dlclose(void *handle);
    extern int dladdr(const void *dl_restrict, Dl_info *dl_restrict);

    #define RTLD_LAZY   0x1
    #define RTLD_NOW    0x2
    #define RTLD_LOCAL  0x4
    #define RTLD_GLOBAL 0x8
    #define RTLD_NOLOAD 0x10
    #define RTLD_NODELETE   0x80

    /*
    * Special handle arguments for dlsym().
    */
    #define RTLD_NEXT       ((void *) -1)   /* Search subsequent objects. */
    #define RTLD_DEFAULT    ((void *) -2)   /* Use default search algorithm. */

#else

    /* it is assumed that other operating systems provide the dlopen api */
    #define handle_t void *
    #include <dlfcn.h>

#endif

#endif
