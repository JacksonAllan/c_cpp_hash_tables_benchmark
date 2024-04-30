// hash_map_benchmarks/shims/uthash/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#include "uthash.h"

template< typename > struct uthash
{
  static constexpr const char *label = "uthash";
  static constexpr const char *color = "rgb( 145, 99, 127 )";
};

#define UTHASH_SPECIALIZATION( blueprint )                                               \
                                                                                         \
struct uthash_##blueprint##_bucket                                                       \
{                                                                                        \
  blueprint::key_type key;                                                               \
  blueprint::value_type value;                                                           \
  UT_hash_handle hh;                                                                     \
};                                                                                       \
                                                                                         \
template<> struct uthash< blueprint >                                                    \
{                                                                                        \
  using map_type = uthash_##blueprint##_bucket *;                                        \
  using itr_type = uthash_##blueprint##_bucket *;                                        \
                                                                                         \
  static map_type create_map()                                                           \
  {                                                                                      \
    return NULL;                                                                         \
  }                                                                                      \
                                                                                         \
  static itr_type find( map_type &map, const blueprint::key_type &key )                  \
  {                                                                                      \
    unsigned int hash = blueprint::hash_key( key );                                      \
                                                                                         \
    itr_type itr;                                                                        \
    HASH_FIND_BYHASHVALUE( hh, map, &key, sizeof( key ), hash, itr );                    \
    return itr;                                                                          \
  }                                                                                      \
                                                                                         \
  static void insert( map_type &map, const blueprint::key_type &key )                    \
  {                                                                                      \
    unsigned int hash = blueprint::hash_key( key );                                      \
                                                                                         \
    itr_type itr;                                                                        \
    HASH_FIND_BYHASHVALUE( hh, map, &key, sizeof( key ), hash, itr );                    \
                                                                                         \
    if( !itr )                                                                           \
    {                                                                                    \
      itr = (uthash_##blueprint##_bucket *)malloc( sizeof( uthash_##blueprint##_bucket ) ); \
      itr->key = key;                                                                    \
      HASH_ADD_BYHASHVALUE( hh, map, key, sizeof( key ), hash, itr );                    \
    }                                                                                    \
                                                                                         \
    itr->value = typename blueprint::value_type();                                       \
  }                                                                                      \
                                                                                         \
  static void erase( map_type &map, const blueprint::key_type &key )                     \
  {                                                                                      \
    unsigned int hash = blueprint::hash_key( key );                                      \
                                                                                         \
    itr_type itr;                                                                        \
    HASH_FIND_BYHASHVALUE( hh, map, &key, sizeof( key ), hash, itr );                    \
                                                                                         \
    if( itr )                                                                            \
    {                                                                                    \
      HASH_DEL( map, itr );                                                              \
      free( itr );                                                                       \
    }                                                                                    \
  }                                                                                      \
                                                                                         \
  static itr_type begin_itr( map_type &map )                                             \
  {                                                                                      \
    return map;                                                                          \
  }                                                                                      \
                                                                                         \
  static bool is_itr_valid( map_type &map, itr_type &itr )                               \
  {                                                                                      \
    return itr;                                                                          \
  }                                                                                      \
                                                                                         \
  static void increment_itr( map_type &map, itr_type &itr )                              \
  {                                                                                      \
    itr = (uthash_##blueprint##_bucket *)itr->hh.next;                                   \
  }                                                                                      \
                                                                                         \
  static const blueprint::key_type &get_key_from_itr( map_type &map, itr_type &itr )     \
  {                                                                                      \
    return itr->key;                                                                     \
  }                                                                                      \
                                                                                         \
  static const blueprint::value_type &get_value_from_itr( map_type &map, itr_type &itr ) \
  {                                                                                      \
    return itr->value;                                                                   \
  }                                                                                      \
                                                                                         \
  static void destroy_map( map_type &map )                                               \
  {                                                                                      \
    itr_type current;                                                                    \
    itr_type temp;                                                                       \
                                                                                         \
    HASH_ITER( hh, map, current, temp )                                                  \
    {                                                                                    \
        HASH_DEL( map, current );                                                        \
        free( current );                                                                 \
    }                                                                                    \
  }                                                                                      \
};                                                                                       \

#ifdef UINT32_UINT32_MURMUR_ENABLED

#undef HASH_KEYCMP
#define HASH_KEYCMP( a, b, len )                                                                                   \
!uint32_uint32_murmur::cmpr_keys( *(uint32_uint32_murmur::key_type *)(a), *(uint32_uint32_murmur::key_type *)(b) ) \

UTHASH_SPECIALIZATION( uint32_uint32_murmur )

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

#undef HASH_KEYCMP
#define HASH_KEYCMP( a, b, len )             \
!uint64_struct448_murmur::cmpr_keys(         \
  *(uint64_struct448_murmur::key_type *)(a), \
  *(uint64_struct448_murmur::key_type *)(b)  \
)                                            \

UTHASH_SPECIALIZATION( uint64_struct448_murmur )

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED


#undef HASH_KEYCMP
#define HASH_KEYCMP( a, b, len )                                                                                   \
!cstring_uint64_fnv1a::cmpr_keys( *(cstring_uint64_fnv1a::key_type *)(a), *(cstring_uint64_fnv1a::key_type *)(b) ) \

UTHASH_SPECIALIZATION( cstring_uint64_fnv1a )

#endif

