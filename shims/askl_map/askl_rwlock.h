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

#ifndef ASKL_RWLOCK_H

#define ASKL_RWLOCK_H

#include "askl.h"

/** @defgroup rwlock ASKL::rwlock */

typedef struct _RW_Lock RW_Lock;

/* -------------------------------------------------------------------------- */

INTERNAL RW_Lock *lock_alloc(void);

/**
 * @ingroup rwlock
 * @fn RW_Lock *lock_alloc(void)
 * @return a newly allocated lock on success, or NULL on allocation failure
 *
 * This private helper allocates an uninitialized read/write lock structure.
 *
 * The returned lock must be initialized with @ref lock_init() before it can
 * be used.
 *
 * @note This function only allocates memory; it does not initialize any
 *       synchronization primitives inside the lock.
 *
 * @see lock_init()
 * @see lock_free()
 */

/* -------------------------------------------------------------------------- */

INTERNAL int lock_init(RW_Lock *lock);

/**
 * @ingroup rwlock
 * @fn int lock_init(RW_Lock *lock)
 * @param lock pointer to a lock structure
 * @return 0 on success, -1 on error
 *
 * This function initializes a read/write lock structure.
 *
 * @see lock_alloc()
 * @see lock_destroy()
 */

/* -------------------------------------------------------------------------- */

INTERNAL int lock_rdlock(RW_Lock *lock);

/**
 * @ingroup rwlock
 * @fn int lock_rdlock(RW_Lock *lock)
 * @param lock the lock to acquire in read (shared) mode
 * @return 0 on success, -1 if the lock is broken or an error occurred
 *
 * This function acquires the lock in shared (read) mode. Multiple readers
 * may hold the lock concurrently as long as no writer owns it.
 *
 * If the lock is currently write-locked, the caller blocks until the write
 * lock is released or the lock is broken via @ref lock_break().
 *
 * On success, the caller holds a read lock and must eventually release it
 * with @ref lock_unlock().
 *
 * On failure (return value -1), the caller does not hold the lock. This
 * typically indicates that the lock has been broken.
 *
 * @warning The return value must always be checked. Treat a return value
 *          of -1 as "you do not own the lock".
 *
 * @see lock_wrlock()
 * @see lock_unlock()
 * @see lock_break()
 */

/* -------------------------------------------------------------------------- */

INTERNAL int lock_wrlock(RW_Lock *lock);

/**
 * @ingroup rwlock
 * @fn int lock_wrlock(RW_Lock *lock)
 * @param lock the lock to acquire in write (exclusive) mode
 * @return 0 on success, -1 if the lock is broken or an error occurred
 *
 * This function acquires the lock in exclusive (write) mode. When a writer
 * holds the lock, no other reader or writer may hold it at the same time.
 *
 * If the lock is currently held by readers or another writer, the caller
 * blocks until the lock becomes available or is broken via
 * @ref lock_break().
 *
 * On success, the caller holds the write lock and must eventually release
 * it with @ref lock_unlock().
 *
 * On failure (return value -1), the caller does not hold the lock and
 * should treat the lock as broken.
 *
 * @see lock_rdlock()
 * @see lock_unlock()
 * @see lock_break()
 */

/* -------------------------------------------------------------------------- */

INTERNAL int lock_upgrade(RW_Lock *lock);

/**
 * @ingroup rwlock
 * @fn int lock_upgrade(RW_Lock *lock)
 * @param lock the lock to upgrade from read to write mode
 * @return 0 on success, -1 if the lock is broken or the upgrade fails
 *
 * This function upgrades a lock that the caller already holds in read mode
 * to an exclusive write lock, without allowing an intervening writer to
 * slip in.
 *
 * Only one upgrader at a time may claim the lock. If multiple readers attempt
 * to upgrade concurrently, unsuccessful claimants temporarily cooperate by
 * dropping and later re-acquiring their read share so the current upgrader
 * can become the sole owner and transition the lock to write mode.
 *
 * On success, the caller no longer holds a read lock; it now owns the lock in
 * a special write mode and must eventually release it with @ref lock_restore().
 *
 * On failure (return value -1), the caller must assume that it no longer
 * owns a valid lock, neither for reading nor for writing. This usually
 * indicates that the lock has been broken via @ref lock_break().
 *
 * @warning This function must only be called by a thread that already
 *          holds the lock in read mode. Calling it without a read lock
 *          is undefined behavior.
 * 
 * @warning An upgraded lock must only be released with @ref lock_restore(),
 *          using @ref lock_unlock() instead will result in a corrupted lock
 *          state and undefined behavior.
 *
 * @see lock_rdlock()
 * @see lock_wrlock()
 * @see lock_restore()
 * @see lock_break()
 */

/* -------------------------------------------------------------------------- */

INTERNAL void lock_restore(RW_Lock *lock);

/**
 * @ingroup rwlock
 * @fn void lock_restore(RW_Lock *lock)
 * @param lock the lock to restore to read mode
 * @return void
 *
 * This function restores a lock that was previously upgraded to write mode
 * via @ref lock_upgrade() back to a regular read-lock owned by the
 * upgrading thread, and releases the upgrade claim so that other upgraders
 * may proceed.
 *
 * After @ref lock_restore() returns, the caller holds the lock in read
 * mode and must still eventually release it with @ref lock_unlock().
 *
 * @warning This function must only be called after a successful
 *          @ref lock_upgrade(). Calling it on a lock that was not
 *          upgraded by the calling thread results in undefined
 *          behavior.
 *
 * @see lock_upgrade()
 */

/* -------------------------------------------------------------------------- */

INTERNAL void lock_break(RW_Lock *lock);

/**
 * @ingroup rwlock
 * @fn void lock_break(ASKL_RWLock *lock)
 * @param lock the lock to break
 * @return void
 *
 * This function marks the lock as "broken" and wakes up all threads that
 * are currently waiting on it.
 *
 * Once a lock is broken, all subsequent calls to @ref lock_rdlock(),
 * @ref lock_wrlock() or @ref lock_upgrade() return -1, and waiting
 * operations will abort rather than blocking indefinitely.
 *
 * This is typically used just before destroying the lock, so that all
 * threads currently blocked on the lock can detect the shutdown and exit
 * their critical sections cleanly.
 *
 * @note Calling @ref lock_unlock() on a broken lock is safe and becomes
 *       a no-op.
 *
 * @warning After a lock has been broken, it must not be used again except
 *          to allow pending operations to detect the broken state and
 *          exit gracefully.
 */

/* -------------------------------------------------------------------------- */

INTERNAL void lock_unlock(RW_Lock *lock);

/**
 * @ingroup rwlock
 * @fn void lock_unlock(RW_Lock *lock)
 * @param lock the lock to release
 * @return void
 *
 * This function releases a lock held either in read or write mode by the
 * calling thread.
 *
 * For a read lock, it decrements the internal reader count; for a write
 * lock, it transitions the state back to "unlocked". Waiting readers and
 * writers are notified via the internal condition variable.
 *
 * Calling this function on a broken lock is safe and effectively a no-op.
 *
 * @warning The caller must only call this after successfully acquiring
 *          the lock (via @ref lock_rdlock() or @ref lock_wrlock()).
 *
 * @see lock_rdlock()
 * @see lock_wrlock()
 */

/* -------------------------------------------------------------------------- */

INTERNAL void lock_destroy(RW_Lock *lock);

/**
 * @ingroup rwlock
 * @fn void lock_destroy(ASKL_RWLock *lock)
 * @param lock the lock whose resources should be released
 * @return void
 *
 * This function destroys the internal mutexes and condition variables
 * associated with a lock. It does not free the memory of the lock itself.
 *
 * The caller is responsible for ensuring that no thread is currently
 * blocked on or holding the lock when this function is called.
 *
 * @warning Destroying a lock that is still in use by other threads leads
 *          to undefined behavior.
 *
 * @see lock_break()
 * @see lock_free()
 */

/* -------------------------------------------------------------------------- */

INTERNAL RW_Lock *lock_free(RW_Lock *lock);

/**
 * @ingroup rwlock
 * @fn RW_Lock *lock_free(RW_Lock *lock)
 * @param lock the lock structure to free
 * @return always NULL
 *
 * This helper frees the memory associated with a lock structure previously
 * allocated with @ref lock_alloc(). It always returns NULL, which allows
 * idioms such as:
 *
 * @code
 *     mylock = lock_free(mylock);
 * @endcode
 *
 * @warning The lock must have been destroyed with @ref lock_destroy()
 *          (or otherwise guaranteed to be unused) before calling this
 *          function. Freeing a lock that is still in use results in
 *          undefined behavior.
 *
 * @see lock_alloc()
 * @see lock_destroy()
 */

/* -------------------------------------------------------------------------- */

#endif
