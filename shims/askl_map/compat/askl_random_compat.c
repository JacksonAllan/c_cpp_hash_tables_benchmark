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

#include "askl_random_compat.h"

/* -------------------------------------------------------------------------- */
#ifdef WIN32 /* WIN32 compatibility */
/* -------------------------------------------------------------------------- */

ASKL_API int random_seed(uint32_t *out, size_t words)
{
    HCRYPTPROV prov;
    BOOL ret;
    size_t bytes;

    if (! out || ! words) return -1;

    bytes = words * sizeof(*out);

    #ifdef PROV_RSA_AES
    ret = CryptAcquireContextA(
        & prov,
        NULL,
        NULL,
        PROV_RSA_AES,
        CRYPT_VERIFYCONTEXT | CRYPT_SILENT
    );
    if (! ret)
    #endif
    ret = CryptAcquireContextA(
        & prov,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT | CRYPT_SILENT
    );

    if (! ret) return -1;

    if (! CryptGenRandom(prov, (DWORD) bytes, (BYTE *) out)) {
        CryptReleaseContext(prov, 0);
        return -1;
    }

    CryptReleaseContext(prov, 0);

    return 0;
}

/* -------------------------------------------------------------------------- */
#else /* POSIX compatibility */
/* -------------------------------------------------------------------------- */

ASKL_API int random_seed(uint32_t *out, size_t words)
{
    int fd;
    size_t i, bytes;
    ssize_t ret = 0;
    unsigned char *p = (unsigned char *) out;

    if (! out || ! words) return -1;
    
    bytes = words * sizeof(*out);

    if ( (fd = open("/dev/urandom", O_RDONLY)) == -1) {
        perror(ERR(random_seed, open));
        return -1;
    }

    for (i = 0; i < bytes; i += ret) {
        if ( (ret = read(fd, p + i, bytes - i)) <= 0) {
            if (errno == EINTR) continue;
            perror(ERR(random_seed, read));
            close(fd);
            return -1;
        }
    }

    close(fd);

    return 0;
}

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */
