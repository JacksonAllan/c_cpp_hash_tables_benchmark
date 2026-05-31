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

#ifndef ASKL_MAP_H

#define ASKL_MAP_H

#ifdef _ENABLE_HASHMAP

#include "askl.h"
#include "askl_rwlock.h"
#include "askl_variant.h"

#define MAP_ASC                 0
#define MAP_DESC                1
#define MAP_SORT_ONCE           2

/** @defgroup map ASKL::map */

typedef struct _Map Map;

/**
 * @ingroup map
 * @struct Map
 *
 * Opaque handle to a concurrent linked hash map.
 *
 * A Map is a hash-indexed associative container mapping arbitrary byte-string
 * keys to @ref variant values. In addition to O(1) expected-time lookups, it
 * maintains a stable internal index so that entries can be visited in a
 * well-defined order (e.g. LIFO insertion order or user-specified sort order).
 *
 * The map is safe for concurrent access: readers and writers are synchronized
 * internally using a read–write lock. Simple operations such as @ref map_get(),
 * @ref map_set() or @ref map_remove() may be used directly from multiple
 * threads without additional external locking.
 *
 * Instances of this type are created with @ref map_alloc() and must be
 * destroyed with @ref map_free() when no longer needed. All interaction with
 * the map should go through the functions declared in this header; the
 * structure layout is intentionally hidden and may change between releases.
 */

typedef struct Map_Iterator {
    Map *map;
    struct _Bucket *_current;
    const char *key;
    size_t len;
    Variant val;
} Map_Iterator;

/**
 * @ingroup map
 * @struct Map_Iterator
 *
 * This structure represents an iterator over the entries of a @ref Map.
 *
 * Iterators are created by @ref map_each() or @ref map_at().
 * They carry a reference to the underlying map and expose the current
 * key/value pair through their public fields.
 *
 * The iterator maintains a read lock on @ref map for the duration of its
 * lifetime. The lock is acquired when the iterator is created and is released
 * when the iterator is exhausted (via @ref map_next()) or explicitly
 * destroyed with @ref map_break().
 *
 * @b public @ref map      points to the map being traversed.
 * @b public @ref key      points to the current key bytes (NUL-terminated).
 * @b public @ref len      is the length of the current key in bytes
 *                         (excluding the terminating NUL).
 * @b public @ref val      is the current value associated with @ref key.
 *
 * @b private @ref _current is the internal cursor used to walk the map’s
 *                          index list. It must not be accessed directly by
 *                          user code.
 *
 * Iteration is performed by repeatedly calling @ref map_next() until it
 * returns NULL. The current entry may be updated or removed in-place using
 * @ref map_set_at() and @ref map_remove_at(), which perform the necessary
 * lock upgrades internally.
 *
 * @note The iterator itself is heap-allocated and is freed automatically when
 *       @ref map_next() reaches the end of the traversal, or manually by
 *       calling @ref map_break().
 */

typedef int (*Map_Comparator)(
    const char *, size_t, Variant,
    const char *, size_t, Variant
);

/**
 * @ingroup map
 * @typedef Map_Comparator
 *
 * Comparator used by map_sort().
 *
 * The function receives two key/value pairs and must return a negative value
 * if the first pair should come before the second, zero if they compare equal,
 * or a positive value if the first pair should come after the second.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Map *map_alloc(void (*freeval)(Variant));

/**
 * @ingroup map
 * @fn Map *map_alloc(void (*freeval)(Variant))
 * @param freeval optional callback used to destroy stored values
 * @return a pointer to a newly allocated @ref Map, or NULL on error
 *
 * This function allocates and initializes a new concurrent linked hash map.
 *
 * If @p freeval is not NULL, it will be called once for each remaining value
 * stored in the map when @ref map_free() is called, or when entries are
 * removed via @ref map_foreach().
 *
 * The returned hashmap must be destroyed with @ref map_free() when no longer
 * needed.
 */

/* -------------------------------------------------------------------------- */

ASKL_API void map_reserve(Map *h, size_t count);

/**
 * @ingroup map
 * @fn void map_reserve(Map *h, size_t count)
 * @param h     a pointer to a linked hashmap
 * @param count the number of entries to reserve space for
 * @return void
 *
 * This function reserves enough hash-index capacity for at least @p count
 * entries.
 *
 * Reserving capacity before inserting many entries can reduce the number of
 * internal resizes and rehashes performed while the map grows. It does not
 * insert any entries, change existing key/value pairs, or alter the traversal
 * order of the map.
 *
 * If @p count is less than or equal to the current effective capacity, this
 * function succeeds without modifying the map.
 *
 * @note This function acquires a write lock on the hashmap while resizing.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_set_with(
    Map *h,
    const char *k,
    size_t l,
    Variant v,
    Variant (*function)(const char *k, size_t l, Variant old, Variant nevv)
);

/**
 * @ingroup map
 * @fn Variant map_set_with(Map *h, const char *key, size_t len,
 *                          Variant value,
 *                          Variant (*function)(const char *, size_t,
 *                                              Variant, Variant))
 * @param h        a pointer to a linked hashmap
 * @param key      pointer to the key bytes
 * @param len      length of the key in bytes
 * @param value    the new value (also available to @p function as @p new)
 * @param function optional callback used to resolve replacement
 * @return the previous value associated with @p key, or VARIANT_NULL if the
 *         key was newly inserted or the value is aliased.
 *
 * This function sets @p key to @p value, inserting a new entry if needed.
 *
 * If @p function is NULL, the stored value is replaced unconditionally and
 * the previous value is returned. If there was no previous value, i.e. the
 * key was created, or if the new value is identical to the previous value
 * (i.e. aliased), the function returns VARIANT_NULL.
 *
 * If @p function is non-NULL and the key already exists, it is invoked with
 * the current value (@p old) and the requested value (@p new).
 *
 * If @p function returns:
 * - @p old: the map is left unchanged and map_set_with returns @p new.
 * - @p new: the @p new value is stored in the map and map_set_with returns
 *           the previous value (@p old).
 * - any other value: the previous value (@p old) is discarded (and the
 *                    _freeval callback invoked if defined), the returned value
 *                    is stored in the map, and map_set_with returns @p new
 *                    for the caller to dispose of.
 *
 * @note If the new value is identical to the previous value, the function will
 *       return VARIANT_NULL to prevent unsafe access to an object still owned
 *       by the map, or its accidental destruction.
 *
 * @note The callback @p function is executed while the map's write lock is
 *       held, ensuring atomicity. The callback must be fast and non-blocking.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_set(Map *h, const char *k, size_t l, Variant v);

/**
 * @ingroup map
 * @fn Variant map_set(Map *h, const char *k, size_t l, Variant v)
 * @param h a pointer to a linked hashmap
 * @param k pointer to the key bytes (not necessarily NUL-terminated)
 * @param l length of the key in bytes
 * @param v the value to store
 * @return the previous value associated with @p k, or VARIANT_NULL if none
 *
 * This function stores the value @p v under key @p k into the map.
 * If an entry with the same key already exists, its value is replaced and the
 * previous value is returned. Otherwise, the value is inserted and
 * VARIANT_NULL is returned.
 *
 * @note If the new value is identical to the previous value, the function will
 *       return VARIANT_NULL to prevent unsafe access to an object still owned
 *       by the map, or its accidental destruction.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_insert_with(
    Map *h,
    const char *key,
    size_t len,
    Variant value,
    Variant (*function)(const char *k, size_t l, Variant nevv)
);

/**
 * @ingroup map
 * @fn Variant map_insert_with(Map *h, const char *key, size_t len,
 *                             Variant value,
 *                             Variant (*function)(const char *, size_t,
 *                                                 Variant))
 * @param h        a pointer to a linked hashmap
 * @param key      pointer to the key bytes
 * @param len      length of the key in bytes
 * @param value    value passed to @p function as @p new
 * @param function optional callback to compute or initialize the inserted value
 * @return VARIANT_NULL if the value was inserted, or @p value if
 *         the key already existed or the insertion failed.
 *
 * This function performs an insert-only operation with an optional callback.
 *
 * If an entry with @p key already exists, the map is left unchanged,
 * @p function is not called, and @p value is returned to the caller.
 *
 * If the key does not exist, a new entry is inserted. If @p function is NULL,
 * @p value is stored directly. If @p function is non-NULL, it is invoked with
 * @p value as parameter and its return value is stored instead.
 *
 * @note The callback @p function is executed only when the key is newly
 *       inserted. It is executed while the map's write lock is held, ensuring
 *       atomicity. The callback must be fast and non-blocking.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_insert(Map *h, const char *k, size_t l, Variant v);

/**
 * @ingroup map
 * @fn Variant map_insert(Map *h, const char *k, size_t l, Variant v)
 * @param h a pointer to a linked hashmap
 * @param k pointer to the key bytes (not necessarily NUL-terminated)
 * @param l length of the key in bytes
 * @param v the value to store
 * @return VARIANT_NULL if the value was inserted, or @p v if
 *         the key already existed or the insertion failed.
 *
 * This function performs an insert-only operation. If no entry with the given
 * key exists, the key/value pair is inserted and VARIANT_NULL is returned.
 * If an entry already exists, the map is left unchanged and @p v is returned.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_update_with(
    Map *h,
    const char *k,
    size_t l,
    Variant v,
    Variant (*function)(const char *k, size_t l, Variant old, Variant nevv)
);

/**
 * @ingroup map
 * @fn Variant map_update_with(Map *h, const char *key, size_t len,
 *                             Variant value,
 *                             Variant (*function)(const char *key, size_t len,
 *                                                 Variant old, Variant new))
 * @param h        a pointer to a linked hashmap
 * @param key      pointer to the key bytes (not necessarily NUL-terminated)
 * @param len      length of the key in bytes
 * @param value    the proposed new value (also passed to @p function as @p new)
 * @param function optional callback used to compute the replacement value
 * @return the previous value associated with @p key if it existed, or
 *         @p value if the key was not present and no update was performed
 *
 * This function performs an update-only operation with an optional callback.
 * It never inserts new keys into the map.
 *
 * If the key @p key does not exist in @p h, the map is left unchanged and
 * @p value is returned to be disposed of.
 *
 * If the key exists and @p function is NULL, the stored value is replaced
 * unconditionally with @p value and the previous value is returned.
 *
 * If the key exists and @p function is non-NULL, the callback is invoked with
 * the current value (@p old) and the proposed value (@p new). Its return
 * value determines what is stored in the map and what is returned:
 *
 * - If @p function returns @p old: the map is left unchanged and
 *   map_update_with() returns @p new.
 * - If @p function returns @p new: the returned value is stored in the map
 *   and map_update_with() returns @p old.
 * - If @p function returns any other value: the previous value (@p old) is
 *   discarded (and the map's @c _freeval callback is invoked if defined),
 *   the returned value is stored in the map, and map_update_with() returns
 *   @p new so that the caller may dispose of it if necessary.
 *
 * @note If the new value is identical to the previous value, the function will
 *       return VARIANT_NULL to prevent unsafe access to an object still owned
 *       by the map, or its accidental destruction.
 *
 * @note The callback @p function is executed only when the key already exists.
 *       It is executed while the map's write lock is held, ensuring atomicity.
 *       The callback must be fast and non-blocking.
 *
 * @see map_set_with()
 * @see map_insert_with()
 * @see map_update()
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_update(Map *h, const char *k, size_t l, Variant v);

/**
 * @ingroup map
 * @fn Variant map_update(Map *h, const char *key, size_t len, Variant value)
 * @param h     a pointer to a linked hashmap
 * @param key   pointer to the key bytes (not necessarily NUL-terminated)
 * @param len   length of the key in bytes
 * @param value the new value to store
 * @return the previous value associated with @p key if it existed, or
 *         @p value if the key was not present and no update was performed
 *
 * This function performs a simple update-only operation. If an entry with
 * @p key exists, its value is replaced with @p value and the previous value
 * is returned.
 *
 * If the key does not exist in the map, the map is left unchanged and
 * @p value is returned to be disposed of. No new entry is created.
 *
 * This is the update-only counterpart to @ref map_insert(), and is useful
 * when the caller wants to modify an entry only if it already exists, and
 * do nothing otherwise.
 *
 * @note If the new value is identical to the previous value, the function will
 *       return VARIANT_NULL to prevent unsafe access to an object still owned
 *       by the map, or its accidental destruction.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_get_with(
    Map *h,
    const char *key,
    size_t len,
    Variant (*function)(Variant)
);

/**
 * @ingroup map
 * @fn Variant map_get_with(Map *h, const char *key, size_t len,
 *                          Variant (*function)(Variant))
 * @param h        a pointer to a linked hashmap
 * @param key      pointer to the key bytes
 * @param len      length of the key in bytes
 * @param function optional callback applied to the stored value
 * @return the stored value, the result of @p function, or VARIANT_NULL
 *
 * This function looks up the entry associated with @p key in @p h.
 *
 * If the key is found and @p function is NULL, the stored value is returned.
 * If @p function is non-NULL, it is called with the stored value as argument
 * and its return value is returned instead.
 *
 * If the key is not present in the map, VARIANT_NULL is returned.
 *
 * @note The callback @p function is executed while the map's read lock is
 *       held, ensuring atomicity. This is useful for acquiring locks on
 *       stored objects, or other operations that must be atomic with the
 *       lookup. The callback must be fast and non-blocking.
 *
 * @warning The callback @p function must not free the stored value or
 *          otherwise invalidate it.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_get(Map *h, const char *key, size_t len);

/**
 * @ingroup map
 * @fn Variant map_get(Map *h, const char *key, size_t len)
 * @param h   a pointer to a linked hashmap
 * @param key pointer to the key bytes
 * @param len length of the key in bytes
 * @return the stored value, or VARIANT_NULL if the key is not present
 *
 * This is a convenience wrapper around @ref map_get_with() with a NULL
 * callback. It simply returns the value associated with @p key, or
 * VARIANT_NULL if the key is not in the map.
 */

/* -------------------------------------------------------------------------- */

ASKL_API int map_has(Map *h, const char *key, size_t len);

/**
 * @ingroup map
 * @fn int map_has(Map *h, const char *key, size_t len)
 * @param h   a pointer to a linked hashmap
 * @param key pointer to the key bytes
 * @param len length of the key in bytes
 * @return non-zero if @p key exists in the map, or 0 otherwise
 *
 * This function checks whether an entry with the given key exists in the map.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int map_merge(
    Map *dest,
    Map *src,
    Variant merge(const char *key, size_t len, Variant destval, Variant srcval)
);

/**
 * @ingroup map
 * @fn int map_merge(Map *dest, Map *src,
 *                   Variant (*merge)(const char *, size_t, Variant, Variant))
 * @param dest  destination hashmap
 * @param src   source hashmap (consumed and destroyed)
 * @param merge conflict resolution callback
 * @return 0 on success, or -1 on error
 *
 * The function transfers all entries from @p src into @p dest and resolves
 * conflicts using the user-supplied @p merge callback.
 *
 * When a key exists in both maps, the callback is invoked with the key and
 * the values from @p dest and @p src. The callback's return value replaces
 * the value stored in @p dest for that key.
 *
 * If the callback returns the original value from @p dest, the value from
 * @p src is discarded. If it returns the original value from @p src, the
 * value from @p dest is discarded. If it returns a different value, both
 * original values are discarded.
 *
 * Discarded values are released using the owning map's @p _freeval callback,
 * if defined. If no @p _freeval callback is configured for the map owning a
 * discarded value, the @p merge callback must release that value itself to
 * avoid leaks.
 *
 * After a successful call, @p src is destroyed. The @p src pointer becomes
 * invalid and must not be accessed again.
 *
 * @note The @p merge callback is executed while both maps are write-locked.
 *       It must be fast and must not attempt to access either map or perform
 *       blocking operations.
 *
 * @warning This function consumes and destroys @p src.
 *          The caller must ensure that no other threads access @p src
 *          concurrently with this call, or after it returns.
 */

/* -------------------------------------------------------------------------- */

ASKL_API void map_foreach(
    Map *h,
    int (*function)(const char *, size_t, Variant, void *),
    void *context
);

/**
 * @ingroup map
 * @fn void map_foreach(Map *h,
 *                      int (*function)(const char *, size_t, Variant, void *),
 *                      void *context)
 * @param h        a pointer to a linked hashmap
 * @param function a callback invoked once per key/value pair
 * @param context  optional user data passed to the callback
 * @return void
 *
 * This function iterates over all entries in the hashmap and calls @p function
 * for each key/value pair. The callback receives:
 *  - the key pointer (NUL-terminated),
 *  - the key length in bytes,
 *  - the associated value,
 *  - the user provided context.
 *
 * If @p function returns -1 for an entry, that entry is removed from the
 * map. If the hashmap was created with a @p freeval callback, it is invoked on
 * the value before the entry is destroyed.
 * 
 * If @p function returns 1 for an entry, the traversal stops.
 *
 * Any other return value from @p function is ignored and the iteration
 * continues.
 *
 * @note This function acquires a write lock on the hashmap for the entire
 *       duration of the traversal.
 */

/* -------------------------------------------------------------------------- */

ASKL_API int map_sort(Map *h, unsigned int order, Map_Comparator cmp);

/**
 * @ingroup map
 * @fn int map_sort(Map *h, unsigned int order, Map_Comparator cmp)
 * @param h     a pointer to a map
 * @param order sort order: @ref MAP_ASC or @ref MAP_DESC, optionally OR'ed
 *              with @ref MAP_SORT_ONCE
 * @param cmp   comparison callback
 * @return 0 on success, -1 on error
 *
 * This function sorts the internal traversal index of @p h using the
 * user-provided comparator @p cmp and a stable natural merge sort.
 *
 * The comparator receives:
 *  - @p key0, @p key1: pointers to key bytes;
 *  - @p len0, @p len1: length of the keys in bytes;
 *  - @p value0, @p value1: associated values.
 *
 * @warning Keys are followed by a trailing NUL byte for convenience, but may
 * contain embedded NUL bytes. Comparators should use @p len0 and @p len1
 * rather than treating keys as C strings.
 *
 * The comparator must return:
 *  - a negative value if (key0, value0) should come before (key1, value1),
 *  - zero if they are considered equal for ordering purposes,
 *  - a positive value if (key0, value0) should come after (key1, value1).
 *
 * The @p order argument controls whether the resulting order is ascending
 * (@ref MAP_ASC) or descending (@ref MAP_DESC) with respect to @p cmp.
 *
 * By default, sorting is persistent. After a successful call without
 * @ref MAP_SORT_ONCE, @p cmp and @p order become the map's active traversal
 * ordering policy. Later insertions or updates mark the index stale;
 * @ref map_foreach(), @ref map_each(), and @ref map_at() will lazily re-sort
 * the index before traversal when needed.
 *
 * If @ref MAP_SORT_ONCE is OR'ed into @p order, the index is sorted
 * immediately using @p cmp, but @p cmp and @p order are not installed as the
 * persistent ordering policy. If a persistent ordering policy is already
 * active, it is left unchanged and will be used again after a later mutation
 * marks the index stale.
 *
 * Example:
 * @code
 * map_sort(map, MAP_DESC | MAP_SORT_ONCE, map_sort_keys);
 * @endcode
 *
 * @note Sorting only affects traversal order, including @ref map_foreach()
 *       and iterators. It does not change lookup, insertion, update, or
 *       removal semantics.
 */

/* -------------------------------------------------------------------------- */

ASKL_API int map_sort_keys(
    const char *key0,
    size_t len0,
    UNUSED Variant val0,
    const char *key1,
    size_t len1,
    UNUSED Variant val1
);

/**
 * @ingroup map
 * @fn int map_sort_keys(const char *key0, size_t len0, Variant val0,
 *                       const char *key1, size_t len1, Variant val1)
 * @param key0 first key
 * @param len0 first key length
 * @param val0 unused
 * @param key1 second key
 * @param len1 second key length
 * @param val1 unused
 * @return an integer less than, equal to, or greater than zero
 *
 * This is a convenience comparator suitable for use with @ref map_sort(),
 * it compares @p key0 and @p key1 lexicographically and ignores the values.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_remove_if(
    Map *h,
    const char *key,
    size_t len,
    int (*condition)(const char *key, size_t len, Variant val)
);

/**
 * @ingroup map
 * @fn Variant map_remove_if(Map *h, const char *key, size_t len,
 *                           int (*condition)(const char *, size_t, Variant))
 * @param h         a pointer to a linked hashmap
 * @param key       pointer to the key bytes
 * @param len       length of the key in bytes
 * @param condition optional predicate controlling removal
 * @return the removed value if the entry was removed, or VARIANT_NULL if the
 *         key was not present or was not removed
 *
 * This function removes the entry associated with @p key from @p h.
 *
 * If @p condition is NULL, the entry is removed unconditionally.
 *
 * If @p condition is non-NULL, it is invoked with the stored value. The entry
 * is removed only if the callback returns non-zero.
 *
 * @note The callback @p condition is executed while the map's write lock is
 *       held, ensuring atomicity. The callback must be fast and non-blocking.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_remove(Map *h, const char *key, size_t len);

/**
 * @ingroup map
 * @fn Variant map_remove(Map *h, const char *key, size_t len)
 * @param h   a pointer to a linked hashmap
 * @param key pointer to the key bytes
 * @param len length of the key in bytes
 * @return the removed value, or VARIANT_NULL if the key was not present
 *
 * This function removes the entry associated with @p key from @p h and
 * returns its value. If the key does not exist, VARIANT_NULL is returned.
 */

/* -------------------------------------------------------------------------- */

ASKL_API size_t map_footprint(Map *h, size_t *overhead);

/**
 * @ingroup map
 * @fn size_t map_footprint(Map *h, size_t *overhead)
 * @param h        a pointer to a linked hashmap
 * @param overhead optional pointer to receive the internal overhead, in bytes
 * @return the total memory footprint of the hashmap, in bytes
 *
 * This function computes an approximate memory footprint of the hashmap,
 * including:
 *  - the structure itself,
 *  - its dynamically allocated lock,
 *  - the bucket array,
 *  - all allocated items and their keys.
 *
 * If @p overhead is non-NULL, @c *overhead is set to the portion of @p h
 * memory that is considered overhead (metadata, buckets, etc.) rather than
 * user payload (keys and the pointer to the value).
 */

/* -------------------------------------------------------------------------- */

ASKL_API Map *map_free(Map *h);

/**
 * @ingroup map
 * @fn Map *map_free(Map *h)
 * @param h a pointer to a linked hashmap
 * @return always NULL
 *
 * This function destroys the hashmap @p h and frees all associated resources.
 * If a @p freeval callback was specified at creation time, it is called once
 * for each remaining stored value before the corresponding entry is freed.
 *
 * This function always returns NULL so it can be used to clear the pointer:
 * @code
 * map = map_free(map);
 * @endcode
 */

/* -------------------------------------------------------------------------- */

ASKL_API Map_Iterator *map_each(Map *h);

/**
 * @ingroup map
 * @fn Map_Iterator *map_each(Map *h)
 * @param h   a pointer to a linked hashmap
 * @return a newly allocated iterator positioned on the first entry, or @c NULL
 *         if the map is empty or an error occurred
 *
 * This function creates an iterator that allows the caller to traverse all
 * entries of the hashmap in the current traversal order.
 *
 * The returned iterator holds a read lock on @p h for the duration of the
 * iteration. The iterator must be advanced using @ref map_next and eventually
 * destroyed using @ref map_break (or implicitly when @ref map_next reaches
 * the end).
 *
 * @note The iterator acquires a read lock on @p h when created. This lock
 *       is automatically released when the iterator is exhausted or explicitly
 *       destroyed with @ref map_break.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Map_Iterator *map_at(Map *h, const char *key, size_t len);

/**
 * @ingroup map
 * @fn Map_Iterator *map_at(Map *h, const char *key, size_t len)
 * @param h    a pointer to a linked hashmap
 * @param key  a pointer to the key to look up
 * @param len  the length in bytes of @p key
 * @return a newly allocated iterator positioned on the entry matching @p key,
 *         or @c NULL if the key is not found or an error occurred
 *
 * This function creates an iterator positioned on the entry associated with
 * the specified @p key in the hashmap. It performs a lookup in @p h and, if
 * the key exists, returns an iterator whose @c val, @c key and @c len fields
 * are initialized to the corresponding entry.
 *
 * The returned iterator holds a read lock on @p h for the duration of its
 * lifetime. As with iterators created by @ref map_each, the iterator must be
 * advanced using @ref map_next and eventually destroyed using @ref map_break
 * (or implicitly when @ref map_next reaches the end).
 *
 * @note If @p key is not present in the map or an internal error occurs,
 *       this function returns @c NULL and does not leave a lock held on
 *       @p h.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Map_Iterator *map_next(Map_Iterator *iterator);

/**
 * @ingroup map
 * @fn Map_Iterator *map_next(Map_Iterator *iterator)
 * @param iterator  an iterator created with @ref map_each or @ref map_at
 * @return the same iterator positioned on the next entry, or @c NULL if the
 *         end of the traversal is reached or an error occurred
 *
 * This function advances the iterator to the next entry in the map. If another
 * entry is found, the iterator's @c key, @c len and @c val fields are updated
 * accordingly and @p iterator is returned.
 *
 * When there are no more entries, the iterator is automatically destroyed,
 * its read lock on the map is released, and @c NULL is returned.
 *
 * @note The caller must not free the iterator returned by @ref map_next; it is
 *       freed automatically when the iteration ends. To stop early, call
 *       @ref map_break instead.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_set_at(Map_Iterator *iterator, Variant nevv);

/**
 * @ingroup map
 * @fn Variant map_set_at(Map_Iterator *iterator, Variant value)
 * @param iterator  a valid iterator positioned on an existing entry
 * @param value     the new value to store at the current position
 * @return the previous value stored at the iterator's current key, or
           VARIANT_NULL if the previous value compares equal to @p value
 *
 * This function replaces the value associated with the entry currently pointed
 * to by @p iterator. The key is left unchanged; only the value is updated.
 *
 * Internally, the implementation upgrades the iterator's read lock to a write
 * lock for the duration of the update, then restores it back to a read lock.
 * This ensures that the update is atomic with respect to other concurrent
 * map operations and that the iterator remains valid after the call.
 *
 * @note If the map has a persistent ordering policy, changing the value marks
 *       the traversal order stale. The active iterator is not re-sorted; the
 *       order will be restored before the next fresh traversal.
 *
 * @warning The iterator must currently point to a valid entry (i.e. it must
 *          be the result of a successful call to @ref map_each, @ref map_at
 *          or @ref map_next). Calling this function on an exhausted or broken
 *          iterator results in undefined behaviour.
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_remove_at(Map_Iterator *iterator);

/**
 * @ingroup map
 * @fn Variant map_remove_at(Map_Iterator *iterator)
 * @param iterator  a valid iterator positioned on an existing entry
 * @return the value that was stored at the iterator’s current key
 *
 * This function removes the entry currently pointed to by @p iterator from
 * the map and returns its value. The key is removed from the map; subsequent
 * lookups for that key will fail as if it had never been inserted.
 *
 * Internally, the implementation upgrades the iterator's read lock to a write
 * lock for the duration of the update, then restores it back to a read lock.
 * This ensures that the update is atomic with respect to other concurrent
 * map operations and that the iterator remains valid after the call.
 *
 * After @ref map_remove_at() returns, @p iterator remains valid but its
 * current position should be considered implementation-defined. The only
 * valid operations on the iterator are to continue the traversal with
 * @ref map_next() or to stop it with @ref map_break(). The caller must not
 * attempt to reuse the previous @c key/@c len/@c val fields after the entry
 * has been removed.
 *
 * The caller is responsible for disposing of the returned value if needed.
 * In particular, if the map was configured with a @c _freeval callback, that
 * callback is **not** invoked automatically by @ref map_remove_at(); it is
 * up to the caller to free or recycle the removed value as appropriate.
 *
 * @warning The iterator must currently point to a valid entry (i.e. it must
 *          be the result of a successful call to @ref map_each, @ref map_at
 *          or @ref map_next). Calling this function on an exhausted or broken
 *          iterator results in undefined behaviour.
 *
 * @see map_each()
 * @see map_next()
 * @see map_break()
 */

/* -------------------------------------------------------------------------- */

ASKL_API Map_Iterator *map_break(Map_Iterator *iterator);

/**
 * @ingroup map
 * @fn Map_Iterator *map_break(Map_Iterator *iterator)
 * @param iterator  an iterator obtained from @ref map_each or @ref map_at
 * @return always @c NULL
 *
 * This function explicitly destroys @p iterator and releases its read lock on
 * the underlying map. After this call, @p iterator must not be used again.
 *
 * This is the manual counterpart to the implicit destruction performed by
 * @ref map_next when the end of the traversal is reached.
 *
 * @note This function always returns @c NULL so that callers can conveniently
 *       clear their iterator variables:
 * @code
 * it = map_break(it);
 * @endcode
 */

/* _ENABLE_HASHMAP */
#endif

#endif
