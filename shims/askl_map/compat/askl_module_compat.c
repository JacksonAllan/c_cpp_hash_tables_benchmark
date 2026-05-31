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

#include "askl_module_compat.h"

/* -------------------------------------------------------------------------- */
#ifdef WIN32
/* -------------------------------------------------------------------------- */

#ifdef __GNUC__
static __thread char _dlerror_buffer[1024];
#else
static __declspec(thread) char _dlerror_buffer[1024];
#endif

INTERNAL const char *dlerror(void)
{
    DWORD err = GetLastError();

    if (! err) return NULL;

    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        _dlerror_buffer,
        (DWORD) sizeof(_dlerror_buffer),
        NULL
    );

    return _dlerror_buffer;
}

/* -------------------------------------------------------------------------- */
#elif (defined(__APPLE__))
/* -------------------------------------------------------------------------- */

#if (! defined(MAC_OS_X_VERSION_10_3) && ! defined(__MAC_10_3))
/* use dlcompat up to Mac OS 10.2.x */

/*
Copyright (c) 2002 Peter O'Gorman <ogorman@users.sourceforge.net>

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


/* Just to prove that it isn't that hard to add Mac calls to your code :)
   This works with pretty much everything, including kde3 xemacs and the gimp,
   I'd guess that it'd work in at least 95% of cases, use this as your starting
   point, rather than the mess that is dlfcn.c, assuming that your code does not
   require ref counting or symbol lookups in dependent libraries
*/

#define ERR_STR_LEN 256
static void *dlsymIntern(void *handle, const char *symbol);
static const char *error(int setget, const char *str, ...);

/* -------------------------------------------------------------------------- */

/* Set and get the error string for use by dlerror */
static const char *error(int setget, const char *str, ...)
{
    static char errstr[ERR_STR_LEN];
    static int err_filled = 0;
    const char *retval;
    NSLinkEditErrors ler;
    int lerno;
    const char *dylderrstr;
    const char *file;
    va_list arg;

    if (setget <= 0) {

        va_start(arg, str);
        strncpy(errstr, "dlsimple: ", ERR_STR_LEN);
        vsnprintf(errstr + 10, ERR_STR_LEN - 10, str, arg);
        va_end(arg);

        /* We prefer to use the dyld error string if getset is 1*/
        if (setget == 0) {
            NSLinkEditError(&ler, &lerno, &file, &dylderrstr);
            fprintf(stderr, "dyld: %s\n", dylderrstr);
            if (dylderrstr && strlen(dylderrstr))
                strncpy(errstr, dylderrstr, ERR_STR_LEN);
        }
        err_filled = 1;
        retval = NULL;

    } else {
        retval = (! err_filled) ? NULL : errstr;
        err_filled = 0;
    }

    return retval;
}

/* -------------------------------------------------------------------------- */

/* dlopen */
INTERNAL void *dlopen(const char *path, int mode)
{
    void *module = 0;
    NSObjectFileImage ofi = 0;
    NSObjectFileImageReturnCode ofirc;
    static int (*make_private_module_public)(NSModule module) = 0;
    unsigned int flags =
    NSLINKMODULE_OPTION_RETURN_ON_ERROR | NSLINKMODULE_OPTION_PRIVATE;

    /* If we got no path, the app wants the global namespace, use -1 as the marker
       in this case */
    if (! path) return (void *) -1;

    /* Create the object file image, works for things linked
       with the -bundle arg to ld */
    ofirc = NSCreateObjectFileImageFromFile(path, &ofi);

    switch (ofirc) {

        case NSObjectFileImageSuccess:
            /* It was okay, so use NSLinkModule to link in the image */
            if (!(mode & RTLD_LAZY)) flags += NSLINKMODULE_OPTION_BINDNOW;
            module = NSLinkModule(ofi, path,flags);
            /* Don't forget to destroy the object file image, unless you like leaks */
            NSDestroyObjectFileImage(ofi);
            /* If the mode was global, then change the module, this avoids
               multiply defined symbol errors to first load private then make
               global. Silly, isn't it. */
            if ((mode & RTLD_GLOBAL)) {
                if (!make_private_module_public) {
                    _dyld_func_lookup("__dyld_NSMakePrivateModulePublic",
                                      (unsigned long *) & make_private_module_public);
                }
                make_private_module_public(module);
            }
            break;

        case NSObjectFileImageInappropriateFile:
            /* It may have been a dynamic library rather than a bundle,
               try to load it */
            module = (void *) NSAddImage(path, NSADDIMAGE_OPTION_RETURN_ON_ERROR);
            break;

        case NSObjectFileImageFailure:
            error(0,"Object file setup failure :  \"%s\"", path);
            return 0;

        case NSObjectFileImageArch:
            error(0,"No object for this architecture :  \"%s\"", path);
            return 0;

        case NSObjectFileImageFormat:
            error(0,"Bad object file format :  \"%s\"", path);
            return 0;

        case NSObjectFileImageAccess:
            error(0,"Can't read object file :  \"%s\"", path);
            return 0;
    }

    if (! module) error(0, "Can not open \"%s\"", path);

    return module;
}

/* -------------------------------------------------------------------------- */

/* dlsymIntern is used by dlsym to find the symbol */
static void *dlsymIntern(void *handle, const char *symbol)
{
    NSSymbol *nssym = 0;

    /* If the handle is -1, it is the app global context */
    if (handle == (void *) -1) {
        /* Global context, use NSLookupAndBindSymbol */
        if (NSIsSymbolNameDefined(symbol)) nssym = NSLookupAndBindSymbol(symbol);

    } else {
        /* Now see if the handle is a struch mach_header * or not,
           use NSLookupSymbol in image for libraries, and
           NSLookupSymbolInModule for bundles */

        /* Check for both possible magic numbers depending on x86/ppc byte order */
        if ( (((struct mach_header *) handle)->magic == MH_MAGIC) ||
             (((struct mach_header *) handle)->magic == MH_CIGAM)) {
            if (NSIsSymbolNameDefinedInImage((struct mach_header *) handle, symbol)) {
                nssym = NSLookupSymbolInImage((struct mach_header *) handle,
                                              symbol,
                                              NSLOOKUPSYMBOLINIMAGE_OPTION_BIND |
                                              NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR);
            }

        } else nssym = NSLookupSymbolInModule(handle, symbol);
    }

    if (! nssym) {
        error(0, "Symbol \"%s\" Not found", symbol);
        return NULL;
    }

    return NSAddressOfSymbol(nssym);
}

/* -------------------------------------------------------------------------- */

INTERNAL const char *dlerror(void)
{
    return error(1, (char *) NULL);
}

/* -------------------------------------------------------------------------- */

INTERNAL int dlclose(void *handle)
{
    if ( (((struct mach_header *) handle)->magic == MH_MAGIC) ||
         (((struct mach_header *) handle)->magic == MH_CIGAM) ) {
        error(-1, "Can't remove dynamic libraries on darwin");
        return 0;
    }

    if (! NSUnLinkModule(handle, 0)) {
        error(0, "unable to unlink module %s", NSNameOfModule(handle));
        return 1;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

/* dlsym, prepend the underscore and call dlsymIntern */
INTERNAL void *dlsym(void *handle, const char *symbol)
{
    static char undersym[257];  /* Saves calls to malloc(3) */
    int sym_len = strlen(symbol);
    void *value = NULL;
    char *malloc_sym = NULL;

    if (sym_len < 256) {
        snprintf(undersym, 256, "_%s", symbol);
        value = dlsymIntern(handle, undersym);
    } else {
        if ( (malloc_sym = malloc(sym_len + 2)) ) {
            sprintf(malloc_sym, "_%s", symbol);
            value = dlsymIntern(handle, malloc_sym);
            free(malloc_sym);
        } else {
            error(-1, "Unable to allocate memory");
        }
    }

    return value;
}

#endif /* MAC OS X 10.2 */

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */
