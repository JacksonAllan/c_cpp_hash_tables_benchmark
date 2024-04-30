// hash_stc/shims/stc/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

template< typename > struct stc_hmap
{
  static constexpr const char *label = "STC";
  static constexpr const char *color = "rgb( 235, 102, 107 )";
};

#define STC_HMAP_SPECIALIZATION( blueprint )                                                                        \
                                                                                                                    \
template<> struct stc_hmap< blueprint >                                                                             \
{                                                                                                                   \
  static stc_##blueprint##_map create_map()                                                                         \
  {                                                                                                                 \
    return stc_##blueprint##_map_init();                                                                            \
  }                                                                                                                 \
                                                                                                                    \
  static stc_##blueprint##_map_iter find( stc_##blueprint##_map &map, const blueprint::key_type &key )              \
  {                                                                                                                 \
    return stc_##blueprint##_map_find( &map, key );                                                                 \
  }                                                                                                                 \
                                                                                                                    \
  static void insert( stc_##blueprint##_map &map, const blueprint::key_type &key )                                  \
  {                                                                                                                 \
    stc_##blueprint##_map_insert_or_assign( &map, key, blueprint::value_type() );                                   \
  }                                                                                                                 \
                                                                                                                    \
  static void erase( stc_##blueprint##_map &map, const blueprint::key_type &key )                                   \
  {                                                                                                                 \
    stc_##blueprint##_map_erase( &map, key );                                                                       \
  }                                                                                                                 \
                                                                                                                    \
  static stc_##blueprint##_map_iter begin_itr( stc_##blueprint##_map &map )                                         \
  {                                                                                                                 \
    return stc_##blueprint##_map_begin( &map );                                                                     \
  }                                                                                                                 \
                                                                                                                    \
  static bool is_itr_valid( stc_##blueprint##_map &map, stc_##blueprint##_map_iter &itr )                           \
  {                                                                                                                 \
    return itr.ref;                                                                                                 \
  }                                                                                                                 \
                                                                                                                    \
  static void increment_itr( stc_##blueprint##_map &map, stc_##blueprint##_map_iter &itr )                          \
  {                                                                                                                 \
    stc_##blueprint##_map_next( &itr );                                                                             \
  }                                                                                                                 \
                                                                                                                    \
  static const blueprint::key_type &get_key_from_itr( stc_##blueprint##_map &map, stc_##blueprint##_map_iter &itr ) \
  {                                                                                                                 \
    return itr.ref->first;                                                                                          \
  }                                                                                                                 \
                                                                                                                    \
  static const blueprint::value_type &get_value_from_itr(                                                           \
    stc_##blueprint##_map &map,                                                                                     \
    stc_##blueprint##_map_iter &itr                                                                                 \
  )                                                                                                                 \
  {                                                                                                                 \
    return itr.ref->second;                                                                                         \
  }                                                                                                                 \
                                                                                                                    \
  static void destroy_map( stc_##blueprint##_map &map )                                                             \
  {                                                                                                                 \
    stc_##blueprint##_map_drop( &map );                                                                             \
  }                                                                                                                 \
};                                                                                                                  \

#ifdef UINT32_UINT32_MURMUR_ENABLED

#define i_type               stc_uint32_uint32_murmur_map
#define i_key                uint32_uint32_murmur::key_type
#define i_val                uint32_uint32_murmur::value_type
#define i_hash( key )        uint32_uint32_murmur::hash_key( *(key) )
#define i_eq( key_1, key_2 ) uint32_uint32_murmur::cmpr_keys( *(key_1), *(key_2) )
#define i_max_load_factor    MAX_LOAD_FACTOR
#include "stc/hmap.h"

STC_HMAP_SPECIALIZATION( uint32_uint32_murmur )

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

#define i_type               stc_uint64_struct448_murmur_map
#define i_key                uint64_struct448_murmur::key_type
#define i_val                uint64_struct448_murmur::value_type
#define i_hash( key )        uint64_struct448_murmur::hash_key( *(key) )
#define i_eq( key_1, key_2 ) uint64_struct448_murmur::cmpr_keys( *(key_1), *(key_2) )
#define i_max_load_factor    MAX_LOAD_FACTOR
#include "stc/hmap.h"

STC_HMAP_SPECIALIZATION( uint64_struct448_murmur )

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED
#define i_type               stc_cstring_uint64_fnv1a_map
#define i_key                cstring_uint64_fnv1a::key_type
#define i_val                cstring_uint64_fnv1a::value_type
#define i_hash( key )        cstring_uint64_fnv1a::hash_key( *(key) )
#define i_eq( key_1, key_2 ) cstring_uint64_fnv1a::cmpr_keys( *(key_1), *(key_2) )
#define i_max_load_factor    MAX_LOAD_FACTOR
#include "stc/hmap.h"

STC_HMAP_SPECIALIZATION( cstring_uint64_fnv1a )

#endif
