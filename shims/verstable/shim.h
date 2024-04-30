// hash_map_benchmarks/shims/verstable/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

template< typename > struct verstable
{
  static constexpr const char *label = "Verstable";
  static constexpr const char *color = "rgb( 236, 122, 191 )";
};

#define VERSTABLE_SPECIALIZATION( blueprint )                                                                     \
                                                                                                                  \
template<> struct verstable< blueprint >                                                                          \
{                                                                                                                 \
  static verstable_##blueprint##_map create_map()                                                                 \
  {                                                                                                               \
    verstable_##blueprint##_map map;                                                                              \
    verstable_##blueprint##_map_init( &map );                                                                     \
    return map;                                                                                                   \
  }                                                                                                               \
                                                                                                                  \
  static verstable_##blueprint##_map_itr find( verstable_##blueprint##_map &map, const blueprint::key_type &key ) \
  {                                                                                                               \
    return verstable_##blueprint##_map_get( &map, key );                                                          \
  }                                                                                                               \
                                                                                                                  \
  static void insert( verstable_##blueprint##_map &map, const blueprint::key_type &key )                          \
  {                                                                                                               \
    verstable_##blueprint##_map_insert( &map, key, blueprint::value_type() );                                     \
  }                                                                                                               \
                                                                                                                  \
  static void erase( verstable_##blueprint##_map &map, const blueprint::key_type &key )                           \
  {                                                                                                               \
    verstable_##blueprint##_map_erase( &map, key );                                                               \
  }                                                                                                               \
                                                                                                                  \
  static verstable_##blueprint##_map_itr begin_itr( verstable_##blueprint##_map &map )                            \
  {                                                                                                               \
    return verstable_##blueprint##_map_first( &map );                                                             \
  }                                                                                                               \
                                                                                                                  \
  static bool is_itr_valid( verstable_##blueprint##_map &map, verstable_##blueprint##_map_itr &itr )              \
  {                                                                                                               \
    return !verstable_##blueprint##_map_is_end( itr );                                                            \
  }                                                                                                               \
                                                                                                                  \
  static void increment_itr( verstable_##blueprint##_map &map, verstable_##blueprint##_map_itr &itr )             \
  {                                                                                                               \
    itr = verstable_##blueprint##_map_next( itr );                                                                \
  }                                                                                                               \
                                                                                                                  \
  static const blueprint::key_type &get_key_from_itr(                                                             \
    verstable_##blueprint##_map &map,                                                                             \
    verstable_##blueprint##_map_itr &itr                                                                          \
  )                                                                                                               \
  {                                                                                                               \
    return itr.data->key;                                                                                         \
  }                                                                                                               \
                                                                                                                  \
  static const blueprint::value_type &get_value_from_itr(                                                         \
    verstable_##blueprint##_map &map,                                                                             \
    verstable_##blueprint##_map_itr &itr                                                                          \
  )                                                                                                               \
  {                                                                                                               \
    return itr.data->val;                                                                                         \
  }                                                                                                               \
                                                                                                                  \
  static void destroy_map( verstable_##blueprint##_map &map )                                                     \
  {                                                                                                               \
    verstable_##blueprint##_map_cleanup( &map );                                                                  \
  }                                                                                                               \
};                                                                                                                \

#ifdef UINT32_UINT32_MURMUR_ENABLED

#define NAME     verstable_uint32_uint32_murmur_map
#define KEY_TY   uint32_uint32_murmur::key_type
#define VAL_TY   uint32_uint32_murmur::value_type
#define HASH_FN  uint32_uint32_murmur::hash_key
#define CMPR_FN  uint32_uint32_murmur::cmpr_keys
#define MAX_LOAD MAX_LOAD_FACTOR
#include "verstable.h"

VERSTABLE_SPECIALIZATION( uint32_uint32_murmur )

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

#define NAME     verstable_uint64_struct448_murmur_map
#define KEY_TY   uint64_struct448_murmur::key_type
#define VAL_TY   uint64_struct448_murmur::value_type
#define HASH_FN  uint64_struct448_murmur::hash_key
#define CMPR_FN  uint64_struct448_murmur::cmpr_keys
#define MAX_LOAD MAX_LOAD_FACTOR
#include "verstable.h"

VERSTABLE_SPECIALIZATION( uint64_struct448_murmur )

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED

#define NAME     verstable_cstring_uint64_fnv1a_map
#define KEY_TY   cstring_uint64_fnv1a::key_type
#define VAL_TY   cstring_uint64_fnv1a::value_type
#define HASH_FN  cstring_uint64_fnv1a::hash_key
#define CMPR_FN  cstring_uint64_fnv1a::cmpr_keys
#define MAX_LOAD MAX_LOAD_FACTOR
#include "verstable.h"

VERSTABLE_SPECIALIZATION( cstring_uint64_fnv1a )

#endif
