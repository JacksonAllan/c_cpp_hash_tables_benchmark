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

#include "askl_file_compat.h"

/* -------------------------------------------------------------------------- */
#ifdef WIN32 /* make WIN32 open() more like its UNIX counterpart */
/* -------------------------------------------------------------------------- */

ASKL_API int posix_open(const char *pathname, int flags, ...)
{
    unsigned int mode = 0;
    va_list args;

    /* XXX win32 opens files in _O_TEXT mode by default */
    flags |= _O_BINARY;

    if (flags & O_CREAT) {
        va_start(args, flags);
        mode = va_arg(args, int);
        va_end(args);
        return _sopen(pathname, flags, _SH_DENYNO, mode);
    }

    return _sopen(pathname, flags, _SH_DENYNO);
}

/* -------------------------------------------------------------------------- */
/* Extended Attributes */
/* -------------------------------------------------------------------------- */

/* ntea.c: code for manipulating NTEA information

   Copyright 1997, 1998, 2000, 2001 Red Hat, Inc.

   Written by Sergey S. Okhapkin (sos@prospect.com.ru)

This file is part of Cygwin.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

typedef struct _FILE_FULL_EA_INFORMATION {
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

/* -------------------------------------------------------------------------- */

static PFILE_FULL_EA_INFORMATION NTReadEARaw(HANDLE f, int *len)
{
    WIN32_STREAM_ID sid;
    DWORD w;
    LPVOID ctx = NULL;
    DWORD size;
    PFILE_FULL_EA_INFORMATION eafound = NULL;

    size = sizeof(WIN32_STREAM_ID) - sizeof(WCHAR **);

    /* read the WIN32_STREAM_ID in */
    while (BackupRead(f, (LPBYTE) & sid, size, & w, FALSE, FALSE, & ctx)) {
        DWORD sl, sh;

        /* no more stream ids */
        if (! w) break;

        /* skip StreamName */
        if (sid.dwStreamNameSize) {
            unsigned char *buf = NULL;

            if (! (buf = malloc(sid.dwStreamNameSize)) ) break;

            if (! BackupRead(f, buf, sid.dwStreamNameSize,
                                & w, FALSE, FALSE, & ctx)) {
                /* read error */
                free(buf); break;
            }

            free(buf);
        }

        /* EA stream */
        if (sid.dwStreamId == BACKUP_EA_DATA) {
            unsigned char *buf = NULL;

            if (! (buf = malloc(sid.Size.LowPart)) ) break;

            if (! BackupRead(f, buf, sid.Size.LowPart,
                                & w, FALSE, FALSE, & ctx)) {
                /* read error */
                free(buf); break;
            }

            eafound = (PFILE_FULL_EA_INFORMATION) buf;
            *len = sid.Size.LowPart;
            break;
        }

        /* skip current stream */
        if (! BackupSeek(f, sid.Size.LowPart, sid.Size.HighPart,
                            & sl, & sh, & ctx))
            break;
    }

    /* free context */
    BackupRead(f, NULL, 0, & w, TRUE, FALSE, & ctx);

    return eafound;
}

/* -------------------------------------------------------------------------- */

ASKL_API ssize_t file_getxattr(
    const char *file,
    const char *attrname,
    void *attrbuf,
    size_t len
)
{
    HANDLE f;
    int eafound = 0;
    PFILE_FULL_EA_INFORMATION ea, sea;
    int easize;
    SECURITY_ATTRIBUTES sec_attr;

    sec_attr.nLength = sizeof(sec_attr);
    sec_attr.bInheritHandle = FALSE;
    sec_attr.lpSecurityDescriptor = NULL;

    f = CreateFile(file, FILE_READ_EA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                   & sec_attr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (f == INVALID_HANDLE_VALUE) return -1;

    /* read in raw array of EAs */
    ea = sea = NTReadEARaw(f, & easize);

    /* search for requested attribute */
    while (sea) {

        if (! stricmp(ea->EaName, attrname)) {

            if (ea->EaValueLength > len) {
                /* buffer too small */
                eafound = -1; errno = ERANGE; break;
            }

            /* EA found, copy the data */
            memcpy(attrbuf, ea->EaName + (ea->EaNameLength + 1),
                   ea->EaValueLength);
            eafound = ea->EaValueLength;
            break;
        }

        if (! ea->NextEntryOffset || ((int) ea->NextEntryOffset > easize))
            break;

        ea = (PFILE_FULL_EA_INFORMATION) ((char *) ea + ea->NextEntryOffset);
    }

    free(sea);

    CloseHandle(f);

    return eafound;
}

/* -------------------------------------------------------------------------- */

ASKL_API int file_setxattr(
    const char *file,
    const char *attrname,
    const void *buf,
    size_t len,
    int flags
)
{
    HANDLE f;
    WIN32_STREAM_ID sid;
    DWORD w;
    LPVOID ctx = NULL;
    DWORD size, easize;
    int ret = -1;
    PFILE_FULL_EA_INFORMATION ea;
    SECURITY_ATTRIBUTES sec_attr;

    sec_attr.nLength = sizeof(sec_attr);
    sec_attr.bInheritHandle = FALSE;
    sec_attr.lpSecurityDescriptor = NULL;

    f = CreateFile(file, FILE_WRITE_EA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    & sec_attr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (f == INVALID_HANDLE_VALUE) return -1;

    size = sizeof(WIN32_STREAM_ID) - sizeof(WCHAR **);

    /* FILE_FULL_EA_INFORMATION structure is longword-aligned */
    easize = sizeof(*ea) - sizeof(WCHAR **) + strlen(attrname) + 1 + len
             + (sizeof(DWORD) - 1);
    easize &= ~(sizeof(DWORD) - 1);

    if (! (ea = malloc(easize)) ) goto _cleanup;

    memset(ea, 0, easize);
    ea->EaNameLength = strlen(attrname);
    ea->EaValueLength = len;
    strcpy(ea->EaName, attrname);
    memcpy(ea->EaName + (ea->EaNameLength + 1), buf, len);

    /* initialize the stream id */
    sid.dwStreamId = BACKUP_EA_DATA;
    sid.dwStreamAttributes = 0;
    sid.Size.HighPart = 0;
    sid.Size.LowPart = easize;
    sid.dwStreamNameSize = 0;

    if (! BackupWrite(f, (LPBYTE) & sid, size, & w, FALSE, FALSE, & ctx))
        goto _cleanup;

    if (! BackupWrite(f, (LPBYTE) ea, easize, & w, FALSE, FALSE, & ctx))
        goto _cleanup;

    ret = 0;

    /* free context */
_cleanup:
    BackupRead(f, NULL, 0, & w, TRUE, FALSE, & ctx);
    CloseHandle(f);
    free(ea);

    return ret;
}

/* -------------------------------------------------------------------------- */
#elif defined(__APPLE__)
/* -------------------------------------------------------------------------- */

ASKL_API ssize_t file_getxattr(
    const char *file,
    const char *attrname,
    void *attrbuf,
    size_t len
)
{
    #if defined(MAC_OS_X_VERSION_10_4) || defined(__MAC_10_4)
    /* XXX getxattr() first appeared in Mac OS X 10.4 */
    return getxattr(file, attrname, attrbuf, len, 0, 0);
    #else
    errno = ENOSYS;
    return -1;
    #endif
}

/* -------------------------------------------------------------------------- */

ASKL_API int file_setxattr(
    const char *file,
    const char *attrname,
    const void *buf,
    size_t len,
    int flags
)
{
    #if defined(MAC_OS_X_VERSION_10_4) || defined(__MAC_10_4)
    /* XXX setxattr() first appeared in Mac OS X 10.4 */
    return setxattr(file, attrname, buf, len, 0, flags);
    #else
    errno = ENOSYS;
    return -1;
    #endif
}

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */
