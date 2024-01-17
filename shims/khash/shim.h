// hash_map_benchmarks/shims/khash/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#include "khash.h"

// Override khash's hard-coded load factor.
#define __ac_HASH_UPPER MAX_LOAD_FACTOR

template< typename > struct khash
{
  static constexpr const char *label = "khash";
  static constexpr const char *color = "rgb( 145, 99, 127 )";
};

#ifdef UINT32_UINT32_MURMUR_ENABLED

KHASH_INIT(
  uint32_uint32_murmur, uint32_uint32_murmur::key_type,
  uint32_uint32_murmur::value_type,
  true,
  uint32_uint32_murmur::hash_key,
  uint32_uint32_murmur::cmpr_keys
)

template<> struct khash< uint32_uint32_murmur >
{
  static khash_t( uint32_uint32_murmur ) *create_map()
  {
    return kh_init( uint32_uint32_murmur );
  }

  static khiter_t find( khash_t( uint32_uint32_murmur ) *&map, const uint32_uint32_murmur::key_type &key )
  {
    return kh_get( uint32_uint32_murmur, map, key );
  }

  static void insert( khash_t( uint32_uint32_murmur ) *&map, const uint32_uint32_murmur::key_type &key )
  {
    int ret;
    khiter_t itr = kh_put( uint32_uint32_murmur, map, key, &ret );
    kh_value( map, itr ) = uint32_uint32_murmur::value_type();
  }

  static void erase( khash_t( uint32_uint32_murmur ) *&map, const uint32_uint32_murmur::key_type &key )
  {
    khiter_t itr = kh_get( uint32_uint32_murmur, map, key );
    if( itr != kh_end( map ) )
      kh_del( uint32_uint32_murmur, map, itr );
  }

  static khiter_t begin_itr( khash_t( uint32_uint32_murmur ) *&map )
  {
    return kh_begin( map );
  }

  static bool is_itr_valid( khash_t( uint32_uint32_murmur ) *&map, khiter_t &itr )
  {
    return itr != kh_end( map );
  }

  static void increment_itr( khash_t( uint32_uint32_murmur ) *&map, khiter_t &itr )
  {
    do
    {
      ++itr;
    } while( itr != kh_end( map ) && !kh_exist( map, itr ) );
  }

  static const uint32_uint32_murmur::key_type &get_key_from_itr( khash_t( uint32_uint32_murmur ) *&map, khiter_t &itr )
  {
    return kh_key( map, itr );
  }

  static const uint32_uint32_murmur::value_type &get_value_from_itr(
    khash_t( uint32_uint32_murmur ) *&map,
    khiter_t &itr
  )
  {
    return kh_val( map, itr );
  }

  static void destroy_map( khash_t( uint32_uint32_murmur ) *&map )
  {
    kh_destroy( uint32_uint32_murmur, map );
  }
};

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

KHASH_INIT(
  uint64_struct448_murmur, uint64_struct448_murmur::key_type,
  uint64_struct448_murmur::value_type,
  true,
  uint64_struct448_murmur::hash_key,
  uint64_struct448_murmur::cmpr_keys
)

template<> struct khash< uint64_struct448_murmur >
{
  static khash_t( uint64_struct448_murmur ) *create_map()
  {
    return kh_init( uint64_struct448_murmur );
  }

  static khiter_t find( khash_t( uint64_struct448_murmur ) *&map, const uint64_struct448_murmur::key_type &key )
  {
    return kh_get( uint64_struct448_murmur, map, key );
  }

  static void insert( khash_t( uint64_struct448_murmur ) *&map, const uint64_struct448_murmur::key_type &key )
  {
    int ret;
    khiter_t itr = kh_put( uint64_struct448_murmur, map, key, &ret );
    kh_value( map, itr ) = uint64_struct448_murmur::value_type();
  }

  static void erase( khash_t( uint64_struct448_murmur ) *&map, const uint64_struct448_murmur::key_type &key )
  {
    khiter_t itr = kh_get( uint64_struct448_murmur, map, key );
    if( itr != kh_end( map ) )
      kh_del( uint64_struct448_murmur, map, itr );
  }

  static khiter_t begin_itr( khash_t( uint64_struct448_murmur ) *&map )
  {
    return kh_begin( map );
  }

  static bool is_itr_valid( khash_t( uint64_struct448_murmur ) *&map, khiter_t &itr )
  {
    return itr != kh_end( map );
  }

  static void increment_itr( khash_t( uint64_struct448_murmur ) *&map, khiter_t &itr )
  {
    do
    {
      ++itr;
    } while( itr != kh_end( map ) && !kh_exist( map, itr ) );
  }

  static const uint64_struct448_murmur::key_type &get_key_from_itr(
    khash_t( uint64_struct448_murmur ) *&map,
    khiter_t &itr
  )
  {
    return kh_key( map, itr );
  }

  static const uint64_struct448_murmur::value_type &get_value_from_itr(
    khash_t( uint64_struct448_murmur ) *&map,
    khiter_t &itr
  )
  {
    return kh_val( map, itr );
  }

  static void destroy_map( khash_t( uint64_struct448_murmur ) *&map )
  {
    kh_destroy( uint64_struct448_murmur, map );
  }
};

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED

KHASH_INIT(
  cstring_uint64_fnv1a, cstring_uint64_fnv1a::key_type,
  cstring_uint64_fnv1a::value_type,
  true,
  cstring_uint64_fnv1a::hash_key,
  cstring_uint64_fnv1a::cmpr_keys
)

template<> struct khash< cstring_uint64_fnv1a >
{
  static khash_t( cstring_uint64_fnv1a ) *create_map()
  {
    return kh_init( cstring_uint64_fnv1a );
  }

  static khiter_t find( khash_t( cstring_uint64_fnv1a ) *&map, const cstring_uint64_fnv1a::key_type &key )
  {
    return kh_get( cstring_uint64_fnv1a, map, key );
  }

  static void insert( khash_t( cstring_uint64_fnv1a ) *&map, const cstring_uint64_fnv1a::key_type &key )
  {
    int ret;
    khiter_t itr = kh_put( cstring_uint64_fnv1a, map, key, &ret );
    kh_value( map, itr ) = cstring_uint64_fnv1a::value_type();
  }

  static void erase( khash_t( cstring_uint64_fnv1a ) *&map, const cstring_uint64_fnv1a::key_type &key )
  {
    khiter_t itr = kh_get( cstring_uint64_fnv1a, map, key );
    if( itr != kh_end( map ) )
      kh_del( cstring_uint64_fnv1a, map, itr );
  }

  static khiter_t begin_itr( khash_t( cstring_uint64_fnv1a ) *&map )
  {
    return kh_begin( map );
  }

  static bool is_itr_valid( khash_t( cstring_uint64_fnv1a ) *&map, khiter_t &itr )
  {
    return itr != kh_end( map );
  }

  static void increment_itr( khash_t( cstring_uint64_fnv1a ) *&map, khiter_t &itr )
  {
    do
    {
      ++itr;
    } while( itr != kh_end( map ) && !kh_exist( map, itr ) );
  }

  static const cstring_uint64_fnv1a::key_type &get_key_from_itr( khash_t( cstring_uint64_fnv1a ) *&map, khiter_t &itr )
  {
    return kh_key( map, itr );
  }

  static const cstring_uint64_fnv1a::value_type &get_value_from_itr(
    khash_t( cstring_uint64_fnv1a ) *&map,
    khiter_t &itr
  )
  {
    return kh_val( map, itr );
  }

  static void destroy_map( khash_t( cstring_uint64_fnv1a ) *&map )
  {
    kh_destroy( cstring_uint64_fnv1a, map );
  }
};

#endif
