// c_cpp_hash_tables_benchmark/shims/khash/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#include "khash.h"

// Override khash's hard-coded load factor.
#define __ac_HASH_UPPER MAX_LOAD_FACTOR

template< typename > struct khash
{
  static constexpr const char *label = "khash";
  static constexpr const char *color = "rgb( 140, 140, 140 )";
  static constexpr bool tombstone_like_mechanism = true;
};

#define KHASH_SPECIALIZATION( blueprint )                                                               \
                                                                                                        \
template<> struct khash< blueprint >                                                                    \
{                                                                                                       \
  static khash_t( blueprint ) *create_table()                                                           \
  {                                                                                                     \
    return kh_init( blueprint );                                                                        \
  }                                                                                                     \
                                                                                                        \
  static khiter_t find( khash_t( blueprint ) *&table, const blueprint::key_type &key )                  \
  {                                                                                                     \
    return kh_get( blueprint, table, key );                                                             \
  }                                                                                                     \
                                                                                                        \
  static void insert( khash_t( blueprint ) *&table, const blueprint::key_type &key )                    \
  {                                                                                                     \
    int ret;                                                                                            \
    khiter_t itr = kh_put( blueprint, table, key, &ret );                                               \
    kh_value( table, itr ) = blueprint::value_type();                                                   \
  }                                                                                                     \
                                                                                                        \
  static void erase( khash_t( blueprint ) *&table, const blueprint::key_type &key )                     \
  {                                                                                                     \
    khiter_t itr = kh_get( blueprint, table, key );                                                     \
    if( itr != kh_end( table ) )                                                                        \
      kh_del( blueprint, table, itr );                                                                  \
  }                                                                                                     \
                                                                                                        \
  static khiter_t begin_itr( khash_t( blueprint ) *&table )                                             \
  {                                                                                                     \
    return kh_begin( table );                                                                           \
  }                                                                                                     \
                                                                                                        \
  static bool is_itr_valid( khash_t( blueprint ) *&table, khiter_t &itr )                               \
  {                                                                                                     \
    return itr != kh_end( table );                                                                      \
  }                                                                                                     \
                                                                                                        \
  static void increment_itr( khash_t( blueprint ) *&table, khiter_t &itr )                              \
  {                                                                                                     \
    do                                                                                                  \
    {                                                                                                   \
      ++itr;                                                                                            \
    } while( itr != kh_end( table ) && !kh_exist( table, itr ) );                                       \
  }                                                                                                     \
                                                                                                        \
  static const blueprint::key_type &get_key_from_itr( khash_t( blueprint ) *&table, khiter_t &itr )     \
  {                                                                                                     \
    return kh_key( table, itr );                                                                        \
  }                                                                                                     \
                                                                                                        \
  static const blueprint::value_type &get_value_from_itr( khash_t( blueprint ) *&table, khiter_t &itr ) \
  {                                                                                                     \
    return kh_val( table, itr );                                                                        \
  }                                                                                                     \
                                                                                                        \
  static void destroy_table( khash_t( blueprint ) *&table )                                             \
  {                                                                                                     \
    kh_destroy( blueprint, table );                                                                     \
  }                                                                                                     \
};                                                                                                      \

#ifdef UINT32_UINT32_MURMUR_ENABLED

KHASH_INIT(
  uint32_uint32_murmur, uint32_uint32_murmur::key_type,
  uint32_uint32_murmur::value_type,
  true,
  uint32_uint32_murmur::hash_key,
  uint32_uint32_murmur::cmpr_keys
)

KHASH_SPECIALIZATION( uint32_uint32_murmur )

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

KHASH_INIT(
  uint64_struct448_murmur, uint64_struct448_murmur::key_type,
  uint64_struct448_murmur::value_type,
  true,
  uint64_struct448_murmur::hash_key,
  uint64_struct448_murmur::cmpr_keys
)

KHASH_SPECIALIZATION( uint64_struct448_murmur )

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED

KHASH_INIT(
  cstring_uint64_fnv1a, cstring_uint64_fnv1a::key_type,
  cstring_uint64_fnv1a::value_type,
  true,
  cstring_uint64_fnv1a::hash_key,
  cstring_uint64_fnv1a::cmpr_keys
)

KHASH_SPECIALIZATION( cstring_uint64_fnv1a )

#endif
