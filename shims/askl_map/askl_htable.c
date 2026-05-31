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

#include "askl_htable.h"

/* -------------------------------------------------------------------------- */
#ifdef _ENABLE_HASHMAP
/* -------------------------------------------------------------------------- */

#include "arcane/bitops.c"
#include "arcane/htable.c"

typedef struct _Item {
    void *ptr;
    Variant val;
    struct {
        uint16_t len;
        char str[];
    } key;
} _Item;

/**
 * @ingroup map
 * @struct _Item
 *
 * Internal representation of a single key/value entry stored in a @ref Map.
 * An @ref _Item is never allocated on its own; it is always embedded
 * inside a @ref _Bucket.
 *
 * @b private @ref key.len is the key length in bytes (excluding the NUL byte).
 * @b private @ref key.str is the inline key storage (flexible array), always
 *                         terminated with a NUL character.
 * @b private @ref val is the value stored in the map, as a @ref variant.
 * @b private @ref ptr has two roles depending on where the item is stored:
 *   - in the main hash table (@ref Map::_bucket), it caches the
 *     primary hash value (hash0) as a uintptr_t
 *   - in the overflow basket (@ref Map::_basket), @ref ptr is
 *     used as the "next" link in the basket's singly linked list.
 *
 * This type is internal and may change at any time.
 */

typedef struct _Bucket {
    struct _Bucket *next;
    _Item item;
} _Bucket;

#if (TAG_SHIFT > 0)
STATIC_ASSERT(
    ((ALIGNOF(_Bucket) | offsetof(_Bucket, item)) & TAG_MASK) == 0,
    pointer_alignment_unsuitable_for_tagging
);
#endif

/**
 * @ingroup map
 * @struct _Bucket
 *
 * Internal node of the map's traversal list.
 *
 * A Map keeps two views of the same entries:
 * - the hash index (@ref Map::_bucket), used for key lookup;
 * - the traversal list (@ref Map::_index), used for iteration and sorting.
 *
 * Each _Bucket is one node in the traversal list and owns one embedded
 * @ref _Item. The hash index and overflow basket do not allocate separate
 * entries; they point to the @ref _Item stored inside these buckets.
 *
 * Sorting only relinks _Bucket nodes in the traversal list. It does not move
 * or copy keys and values, and it does not change lookup semantics.
 *
 * @b private @ref next links the next node in traversal order.
 * @b private @ref item stores the key/value payload for this entry.
 *
 * This type is internal and may change at any time.
 */

struct _Map {
    RW_Lock *_lock;
    struct _Bucket *_index;
    struct _Item **_bucket;
    struct _Item *_basket;
    size_t _bucket_size;
    size_t _bucket_count;
    void (*_freeval)(Variant);
    uintptr_t _seed[HASH_COUNT];
    Map_Comparator _cmpfn;
    int8_t _order;
    uint8_t _state;
};

#define MAP_INDEX_STALE 0x1
#define MAP_DATA_CHANGE 0x2
#define MAP_STATE_DIRTY (MAP_DATA_CHANGE | MAP_INDEX_STALE)

/**
 * @ingroup map
 * @struct _Map
 *
 * This structure holds the internal state of a @ref Map.
 *
 * A Map is a hash-indexed associative container that also maintains a stable
 * traversal order. It combines cuckoo hashing for O(1) expected-time lookup
 * with an overflow basket for guaranteed insertion when cuckoo displacement
 * fails.
 *
 * Concurrency:
 * - Readers hold a read lock to allow concurrent lookups/traversals.
 * - Writers take a write lock to insert/remove/resize/sort.
 *
 * @b private @ref _lock is a reader/writer lock protecting the whole map.
 * @b private @ref _index is the head of the traversal list. Each node embeds
 *                 an @ref _Item and supports O(n) ordered traversal.
 * @b private @ref _bucket is the hash index table. Its entries point to
 *                 items embedded in the traversal list.
 * @b private @ref _basket is the overflow chain head for items that failed
 *                 cuckoo placement after HASH_RETRY displacement attempts.
 * @b private @ref _bucket_size is the current capacity of the hash index.
 * @b private @ref _bucket_count is the number of occupied hash slots/items
 *                 tracked by the hash index.
 * @b private @ref _freeval is an optional destructor callback used when
 *                 removing entries from the map.
 * @b private @ref _seed is the per-map hash seed material (HASH_COUNT words).
 * @b private @ref _cmpfn is the persistent comparator used to maintain sorted
 *                 traversal order, or NULL if no persistent sort is active.
 * @b private @ref _order is the persistent sort order applied to @ref _cmpfn.
 * @b private @ref _state stores internal state flags:
 *                 @ref MAP_INDEX_STALE indicates that the traversal index
 *                 should be lazily re-sorted before the next ordered
 *                 traversal; @ref MAP_DATA_CHANGE indicates that map contents
 *                 changed since the last observer/cache refresh.
 *
 * This type is internal and may change at any time; only use the public
 * @ref Map API.
 */

/* -------------------------------------------------------------------------- */

static int _probe(Map *h, unsigned int i, _Item *nevv, int replace)
{
    if (likely(! h->_bucket[i])) {
        h->_bucket[i] = TAG_PTR(nevv, nevv->ptr);
        h->_bucket_count ++;
        /* insert */
        if (replace >= 0) {
            if (~h->_state & MAP_STATE_DIRTY)
                h->_state |= MAP_STATE_DIRTY;
        }
        return 0;
    }

    if (replace >= 0 && unlikely(GET_TAG(h->_bucket[i]) == HASHTAG(nevv->ptr))) {
        _Item *slot = GET_PTR(h->_bucket[i]);
        if (nevv->ptr == slot->ptr) {
            if (nevv->key.len == slot->key.len) {
                if (! memcmp(nevv->key.str, slot->key.str, slot->key.len)) {
                    /* update if allowed */
                    return (replace == CREATE_ONLY) ? -1 : 1;
                }
            }
        }
    }

    /* continue */
    return INT_MAX;
}

/* -------------------------------------------------------------------------- */

static Variant _update(
    Map *h,
    _Item *slot,
    Variant nevv,
    Variant (*on_update)(const char *k, size_t l, Variant old, Variant nevv)
)
{
    Variant old = slot->val, rejected = old;
    int aliased = variant_equal(nevv, old), changed = ! aliased;

    if (on_update) {
        Variant val = on_update(slot->key.str, slot->key.len, old, nevv);
        if (! variant_equal(nevv, val)) {
            if ( (changed = ! variant_equal(old, val)) ) {
                /* the function returned an entirely new value */
                if (h->_freeval) h->_freeval(old);
            }
            /* the item value was unused */
            rejected = nevv;
        }
        nevv = val;
    }

    if (changed) {
        if (~h->_state & MAP_STATE_DIRTY)
            h->_state |= MAP_STATE_DIRTY;
        slot->val = nevv;
    }

    return (aliased) ? variant_null() : rejected;
}

/* -------------------------------------------------------------------------- */

static int _set_item(
    Map *h,
    _Item *item,
    int replace,
    Variant *val,
    Variant (*on_insert)(const char *k, size_t l, Variant nevv),
    Variant (*on_update)(const char *k, size_t l, Variant old, Variant nevv)
)
{
    unsigned int i = 0, index = 0, retry = 0;
    _Item *slot = NULL;
    uintptr_t hash = 0, mask = h->_bucket_size - 1;

    /* avoid rehashing every key */
    if (unlikely(! (hash = (uintptr_t) item->ptr))) {
        hash = _hash(item->key.str, item->key.len, h->_seed[0]);
        item->ptr = (void *) hash;
    }

    #if (UINTPTR_MAX == 0xffffffffffffffffULL)
    PREFETCH(& h->_bucket[(hash >> 32) & mask], 1, L1_CACHE);
    #endif

    goto _loop;

    /* look for a free slot */
    for (i = 0; i < HASH_COUNT; i ++) {
        int probe;

        hash = _hash(item->key.str, item->key.len, h->_seed[i]);
        #if (UINTPTR_MAX == 0xffffffffffffffffULL)
        PREFETCH(& h->_bucket[(hash >> 32) & mask], 1, L2_CACHE);
        #endif

_loop:  index = hash & mask;
        if ( (probe = _probe(h, index, item, replace)) == 0) {
            if (on_insert)
                item->val = on_insert(item->key.str, item->key.len, item->val);
            return 0;
        } else if (unlikely(probe == -1)) goto _failure;

        if (probe == 1) {
            slot = GET_PTR(h->_bucket[index]);
            goto _replace;
        }

        #if (UINTPTR_MAX == 0xffffffffffffffffULL)
        /* second probe on 64 bits systems */
        index = (hash >> 32) & mask;
        if ( (probe = _probe(h, index, item, replace)) == 0) {
            if (on_insert)
                item->val = on_insert(item->key.str, item->key.len, item->val);
            return 0;
        } else if (unlikely(probe == -1)) goto _failure;

        if (probe == 1) {
            slot = GET_PTR(h->_bucket[index]);
            goto _replace;
        }
        #endif
    }

    /* couldn't find it, look in the basket */
    if (replace != REHASH_ONLY) {
        for (slot = h->_basket; slot; slot = (_Item *) slot->ptr) {
            if (likely(item->key.len == slot->key.len)) {
                if (! memcmp(item->key.str, slot->key.str, item->key.len)) {
                    if (replace == CREATE_ONLY)
                        goto _failure;
                    goto _replace;
                }
            }
        }

        /* no free slot found, the new item will be forcefully inserted */
        if (on_insert)
            item->val = on_insert(item->key.str, item->key.len, item->val);
        if (~h->_state & MAP_STATE_DIRTY)
            h->_state |= MAP_STATE_DIRTY;
    }

    /* try cuckoo eviction */
    for (index = (uintptr_t) item->ptr & mask; retry < HASH_RETRY; retry ++) {
        _Item *tmp = GET_PTR(h->_bucket[index]);
        int loop = (tmp->ptr == item->ptr);

        h->_bucket[index] = TAG_PTR(item, item->ptr); item = tmp;

        /* get rid of tombstones */
        if (unlikely(! item->key.len)) return 0;

        #if (UINTPTR_MAX == 0xffffffffffffffffULL)
        index = (((uintptr_t) item->ptr) >> 32) & mask;
        if (_probe(h, index, item, REHASH_ONLY) == 0)
            return 0;
        #endif

        for (i = 1; i < HASH_COUNT; i ++) {
            hash = _hash(item->key.str, item->key.len, h->_seed[i]);
            index = hash & mask;
            if (_probe(h, index, item, REHASH_ONLY) == 0)
                return 0;
            #if (UINTPTR_MAX == 0xffffffffffffffffULL)
            /* second probe on 64 bits systems */
            index = (hash >> 32) & mask;
            if (_probe(h, index, item, REHASH_ONLY) == 0)
                return 0;
            #endif
        }

        /* avoid evicting the original cuckoo */
        if (unlikely(loop)) break;
    }

    /* store the key in the overflow basket */
    item->ptr = h->_basket; h->_basket = item; h->_bucket_count ++;

    return 0;

_replace:
    *val = _update(h, slot, item->val, on_update);
    return 1;

_failure:
    *val = item->val; /* return the rejected value */
    return -1;
}

/* -------------------------------------------------------------------------- */

static int _grow(Map *h, size_t size)
{
    _Bucket *b = NULL, *next = NULL;
    _Bucket **prev = NULL;
    _Item *item = NULL, *tmp = NULL;
    _Item **nevv = NULL;

    /* round the size to the next highest power of 2 */
    size = __next_pow2(size);
    if (unlikely(size < MAP_MIN_SIZE))
        size = MAP_MIN_SIZE;

    /* try to allocate a new bucket array */
    if (! (nevv = (_Item **) calloc(size, sizeof(*h->_bucket))) ) {
        perror(ERR(_grow, calloc));
        return -1;
    }

    /* clear the basket */
    for (item = h->_basket, h->_basket = NULL; item; item = tmp) {
        tmp = (_Item *) item->ptr;
        item->ptr = NULL;
    }

    /* replace the bucket array */
    free(h->_bucket); h->_bucket = nevv;

    /* update the state */
    h->_bucket_size = size; h->_bucket_count = 0;

    /* rehash old buckets */
    for (prev = & h->_index, b = h->_index; b; b = next) {
        next = b->next;

        if (unlikely(! b->item.key.len)) {
            *prev = next;
            free(b);
            continue;
        }

        prev = & b->next;

        _set_item(h, & b->item, REHASH_ONLY, NULL, NULL, NULL);
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

static inline int _check_size(Map *h)
{
    if (likely(h->_bucket_count <= (h->_bucket_size * MAP_MAX_LOAD) / 1000))
        return 0;

    return _grow(h, h->_bucket_size + 1);
}

/* -------------------------------------------------------------------------- */

static inline Variant _insert(
    Map *h,
    const char *key,
    size_t len,
    Variant val,
    int replace,
    Variant (*on_insert)(const char *k, size_t l, Variant nevv),
    Variant (*on_update)(const char *k, size_t l, Variant old, Variant nevv)
)
{
    _Bucket *bucket = NULL;

    #ifdef DEBUG
    if (unlikely(len >= UINT16_MAX)) {
        debug("_insert(): key is too long.\n");
        return val;
    }
    #endif

    /* replace the key by a dynamically allocated one */
    if (! (bucket = (_Bucket *) malloc(sizeof(*bucket) + len + 1)) ) {
        perror(ERR(_insert, malloc));
        return val;
    }

    memcpy(bucket->item.key.str, key, len);
    bucket->item.key.str[len] = '\0';
    bucket->item.key.len = len;
    bucket->item.val = val;
    bucket->item.ptr = NULL;

    if (lock_wrlock(h->_lock) == -1) goto _err_lock;

    if (_check_size(h) == -1) goto _err_size;

    if (! _set_item(h, & bucket->item, replace, & val, on_insert, on_update)) {
        bucket->next = h->_index; h->_index = bucket;
        val = variant_null();
    } else free(bucket);

    lock_unlock(h->_lock);

    return val;

_err_size:
    lock_unlock(h->_lock);
_err_lock:
    free(bucket);
    return val;
}

/* -------------------------------------------------------------------------- */

ASKL_API Map *map_alloc(void (*freeval)(Variant))
{
    Map *h = NULL;

    if (! (h = (Map *) malloc(sizeof(*h))) ) {
        perror(ERR(map_alloc, malloc));
        return NULL;
    }

    if (random_seed((uint32_t *) h->_seed, sizeof(h->_seed) / 4) == -1)
        goto _err_rand;

    for (int i = 0; i < HASH_COUNT; i ++)
        _check_seed(& h->_seed[i]);

    if (! (h->_lock = lock_alloc()) ) goto _err_lock;
    if (lock_init(h->_lock) == -1) goto _err_init;

    h->_bucket = NULL;
    h->_bucket_count = h->_bucket_size = 0; h->_basket = NULL;
    h->_index = NULL;
    h->_freeval = freeval;

    h->_cmpfn = NULL;
    h->_order = 0;
    h->_state = 0;

    if (_grow(h, 0) == -1) goto _err_init;

    return h;

_err_init:
    lock_free(h->_lock);
_err_lock:
_err_rand:
    free(h);

    return NULL;
}

/* -------------------------------------------------------------------------- */

ASKL_API void map_reserve(Map *h, size_t count)
{
    size_t size = 0;

    if (! h) {
        debug("map_reserve(): bad parameters.\n");
        return;
    }

    if (count > (SIZE_MAX - (MAP_MAX_LOAD - 1)) / 1000) {
        debug("_reserve_size(): requested capacity is too large.\n");
        return;
    }

    size = ((count * 1000) + (MAP_MAX_LOAD - 1)) / MAP_MAX_LOAD;

    if (lock_wrlock(h->_lock) == 0) {
        if (size > h->_bucket_size)
            _grow(h, size);
        lock_unlock(h->_lock);
    }
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_set_with(
    Map *h,
    const char *k,
    size_t l,
    Variant v,
    Variant (*function)(const char *k, size_t l, Variant old, Variant nevv)
)
{
    if (unlikely(! h || ! k || ! l)) {
        debug("map_set_with(): bad parameters.\n");
        return v;
    }

    return _insert(h, k, l, v, STORE_VALUE, NULL, function);
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_set(Map *h, const char *k, size_t l, Variant v)
{
    if (unlikely(! h || ! k || ! l)) {
        debug("map_set(): bad parameters.\n");
        return v;
    }

    return _insert(h, k, l, v, STORE_VALUE, NULL, NULL);
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_insert_with(
    Map *h,
    const char *k,
    size_t l,
    Variant v,
    Variant (*function)(const char *k, size_t l, Variant nevv)
)
{
    if (unlikely(! h || ! k || ! l)) {
        debug("map_insert_with(): bad parameters.\n");
        return v;
    }

    return _insert(h, k, l, v, CREATE_ONLY, function, NULL);
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_insert(Map *h, const char *k, size_t l, Variant v)
{
    if (unlikely(! h || ! k || ! l)) {
        debug("map_insert(): bad parameters.\n");
        return v;
    }

    return _insert(h, k, l, v, CREATE_ONLY, NULL, NULL);
}

/* -------------------------------------------------------------------------- */

static _Item *_get_item(Map *h, const char *k, size_t l, Variant *v)
{
    unsigned int i = 0;
    uintptr_t h0, hash, mask = h->_bucket_size - 1;
    _Item *ptr = NULL;

    #ifdef DEBUG
    if (unlikely(l >= UINT16_MAX)) {
        debug("_get_item(): key is too long.\n");
        return NULL;
    }
    #endif

    h0 = hash = _hash(k, l, h->_seed[0]);
    #if (UINTPTR_MAX == 0xffffffffffffffffULL)
    PREFETCH(& h->_bucket[(hash >> 32) & mask], 0, L1_CACHE);
    #endif
    goto _loop;

    for (i = 0; i < HASH_COUNT; i ++) {
        hash = _hash(k, l, h->_seed[i]);
        #if (UINTPTR_MAX == 0xffffffffffffffffULL)
        PREFETCH(& h->_bucket[(hash >> 32) & mask], 0, L2_CACHE);
        #endif

        #define _MAP_GET(index) \
        /* if an empty slot is found, no need to look further */ \
        if (! (ptr = h->_bucket[(index)]) ) break; \
        if (GET_TAG(ptr) == HASHTAG(h0)) { \
            ptr = GET_PTR(ptr); \
            if ((uintptr_t) ptr->ptr == h0 && likely(ptr->key.len == l)) { \
                if (likely(memcmp(ptr->key.str, k, l) == 0)) { \
                    *v = ptr->val; \
                    return ptr; \
                } \
            } \
        }

_loop:  _MAP_GET(hash & mask);

        #if (UINTPTR_MAX == 0xffffffffffffffffULL)
        /* second probe on 64 bits systems */
        _MAP_GET((hash >> 32) & mask);
        #endif

        #undef _MAP_GET
    }

    /* scan the overflow basket */
    for (ptr = h->_basket; ptr; ptr = (_Item *) ptr->ptr) {
        if (ptr->key.len == l && memcmp(ptr->key.str, k, l) == 0) {
            *v = ptr->val;
            return ptr;
        }
    }

    return NULL;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_update_with(
    Map *h,
    const char *k,
    size_t l,
    Variant nevv,
    Variant (*function)(const char *k, size_t l, Variant old, Variant nevv)
)
{
    Variant old = { { 0 } }, res = nevv;
    _Item *slot = NULL;

    if (unlikely(! h || ! k || ! l)) {
        debug("map_update_with(): bad parameters.\n");
        return res;
    }

    if (lock_rdlock(h->_lock) == -1) return res;

        if ( (slot = _get_item(h, k, l, & old) ) ) {
            if (variant_equal(nevv, old) && ! function) {
                /* no-op, avoid taking the write lock */
                res = variant_null();
                goto _noop;
            }

            if (lock_upgrade(h->_lock) == 0) {

                /* check if the value was deleted during upgrade */
                if (likely(slot->key.len))
                    res = _update(h, slot, nevv, function);

                lock_restore(h->_lock);
            } else goto _fail;
        }
_noop:
    lock_unlock(h->_lock);

_fail:
    return res;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_update(Map *h, const char *k, size_t l, Variant v)
{
    if (unlikely(! h || ! k || ! l)) {
        debug("map_update(): bad parameters.\n");
        return v;
    }

    return map_update_with(h, k, l, v, NULL);
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_get_with(
    Map *h,
    const char *key,
    size_t len,
    Variant (*function)(Variant)
)
{
    Variant res = { { 0 } };

    if (unlikely(! h || ! key || ! len)) {
        debug("map_get_with(): bad parameters.\n");
        return res;
    }

    if (lock_rdlock(h->_lock) == -1) return res;

        if (_get_item(h, key, len, & res) && function)
            res = function(res);

    lock_unlock(h->_lock);

    return res;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_get(Map *h, const char *key, size_t len)
{
    Variant res = { { 0 } };

    if (unlikely(! h || ! key || ! len)) {
        debug("map_get(): bad parameters.\n");
        return res;
    }

    if (lock_rdlock(h->_lock) == -1) return res;

        _get_item(h, key, len, & res);

    lock_unlock(h->_lock);

    return res;
}

/* -------------------------------------------------------------------------- */

static Variant _exists(UNUSED Variant v)
{
    return variant_true();
}

/* -------------------------------------------------------------------------- */

ASKL_API int map_has(Map *h, const char *key, size_t len)
{
    Variant v = map_get_with(h, key, len, _exists);
    return (is_boolean(v) && variant_to_boolean(v));
}

/* -------------------------------------------------------------------------- */

ASKL_API int map_merge(
    Map *dest,
    Map *src,
    Variant merge(const char *key, size_t len, Variant destval, Variant srcval)
)
{
    _Bucket *b = NULL, *next = NULL;

    if (! dest || ! src || ! merge) {
        debug("map_merge(): bad parameters.\n");
        return -1;
    }

    if (unlikely(dest == src)) {
        debug("map_merge(): source and destination are the same map.\n");
        return -1;
    }

    /* pry both maps open */
    if (lock_wrlock(dest->_lock) == -1) return -1;
    if (lock_wrlock(src->_lock) == -1) {
        lock_unlock(dest->_lock);
        return -1;
    }

    lock_break(src->_lock);

    for (b = src->_index, src->_index = NULL; b; b = next) {
        Variant v = variant_null();
        next = b->next;

        if (! b->item.key.len) { free(b); continue; }

        b->item.ptr = NULL;

        /* handle conflicts with the merge helper */
        if (_set_item(dest, & b->item, 1, & v, NULL, merge)) {
            if (variant_equal(v, b->item.val)) {
                if (src->_freeval) src->_freeval(v);
            } else if (dest->_freeval) dest->_freeval(v);
            free(b);
        } else {
            b->next = dest->_index; dest->_index = b;
        }
    }

    /* destroy the source map */
    free(src->_bucket);
    lock_destroy(src->_lock);
    lock_free(src->_lock); free(src);

    lock_unlock(dest->_lock);

    return 0;
}

/* -------------------------------------------------------------------------- */

static inline _Bucket *_select_run(
    _Bucket **src,
    _Bucket **run_tail,
    _Bucket ***tombstones,
    unsigned int order,
    Map_Comparator cmp
)
{
    _Bucket *head = NULL, *tail = NULL, *cur;
    _Bucket **link = & head;

    while ( (cur = *src) ) {
        if (unlikely(! cur->item.key.len)) {
            **tombstones = cur;
            *tombstones = & cur->next;
            *src = cur->next;
            cur->next = NULL;
            continue;
        }

        if (tail) {
            int res = cmp(
                tail->item.key.str, tail->item.key.len, tail->item.val,
                cur->item.key.str, cur->item.key.len, cur->item.val
            );

            if (res && ((res > 0) ^ order))
                break;
        }

        *link = tail = cur;
        link = & cur->next;
        *src = cur->next;
    }

    if ( (*run_tail = tail) )
        tail->next = NULL;

    return head;
}

/* -------------------------------------------------------------------------- */

static void _sort(Map *h, unsigned int order, Map_Comparator cmp)
{
    _Bucket *dead_head = NULL, *live_tail = NULL;
    _Bucket **tombstones = & dead_head;
    int did_merge;

    do {
        _Bucket *res_head = NULL, *res_tail = NULL, *cur = h->_index;

        did_merge = 0;

        while (cur) {
            _Bucket *run[2], *tail[2], *merged, *merged_tail;
            int boundary;

            run[0] = _select_run(& cur, & tail[0], & tombstones, order, cmp);
            if (! run[0]) break;

            run[1] = _select_run(& cur, & tail[1], & tombstones, order, cmp);
            if (! run[1]) {
                if (res_tail)
                    res_tail->next = run[0];
                else
                    res_head = run[0];
                res_tail = tail[0];
                break;
            }

            /* fast path: check boundary between run 0 tail and run 1 head */
            boundary = cmp(
                tail[0]->item.key.str, tail[0]->item.key.len, tail[0]->item.val,
                run[1]->item.key.str, run[1]->item.key.len, run[1]->item.val
            );

            if (! boundary || ((boundary < 0) ^ order)) {
                tail[0]->next = run[1];
                merged = run[0];
                merged_tail = tail[1];
            } else {
                /* check boundary between run 1 tail and run 0 head */
                boundary = cmp(
                    tail[1]->item.key.str, tail[1]->item.key.len, tail[1]->item.val,
                    run[0]->item.key.str, run[0]->item.key.len, run[0]->item.val
                );

                if (boundary && ((boundary < 0) ^ order)) {
                    tail[1]->next = run[0];
                    merged = run[1];
                    merged_tail = tail[0];
                } else {
                    _Bucket *merged_head, *a = run[0], *b = run[1];
                    _Bucket **mlink = & merged_head;

                    /* merge both runs */
                    while (a && b) {
                        _Bucket **pick;
                        int res = cmp(
                            a->item.key.str, a->item.key.len, a->item.val,
                            b->item.key.str, b->item.key.len, b->item.val
                        );

                        /* pick a side and preserve stability */
                        pick = ((order) ? (res >= 0) : (res <= 0)) ? & a : & b;

                        *mlink = *pick;
                        mlink = & (*pick)->next;
                        *pick = (*pick)->next;
                    }

                    *mlink = (a) ? a : b;
                    merged = merged_head;
                    merged_tail = (a) ? tail[0] : tail[1];
                }
            }

            did_merge = 1;

            if (res_tail)
                res_tail->next = merged;
            else
                res_head = merged;
            res_tail = merged_tail;
        }

        h->_index = res_head;
        live_tail = res_tail;
    } while (did_merge);

    if (live_tail)
        live_tail->next = dead_head;
    else
        h->_index = dead_head;
}

/* -------------------------------------------------------------------------- */

ASKL_API int map_sort(Map *h, unsigned int order, Map_Comparator cmp)
{
    int sort_once = order & MAP_SORT_ONCE;

    order ^= sort_once;

    if (! h || ! cmp || (order != MAP_ASC && order != MAP_DESC)) {
        debug("map_sort(): bad parameters.\n");
        return -1;
    }

    if (lock_wrlock(h->_lock) == -1) return -1;

        _sort(h, order, cmp);
        h->_state &= ~MAP_INDEX_STALE;

        if (! sort_once) {
            h->_cmpfn = cmp;
            h->_order = order;
        }

    lock_unlock(h->_lock);

    return 0;
}

/* -------------------------------------------------------------------------- */

ASKL_API int map_sort_keys(
    const char *key0,
    size_t len0,
    UNUSED Variant val0,
    const char *key1,
    size_t len1,
    UNUSED Variant val1
)
{
    int ret;
    size_t len = (len0 < len1) ? len0 : len1;

    if ( (ret = memcmp(key0, key1, len)) )
        return ret;

    return (len0 > len1) - (len0 < len1);
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_remove_if(
    Map *h,
    const char *key,
    size_t len,
    int (*condition)(const char *key, size_t len, Variant val)
)
{
    unsigned int i = 0;
    _Item *tmp = NULL, *prev = NULL;
    Variant result = { { 0 } };
    uintptr_t h0 = 0, hash = 0, mask = 0;

    if (! h || ! key || ! len) {
        debug("map_remove(): bad parameters.\n");
        return result;
    }

    if (lock_wrlock(h->_lock) == -1) return result;

    mask = h->_bucket_size - 1;
    h0 = hash = _hash(key, len, h->_seed[i]);
    goto _loop;

    for (i = 0; i < HASH_COUNT; i ++) {
        hash = _hash(key, len, h->_seed[i]);

        #define _MAP_REMOVE(index) \
        if ( (tmp = h->_bucket[(index)]) && GET_TAG(tmp) == HASHTAG(h0)) { \
            tmp = GET_PTR(tmp); \
            if ((uintptr_t) tmp->ptr == h0 && likely(tmp->key.len == len)) { \
                if (likely(memcmp(tmp->key.str, key, len) == 0)) { \
                    if (! condition || condition(key, len, tmp->val)) { \
                        /* remove from the bucket */ \
                        result = tmp->val; \
                        /* a length of 0 indicates a tombstone */ \
                        tmp->key.len = 0; \
                        /* mark the map as dirty */ \
                        h->_state |= MAP_DATA_CHANGE; \
                    } \
                    goto _result; \
                } \
            } \
        }

_loop:  _MAP_REMOVE(hash & mask);

        #if (UINTPTR_MAX == 0xffffffffffffffffULL)
        /* second probe on 64 bits systems */
        _MAP_REMOVE((hash >> 32) & mask);
        #endif

        #undef _MAP_REMOVE
    }

    /* scan the overflow basket */
    for (tmp = prev = h->_basket; tmp; prev = tmp, tmp = (_Item *) tmp->ptr) {
        if (tmp->key.len == len) {
            if (memcmp(tmp->key.str, key, len) == 0) {
                if (! condition || condition(key, len, tmp->val)) {
                    /* remove from the basket */
                    result = tmp->val;
                    if (tmp == h->_basket) h->_basket = (_Item *) tmp->ptr;
                    else prev->ptr = tmp->ptr;

                    /* tombstone */
                    tmp->key.len = 0;

                    /* mark the map as dirty */
                    h->_state |= MAP_DATA_CHANGE;
                }
                goto _result;
            }
        }
    }

_result:
    lock_unlock(h->_lock);

    return result;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_remove(Map *h, const char *key, size_t len)
{
    return map_remove_if(h, key, len, NULL);
}

/* -------------------------------------------------------------------------- */

ASKL_API void map_foreach(
    Map *h,
    int (*f)(const char *, size_t, Variant, void *),
    void *context
)
{
    _Bucket *bucket = NULL;

    if (! h || ! f) {
        debug("map_foreach(): bad parameters.\n");
        return;
    }

    if (lock_wrlock(h->_lock) == -1) return;

    if (h->_cmpfn && (h->_state & MAP_INDEX_STALE)) {
        _sort(h, h->_order, h->_cmpfn);
        h->_state &= ~MAP_INDEX_STALE;
    }

    for (bucket = h->_index; bucket; bucket = bucket->next) {
        if (bucket->item.key.len) {
            int ret = f(
                bucket->item.key.str,
                bucket->item.key.len,
                bucket->item.val,
                context
            );
            if (ret == -1) {
                /* delete the record */
                if (h->_freeval)
                    h->_freeval(bucket->item.val);
                bucket->item.key.len = 0;
                h->_state |= MAP_DATA_CHANGE;
            } else if (ret == 1) break;
        }
    }

    lock_unlock(h->_lock);

    return;
}

/* -------------------------------------------------------------------------- */

ASKL_API size_t map_footprint(Map *h, size_t *overhead)
{
    _Bucket *bucket = NULL;
    size_t key = 0;
    size_t ret = sizeof(*h);

    if (! h) {
        debug("map_footprint(): bad parameters.\n");
        return 0;
    }

    if (lock_wrlock(h->_lock) == -1) return 0;

    if (h->_bucket_size) {
        /* bucket size */
        ret += h->_bucket_size * sizeof(*h->_bucket);
        /* keys */
        for (bucket = h->_index; bucket; bucket = bucket->next) {
            if (bucket->item.key.len) {
                /* key length + key recorded size + next and value pointers */
                ret += (
                    sizeof(char *) + bucket->item.key.len +
                    sizeof(bucket->item.key.len) +
                    sizeof(char *) + sizeof(Variant)
                );
                /* key length and value pointer are not overhead */
                key += (
                    sizeof(bucket->item.key.len) +
                    bucket->item.key.len + sizeof(void *)
                );
            }
        }
    }

    lock_unlock(h->_lock);

    if (overhead) *overhead = ret - key;

    return ret;
}

/* -------------------------------------------------------------------------- */

ASKL_API Map *map_free(Map *h)
{
    _Bucket *bucket = NULL, *next = NULL;

    if (! h) return NULL;

    if (lock_wrlock(h->_lock) == -1) return NULL;

    /* free the threads waiting after the linked hashmap */
    lock_break(h->_lock);

    for (bucket = h->_index; bucket; bucket = next) {
        next = bucket->next;
        if (h->_freeval && bucket->item.key.len)
            h->_freeval(bucket->item.val);
        free(bucket);
    }

    free(h->_bucket);
    lock_destroy(h->_lock);
    lock_free(h->_lock); free(h);

    return NULL;
}

/* -------------------------------------------------------------------------- */
/* Iterator */
/* -------------------------------------------------------------------------- */

ASKL_API Map_Iterator *map_each(Map *h)
{
    Map_Iterator *iterator = NULL;

    if (! h) {
        debug("map_each(): bad parameters.\n");
        return NULL;
    }

    if (! (iterator = (Map_Iterator *) malloc(sizeof(*iterator)))) {
        perror(ERR(map_each, malloc));
        return NULL;
    }

    if (lock_rdlock(h->_lock) == -1) goto _err_lock;

    if (! h->_index) {
        debug("map_each(): empty map.\n");
        goto _err_init;
    }

    if (h->_cmpfn && (h->_state & MAP_INDEX_STALE)) {
        if (lock_upgrade(h->_lock) == -1) goto _err_lock;
            if (h->_state & MAP_INDEX_STALE) {
                _sort(h, h->_order, h->_cmpfn);
                h->_state &= ~MAP_INDEX_STALE;
            }
        lock_restore(h->_lock);
    }

    iterator->map = h;
    iterator->_current = h->_index;
    if (likely(iterator->_current->item.key.len)) {
        iterator->key = h->_index->item.key.str;
        iterator->len = h->_index->item.key.len;
        iterator->val = h->_index->item.val;
    } else return map_next(iterator);

    return iterator;

_err_init:
    lock_unlock(h->_lock);
_err_lock:
    free(iterator);
    return NULL;
}

/* -------------------------------------------------------------------------- */

ASKL_API Map_Iterator *map_at(Map *h, const char *key, size_t len)
{
    Map_Iterator *iterator = NULL;
    uint8_t *ptr = NULL;

    if (! h || ! key || ! len) {
        debug("map_at(): bad parameters.\n");
        return NULL;
    }

    if (! (iterator = (Map_Iterator *) malloc(sizeof(*iterator)))) {
        perror(ERR(map_at, malloc));
        return NULL;
    }

    iterator->map = h;

    if (lock_rdlock(h->_lock) == -1) goto _err_lock;

    if (h->_cmpfn && (h->_state & MAP_INDEX_STALE)) {
        if (lock_upgrade(h->_lock) == -1) goto _err_lock;
            if (h->_state & MAP_INDEX_STALE) {
                _sort(h, h->_order, h->_cmpfn);
                h->_state &= ~MAP_INDEX_STALE;
            }
        lock_restore(h->_lock);
    }

    if (! (ptr = (uint8_t *) _get_item(h, key, len, & iterator->val))) {
        debug("map_at(): key not found.\n");
        goto _err_item;
    }

    /* find the bucket from the item address */
    iterator->_current = (_Bucket *) (ptr - offsetof(_Bucket, item));
    iterator->key = iterator->_current->item.key.str;
    iterator->len = iterator->_current->item.key.len;

    return iterator;

_err_item:
    lock_unlock(h->_lock);
_err_lock:
    free(iterator);
    return NULL;
}

/* -------------------------------------------------------------------------- */

ASKL_API Map_Iterator *map_next(Map_Iterator *iterator)
{
    _Bucket *bucket = NULL;

    for (bucket = iterator->_current->next; bucket; bucket = bucket->next) {
        if (likely(bucket->item.key.len)) {
            iterator->_current = bucket;
            iterator->key = bucket->item.key.str;
            iterator->len = bucket->item.key.len;
            iterator->val = bucket->item.val;
            return iterator;
        }
    }

    return map_break(iterator);
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_set_at(Map_Iterator *iterator, Variant nevv)
{
    Variant old = nevv;
    int aliased = 0;

    if (lock_upgrade(iterator->map->_lock) == -1) return nevv;
        /* XXX another thread may have deleted the entry during upgrade */
        if (likely(iterator->_current->item.key.len)) {
            old = iterator->_current->item.val;
            if (! (aliased = variant_equal(old, nevv)) ) {
                iterator->_current->item.val = nevv;
                iterator->val = nevv;
                iterator->map->_state |= MAP_STATE_DIRTY;
            }
        }
    lock_restore(iterator->map->_lock);

    return (aliased) ? variant_null() : old;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant map_remove_at(Map_Iterator *iterator)
{
    Variant ret = { { 0 } };
    unsigned int len = 0;

    if (lock_upgrade(iterator->map->_lock) == -1) return ret;
        /* XXX another thread may have deleted the entry during upgrade */
        if (likely(len = iterator->_current->item.key.len)) {
            ret = iterator->_current->item.val;
            iterator->_current->item.key.len = 0;
            iterator->map->_state |= MAP_DATA_CHANGE;
        }
    lock_restore(iterator->map->_lock);

    return ret;
}

/* -------------------------------------------------------------------------- */

ASKL_API Map_Iterator *map_break(Map_Iterator *iterator)
{
    if (! iterator) {
        debug("map_break(): bad parameters.\n");
        return NULL;
    }

    lock_unlock(iterator->map->_lock);
    free(iterator);

    return NULL;
}

/* -------------------------------------------------------------------------- */
#else
/* -------------------------------------------------------------------------- */

#ifdef __GNUC__
__attribute__ ((unused)) static int __dummy__ = 0;
#endif

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */
