// hash_map_benchmarks/shims/verstable/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

template< typename > struct verstable
{
  static constexpr const char *label = "Verstable";
  static constexpr const char *color = "rgb( 236, 122, 191 )";
};

#ifdef UINT32_UINT32_MURMUR_ENABLED

#define NAME     verstable_uint32_uint32_murmur_map
#define KEY_TY   uint32_uint32_murmur::key_type
#define VAL_TY   uint32_uint32_murmur::value_type
#define HASH_FN  uint32_uint32_murmur::hash_key
#define CMPR_FN  uint32_uint32_murmur::cmpr_keys
#define MAX_LOAD MAX_LOAD_FACTOR
#include "verstable_dev.h"

template<> struct verstable< uint32_uint32_murmur >
{
  static verstable_uint32_uint32_murmur_map create_map()
  {
    verstable_uint32_uint32_murmur_map map;
    verstable_uint32_uint32_murmur_map_init( &map );
    return map;
  }

  static verstable_uint32_uint32_murmur_map_itr find(
    verstable_uint32_uint32_murmur_map &map,
    const uint32_uint32_murmur::key_type &key
  )
  {
    return verstable_uint32_uint32_murmur_map_get( &map, key );
  }

  static void insert( verstable_uint32_uint32_murmur_map &map, const uint32_uint32_murmur::key_type &key )
  {
    verstable_uint32_uint32_murmur_map_insert( &map, key, uint32_uint32_murmur::value_type() );
  }

  static void erase( verstable_uint32_uint32_murmur_map &map, const uint32_uint32_murmur::key_type &key )
  {
    verstable_uint32_uint32_murmur_map_erase( &map, key );
  }

  static verstable_uint32_uint32_murmur_map_itr begin_itr( verstable_uint32_uint32_murmur_map &map )
  {
    return verstable_uint32_uint32_murmur_map_first( &map );
  }

  static bool is_itr_valid( verstable_uint32_uint32_murmur_map &map, verstable_uint32_uint32_murmur_map_itr &itr )
  {
    return !verstable_uint32_uint32_murmur_map_is_end( itr );
  }

  static void increment_itr( verstable_uint32_uint32_murmur_map &map, verstable_uint32_uint32_murmur_map_itr &itr )
  {
    itr = verstable_uint32_uint32_murmur_map_next( itr );
  }

  static const uint32_uint32_murmur::key_type &get_key_from_itr(
    verstable_uint32_uint32_murmur_map &map,
    verstable_uint32_uint32_murmur_map_itr &itr
  )
  {
    return itr.data->key;
  }

  static const uint32_uint32_murmur::value_type &get_value_from_itr(
    verstable_uint32_uint32_murmur_map &map,
    verstable_uint32_uint32_murmur_map_itr &itr
  )
  {
    return itr.data->val;
  }

  static void destroy_map( verstable_uint32_uint32_murmur_map &map )
  {
    verstable_uint32_uint32_murmur_map_cleanup( &map );
  }
};

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

#define NAME     verstable_uint64_struct448_murmur_map
#define KEY_TY   uint64_struct448_murmur::key_type
#define VAL_TY   uint64_struct448_murmur::value_type
#define HASH_FN  uint64_struct448_murmur::hash_key
#define CMPR_FN  uint64_struct448_murmur::cmpr_keys
#define MAX_LOAD MAX_LOAD_FACTOR
#include "verstable_dev.h"

template<> struct verstable< uint64_struct448_murmur >
{
  static verstable_uint64_struct448_murmur_map create_map()
  {
    verstable_uint64_struct448_murmur_map map;
    verstable_uint64_struct448_murmur_map_init( &map );
    return map;
  }

  static verstable_uint64_struct448_murmur_map_itr find(
    verstable_uint64_struct448_murmur_map &map,
    const uint64_struct448_murmur::key_type &key
  )
  {
    return verstable_uint64_struct448_murmur_map_get( &map, key );
  }

  static void insert( verstable_uint64_struct448_murmur_map &map, const uint64_struct448_murmur::key_type &key )
  {
    verstable_uint64_struct448_murmur_map_insert( &map, key, uint64_struct448_murmur::value_type() );
  }

  static void erase( verstable_uint64_struct448_murmur_map &map, const uint64_struct448_murmur::key_type &key )
  {
    verstable_uint64_struct448_murmur_map_erase( &map, key );
  }

  static verstable_uint64_struct448_murmur_map_itr begin_itr( verstable_uint64_struct448_murmur_map &map )
  {
    return verstable_uint64_struct448_murmur_map_first( &map );
  }

  static bool is_itr_valid( verstable_uint64_struct448_murmur_map &map, verstable_uint64_struct448_murmur_map_itr &itr )
  {
    return !verstable_uint64_struct448_murmur_map_is_end( itr );
  }

  static void increment_itr(
    verstable_uint64_struct448_murmur_map &map,
    verstable_uint64_struct448_murmur_map_itr &itr
  )
  {
    itr = verstable_uint64_struct448_murmur_map_next( itr );
  }

  static const uint64_struct448_murmur::key_type &get_key_from_itr(
    verstable_uint64_struct448_murmur_map &map,
    verstable_uint64_struct448_murmur_map_itr &itr
  )
  {
    return itr.data->key;
  }

  static const uint64_struct448_murmur::value_type &get_value_from_itr(
    verstable_uint64_struct448_murmur_map &map,
    verstable_uint64_struct448_murmur_map_itr &itr
  )
  {
    return itr.data->val;
  }

  static void destroy_map( verstable_uint64_struct448_murmur_map &map )
  {
    verstable_uint64_struct448_murmur_map_cleanup( &map );
  }
};

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED

#define NAME     verstable_cstring_uint64_fnv1a_map
#define KEY_TY   cstring_uint64_fnv1a::key_type
#define VAL_TY   cstring_uint64_fnv1a::value_type
#define HASH_FN  cstring_uint64_fnv1a::hash_key
#define CMPR_FN  cstring_uint64_fnv1a::cmpr_keys
#define MAX_LOAD MAX_LOAD_FACTOR
#include "verstable_dev.h"

template<> struct verstable< cstring_uint64_fnv1a >
{
  static verstable_cstring_uint64_fnv1a_map create_map()
  {
    verstable_cstring_uint64_fnv1a_map map;
    verstable_cstring_uint64_fnv1a_map_init( &map );
    return map;
  }

  static verstable_cstring_uint64_fnv1a_map_itr find(
    verstable_cstring_uint64_fnv1a_map &map,
    const cstring_uint64_fnv1a::key_type &key
  )
  {
    return verstable_cstring_uint64_fnv1a_map_get( &map, key );
  }

  static void insert( verstable_cstring_uint64_fnv1a_map &map, const cstring_uint64_fnv1a::key_type &key )
  {
    verstable_cstring_uint64_fnv1a_map_insert( &map, key, cstring_uint64_fnv1a::value_type() );
  }

  static void erase( verstable_cstring_uint64_fnv1a_map &map, const cstring_uint64_fnv1a::key_type &key )
  {
    verstable_cstring_uint64_fnv1a_map_erase( &map, key );
  }

  static verstable_cstring_uint64_fnv1a_map_itr begin_itr( verstable_cstring_uint64_fnv1a_map &map )
  {
    return verstable_cstring_uint64_fnv1a_map_first( &map );
  }

  static bool is_itr_valid( verstable_cstring_uint64_fnv1a_map &map, verstable_cstring_uint64_fnv1a_map_itr &itr )
  {
    return !verstable_cstring_uint64_fnv1a_map_is_end( itr );
  }

  static void increment_itr(
    verstable_cstring_uint64_fnv1a_map &map,
    verstable_cstring_uint64_fnv1a_map_itr &itr
  )
  {
    itr = verstable_cstring_uint64_fnv1a_map_next( itr );
  }

  static const cstring_uint64_fnv1a::key_type &get_key_from_itr(
    verstable_cstring_uint64_fnv1a_map &map,
    verstable_cstring_uint64_fnv1a_map_itr &itr
  )
  {
    return itr.data->key;
  }

  static const cstring_uint64_fnv1a::value_type &get_value_from_itr(
    verstable_cstring_uint64_fnv1a_map &map,
    verstable_cstring_uint64_fnv1a_map_itr &itr
  )
  {
    return itr.data->val;
  }

  static void destroy_map( verstable_cstring_uint64_fnv1a_map &map )
  {
    verstable_cstring_uint64_fnv1a_map_cleanup( &map );
  }
};

#endif
