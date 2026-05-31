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

#include "askl_rwlock.h"
#include "arcane/bitops.c"

struct _RW_Lock {
    _ATOMIC int state;
    _ATOMIC int wflag;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

/**
 * @ingroup rwlock
 * @struct RW_Lock
 *
 * An internal read/write lock which provides:
 * - A fast uncontended path via atomic operations on @ref state.
 * - Blocking/wakeup for contended paths via @ref mutex and @ref cond.
 * - Support for upgrading a read lock to a write lock via @ref lock_upgrade()
 *   with a single "claimant" at a time and cooperative help from other readers.
 * - The ability to "break" the lock when destroying a map so that threads
 *   blocked in map operations wake up and fail cleanly instead of deadlocking
 *   on freed memory.
 *
 * The lock uses a single integer @ref state plus a mutex/condition pair.
 * The low bit of @ref state (0x1) is used as an "upgrade in progress" flag,
 * while the higher bits encode the base lock state and the number of readers.
 *
 * The following symbolic values are used:
 *
 * - @c -1            : broken; further calls to lock_rdlock(), lock_wrlock()
 *                      or lock_upgrade() will fail and return -1.
 * - @c WRLOCKED  (0) : write-locked; exactly one writer holds the lock.
 * - @c UPGRADED  (1) : upgraded; a single thread holds what was previously
 *                      a read lock but has transitioned to exclusive mode.
 * - @c UNLOCKED  (2) : no active readers or writers.
 * - @c RDLOCKED  (4) : base value for the "one reader, no claimant" state.
 *
 * For @ref state >= RDLOCKED the value is interpreted as:
 *
 * - Even (@c state & 0x1 == 0):
 *     the lock is held in read mode with no upgrade claim. The number of
 *     readers is (@c state - @c RDLOCKED) / @c LOCKSTEP + 1.
 *
 * - Odd (@c state & 0x1 == 1):
 *     an upgrade claim is in progress. One of the readers has set the
 *     claim flag and is attempting to become a writer.
 *
 * The special value @c CLAIMANT (5) denotes "exactly one reader remains and
 * it is the thread that has claimed the upgrade". At that point the upgrader
 * can atomically transition the lock from @c CLAIMANT to @c UPGRADED.
 *
 * On the fast path, readers and writers adjust @ref state atomically:
 *
 * - Readers increment @ref state by @c LOCKSTEP (2) as long as it is
 *   positive and even (no writer and no upgrade claim).
 * - Writers transition @ref state from @c UNLOCKED to @c WRLOCKED when no
 *   readers or claimers are present.
 * - Upgraders transition from @c RDLOCKED to @c UPGRADED when they are the
 *   only reader, or set the claim bit (by adding 1) when other readers are
 *   present and rely on them to cooperate.
 *
 * Cooperative readers that observe a claimed state with multiple readers
 * temporarily drop their read share (decrement @ref state by @c LOCKSTEP)
 * so that the upgrader can eventually become the sole remaining reader.
 *
 * On the slow path, @ref mutex and @ref cond are used to:
 *
 * - put readers and writers to sleep when they cannot adjust @ref state
 *   immediately, and
 * - wake them when the lock is released or broken.
 *
 * In builds without atomics, the same invariants are preserved, but all
 * updates to @ref state are performed under @ref mutex instead of using
 * atomic operations.
 */

#define WRLOCKED 0
#define UPGRADED 1
#define UNLOCKED 2
#define RDLOCKED 4
#define CLAIMANT 5

#define LOCKSTEP 2
#define RDWAITER 0x1000
#define WRWAITER 0x0001

#ifdef HAS_ATOMICS
#define _LOCKSTATE_GET(lk)       _atomic_ldr(& (lk)->state)
#define _LOCKSTATE_SET(lk, v)    _atomic_str(& (lk)->state, (v))
#define _LOCKSTATE_CAS(lk, a, b) _atomic_cas(& (lk)->state, (a), (b))
#define _LOCKSTATE_INC(lk)       _atomic_add(& (lk)->state, LOCKSTEP)
#define _LOCKSTATE_DEC(lk)       _atomic_sub(& (lk)->state, LOCKSTEP)
#define _LOCKWFLAG_GET(lk)       _atomic_ldr(& (lk)->wflag)
#define _LOCKWFLAG_SET(lk, v)    _atomic_str(& (lk)->wflag, (v))
#define _LOCKWFLAG_INC(lk, v)    _atomic_add(& (lk)->wflag, (v))
#define _LOCKWFLAG_DEC(lk, v)    _atomic_sub(& (lk)->wflag, (v))
#else
#define _LOCKSTATE_GET(lk)    ((lk)->state)
#define _LOCKSTATE_SET(lk, v) do { (lk)->state = (v); } while (0)
#define _LOCKSTATE_INC(lk)    do { (lk)->state += LOCKSTEP; } while (0)
#define _LOCKSTATE_DEC(lk)    do { (lk)->state -= LOCKSTEP; } while (0)
#define _LOCKWFLAG_GET(lk)    ((lk)->wflag)
#define _LOCKWFLAG_SET(lk, v) do { (lk)->wflag = (v); } while (0)
#define _LOCKWFLAG_INC(lk, v) do { (lk)->wflag += (v); } while (0)
#define _LOCKWFLAG_DEC(lk, v) do { (lk)->wflag -= (v); } while (0)
#endif

/* -------------------------------------------------------------------------- */

INTERNAL RW_Lock *lock_alloc(void)
{
    RW_Lock *ret = (RW_Lock *) malloc(sizeof(*ret));
    if (! ret) {
        perror(ERR(lock_alloc, malloc));
        return NULL;
    }
    return ret;
}

/* -------------------------------------------------------------------------- */

INTERNAL int lock_init(RW_Lock *lock)
{
    if (pthread_mutex_init(& lock->mutex, NULL)) {
        perror(ERR(lock_init, pthread_mutex_init));
        return -1;
    }

    if (pthread_cond_init(& lock->cond, NULL)) {
        perror(ERR(lock_init, pthread_cond_init));
        goto _err_cond;
    }

    _LOCKSTATE_SET(lock, UNLOCKED);
    _LOCKWFLAG_SET(lock, 0);

    return 0;

_err_cond:
    pthread_mutex_destroy(& lock->mutex);
    return -1;
}

/* -------------------------------------------------------------------------- */

INTERNAL int lock_rdlock(RW_Lock *lock)
{
    int ret = 0, x = 0;

    if (unlikely(_LOCKWFLAG_GET(lock) & 0xfff)) sched_yield();

    #ifdef HAS_ATOMICS
    for (x = _LOCKSTATE_GET(lock); x && ! (x & 0x1); x = _LOCKSTATE_GET(lock)) {
        if (_LOCKSTATE_CAS(lock, x, x + LOCKSTEP))
            return 0;
    }

    if (unlikely(x == -1)) return -1;
    #endif

    pthread_mutex_lock(& lock->mutex);

        _LOCKWFLAG_INC(lock, RDWAITER);

        #ifdef HAS_ATOMICS
        while (1) {
        #endif
            /* wait while write-locked (lockstate == 0) */
            while (! (x = _LOCKSTATE_GET(lock)) || x & 0x1) {
                if (unlikely(x == -1)) {
                    ret = -1; goto _err_lock;
                }
                pthread_cond_wait(& lock->cond, & lock->mutex);
            }

            #ifdef HAS_ATOMICS
            /* XXX handle slippery claimants */
            if (_LOCKSTATE_CAS(lock, x, x + LOCKSTEP)) break;
            #else
            _LOCKSTATE_INC(lock);
            #endif
        #ifdef HAS_ATOMICS
        }
        #endif

_err_lock:
        _LOCKWFLAG_DEC(lock, RDWAITER);

    pthread_mutex_unlock(& lock->mutex);

    return ret;
}

/* -------------------------------------------------------------------------- */

INTERNAL int lock_wrlock(RW_Lock *lock)
{
    int ret = 0, x = 0;

    #ifdef HAS_ATOMICS
    if (likely(_LOCKSTATE_CAS(lock, UNLOCKED, WRLOCKED))) return 0;
    #endif

    pthread_mutex_lock(& lock->mutex);

        _LOCKWFLAG_INC(lock, WRWAITER);

        #ifdef HAS_ATOMICS
        while (1) {
        #endif
            /* wait for unlock */
            while ( (x = _LOCKSTATE_GET(lock)) != UNLOCKED) {
                if (unlikely(x == -1)) {
                    ret = -1; goto _err_lock;
                }
                pthread_cond_wait(& lock->cond, & lock->mutex);
            }

            #ifdef HAS_ATOMICS
            /* XXX handle slippery readers */
            if (_LOCKSTATE_CAS(lock, UNLOCKED, WRLOCKED)) break;
            #else
            _LOCKSTATE_DEC(lock);
            #endif
        #ifdef HAS_ATOMICS
        }
        #endif

_err_lock:
        _LOCKWFLAG_DEC(lock, WRWAITER);

    pthread_mutex_unlock(& lock->mutex);

    return ret;
}

/* -------------------------------------------------------------------------- */

static int _cooperate(RW_Lock *lock)
{
    int cooperative = 0;
    int state = _LOCKSTATE_GET(lock);

    if ( (state & 0x1) && state > CLAIMANT) {
        /* there is multiple readers, help the claimant by releasing our lock */
        #ifdef HAS_ATOMICS
        if (! _LOCKSTATE_CAS(lock, state, state - LOCKSTEP)) return 0;
        #else
        _LOCKSTATE_DEC(lock);
        #endif
        pthread_cond_broadcast(& lock->cond);
        cooperative = 1;
    } else if (state == -1) return -1;

    #ifndef HAS_ATOMICS
    /* wait for the claim to be relinquished */
    while (! (state = _LOCKSTATE_GET(lock)) || (state & 0x1) ) {
        if (unlikely(state == -1)) return -1;
        pthread_cond_wait(& lock->cond, & lock->mutex);
    }
    #endif

    if (cooperative) {
        /* re-take our lock */
        #ifdef HAS_ATOMICS
        while (1) {
            state = _LOCKSTATE_GET(lock);
            /* XXX a writer may have slipped in when we were cooperating */
            if (state > WRLOCKED && ! (state & 0x1)) {
                if (_LOCKSTATE_CAS(lock, state, state + LOCKSTEP))
                    return 0;
            } else if (unlikely(state == -1)) return -1;
            sched_yield();
        }
        #else
        _LOCKSTATE_INC(lock);
        #endif
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

INTERNAL int lock_upgrade(RW_Lock *lock)
{
    #ifdef HAS_ATOMICS
    /* fast path: only reader */
    if (_LOCKSTATE_CAS(lock, RDLOCKED, UPGRADED)) return 0;
    
    /* claim the lock */
    while (1) {
        int state = _LOCKSTATE_GET(lock);

        if (! (state & 0x1)) {
            if (_LOCKSTATE_CAS(lock, state, state + 1)) {
                /* only I will remain */
                while (! _LOCKSTATE_CAS(lock, CLAIMANT, UPGRADED)) {
                    if (unlikely(_LOCKSTATE_GET(lock) == -1))
                        return -1;
                    sched_yield();
                }
                return 0;
            }
        } else if (state == -1) return -1;

        if (_cooperate(lock) == -1) return -1;
    }
    #else
    while (1) {
        int state;

        pthread_mutex_lock(& lock->mutex);

        if (! ((state = _LOCKSTATE_GET(lock)) & 0x1) ) {
            _LOCKSTATE_SET(lock, state + 1);

            /* wait for the other readers to go away */
            while ( (state = _LOCKSTATE_GET(lock)) != CLAIMANT) {
                if (unlikely(state == -1)) goto _failure;
                pthread_cond_wait(& lock->cond, & lock->mutex);
            }

            /* take the lock */
            _LOCKSTATE_SET(lock, 1);
            goto _success;
        } else if (state == -1) goto _failure;

        /* call cooperate while holding the state mutex */
        if (_cooperate(lock) == -1) goto _failure;

        pthread_mutex_unlock(& lock->mutex);
    }
_failure:
    pthread_mutex_unlock(& lock->mutex);
    return -1;
_success:
    pthread_mutex_unlock(& lock->mutex);
    return 0;
    #endif
}

/* -------------------------------------------------------------------------- */

INTERNAL void lock_restore(RW_Lock *lock)
{
    pthread_mutex_lock(& lock->mutex);

        _LOCKSTATE_SET(lock, RDLOCKED);

    pthread_mutex_unlock(& lock->mutex);

    pthread_cond_broadcast(& lock->cond);
}

/* -------------------------------------------------------------------------- */

INTERNAL void lock_break(RW_Lock *lock)
{
    pthread_mutex_lock(& lock->mutex);

        _LOCKSTATE_SET(lock, -1);

    pthread_mutex_unlock(& lock->mutex);

    pthread_cond_broadcast(& lock->cond);
}

/* -------------------------------------------------------------------------- */

INTERNAL void lock_unlock(RW_Lock *lock)
{
    int x;

    #ifdef HAS_ATOMICS
    while (1) {
        x = _LOCKSTATE_GET(lock);

        if (x == WRLOCKED) {
            if (likely(_LOCKSTATE_CAS(lock, x, UNLOCKED)))
                break;
        } else if (x > UNLOCKED) {
            if (_LOCKSTATE_CAS(lock, x, x - LOCKSTEP))
                break;
        } else return;
    }

    if (likely(! _LOCKWFLAG_GET(lock))) return;

    pthread_mutex_lock(& lock->mutex);
    pthread_mutex_unlock(& lock->mutex);

    pthread_cond_broadcast(& lock->cond);
    #else
    pthread_mutex_lock(& lock->mutex);

        if ( (x = _LOCKSTATE_GET(lock)) == 0)
            _LOCKSTATE_SET(lock, UNLOCKED);
        else if (x > UNLOCKED)
            _LOCKSTATE_DEC(lock);

    pthread_mutex_unlock(& lock->mutex);

    pthread_cond_broadcast(& lock->cond);
    #endif
}

/* -------------------------------------------------------------------------- */

INTERNAL void lock_destroy(RW_Lock *lock)
{
    pthread_cond_destroy(& lock->cond);
    pthread_mutex_destroy(& lock->mutex);
}

/* -------------------------------------------------------------------------- */

INTERNAL RW_Lock *lock_free(RW_Lock *lock)
{
    free(lock);
    return NULL;
}

/* -------------------------------------------------------------------------- */
