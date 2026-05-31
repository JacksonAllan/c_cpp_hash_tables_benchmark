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

#include "askl_time_compat.h"

/* -------------------------------------------------------------------------- */
#ifdef WIN32
/* -------------------------------------------------------------------------- */

static LARGE_INTEGER freq;

ASKL_API void monotonic_timer_init(void)
{
    QueryPerformanceFrequency(& freq);
}

/* -------------------------------------------------------------------------- */

ASKL_API int monotonic_timer(struct timespec *ts)
{
    LARGE_INTEGER t;

    if (! ts) return -1;

    QueryPerformanceCounter(& t);

    ts->tv_sec = t.QuadPart / freq.QuadPart;
    ts->tv_nsec = (1000000000 * (t.QuadPart % freq.QuadPart)) / freq.QuadPart;

    return 0;
}

/* -------------------------------------------------------------------------- */

/* This code was released to public domain by Wu Yongwei. */

ASKL_API int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME ft;
    LARGE_INTEGER li;
    uint64_t t;
    static int tzflag;

    if (tv) {
        GetSystemTimeAsFileTime(& ft);
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        t  = li.QuadPart;                   /* In 100-nanosecond intervals */
        t -= EPOCHFILETIME;                 /* Offset to the Epoch time */
        t /= 10;                            /* In microseconds */
        tv->tv_sec  = (long) (t / 1000000);
        tv->tv_usec = (long) (t % 1000000);
    }

    if (tz) {
        if (! tzflag) {
            _tzset(); tzflag ++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
#elif (defined(__APPLE__))
/* -------------------------------------------------------------------------- */

static mach_timebase_info_data_t tb;

ASKL_API void monotonic_timer_init(void)
{
    mach_timebase_info(& tb);
}

/* -------------------------------------------------------------------------- */

ASKL_API int monotonic_timer(struct timespec *ts)
{
    uint64_t ns = 0;

    if (! ts) return -1;

    ns = (mach_absolute_time() * (uint64_t) tb.numer) / (uint64_t) tb.denom;
    ts->tv_sec = ns / 1000000000;
    ts->tv_nsec = ns - (ts->tv_sec * 1000000000);

    return 0;
}

/* -------------------------------------------------------------------------- */
#else
/* -------------------------------------------------------------------------- */

ASKL_API void monotonic_timer_init(void)
{
    ;
}

/* -------------------------------------------------------------------------- */

ASKL_API int monotonic_timer(struct timespec *ts)
{
    if (! ts) {
        debug("monotonic_timer(): bad parameters.\n");
        return -1;
    }

    if (clock_gettime(CLOCK_MONOTONIC, ts) == -1) {
        perror(ERR(monotonic_timer, clock_gettime));
        return -1;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */
