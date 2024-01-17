// hash_stc/shims/stc/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

template< typename > struct stc_hmap
{
  static constexpr const char *label = "STC hmap";
  static constexpr const char *color = "rgb( 13, 127, 161 )";
};

#ifdef UINT32_UINT32_MURMUR_ENABLED

#define i_type               stc_uint32_uint32_murmur_map
#define i_key                uint32_uint32_murmur::key_type
#define i_val                uint32_uint32_murmur::value_type
#define i_hash( key )        uint32_uint32_murmur::hash_key( *(key) )
#define i_eq( key_1, key_2 ) uint32_uint32_murmur::cmpr_keys( *(key_1), *(key_2) )
#define i_max_load_factor    MAX_LOAD_FACTOR
#include "stc/hmap.h"

template<> struct stc_hmap< uint32_uint32_murmur >
{
  static stc_uint32_uint32_murmur_map create_map()
  {
    return stc_uint32_uint32_murmur_map_init();
  }

  static stc_uint32_uint32_murmur_map_iter find(
    stc_uint32_uint32_murmur_map &map,
    const uint32_uint32_murmur::key_type &key
  )
  {
    return stc_uint32_uint32_murmur_map_find( &map, key );
  }

  static void insert( stc_uint32_uint32_murmur_map &map, const uint32_uint32_murmur::key_type &key )
  {
    stc_uint32_uint32_murmur_map_insert_or_assign( &map, key, uint32_uint32_murmur::value_type() );
  }

  static void erase( stc_uint32_uint32_murmur_map &map, const uint32_uint32_murmur::key_type &key )
  {
    stc_uint32_uint32_murmur_map_erase( &map, key );
  }

  static stc_uint32_uint32_murmur_map_iter begin_itr( stc_uint32_uint32_murmur_map &map )
  {
    return stc_uint32_uint32_murmur_map_begin( &map );
  }

  static bool is_itr_valid( stc_uint32_uint32_murmur_map &map, stc_uint32_uint32_murmur_map_iter &itr )
  {
    return itr.ref;
  }

  static void increment_itr( stc_uint32_uint32_murmur_map &map, stc_uint32_uint32_murmur_map_iter &itr )
  {
    stc_uint32_uint32_murmur_map_next( &itr );
  }

  static const uint32_uint32_murmur::key_type &get_key_from_itr(
    stc_uint32_uint32_murmur_map &map,
    stc_uint32_uint32_murmur_map_iter &itr
  )
  {
    return itr.ref->first;
  }

  static const uint32_uint32_murmur::value_type &get_value_from_itr(
    stc_uint32_uint32_murmur_map &map,
    stc_uint32_uint32_murmur_map_iter &itr
  )
  {
    return itr.ref->second;
  }

  static void destroy_map( stc_uint32_uint32_murmur_map &map )
  {
    stc_uint32_uint32_murmur_map_drop( &map );
  }
};

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

#define i_type               stc_uint64_struct448_murmur_map
#define i_key                uint64_struct448_murmur::key_type
#define i_val                uint64_struct448_murmur::value_type
#define i_hash( key )        uint64_struct448_murmur::hash_key( *(key) )
#define i_eq( key_1, key_2 ) uint64_struct448_murmur::cmpr_keys( *(key_1), *(key_2) )
#define i_max_load_factor    MAX_LOAD_FACTOR
#include "stc/hmap.h"

template<> struct stc_hmap< uint64_struct448_murmur >
{
  static stc_uint64_struct448_murmur_map create_map()
  {
    return stc_uint64_struct448_murmur_map_init();
  }

  static stc_uint64_struct448_murmur_map_iter find(
    stc_uint64_struct448_murmur_map &map,
    const uint64_struct448_murmur::key_type &key
  )
  {
    return stc_uint64_struct448_murmur_map_find( &map, key );
  }

  static void insert( stc_uint64_struct448_murmur_map &map, const uint64_struct448_murmur::key_type &key )
  {
    stc_uint64_struct448_murmur_map_insert_or_assign( &map, key, uint64_struct448_murmur::value_type() );
  }

  static void erase( stc_uint64_struct448_murmur_map &map, const uint64_struct448_murmur::key_type &key )
  {
    stc_uint64_struct448_murmur_map_erase( &map, key );
  }

  static stc_uint64_struct448_murmur_map_iter begin_itr( stc_uint64_struct448_murmur_map &map )
  {
    return stc_uint64_struct448_murmur_map_begin( &map );
  }

  static bool is_itr_valid( stc_uint64_struct448_murmur_map &map, stc_uint64_struct448_murmur_map_iter &itr )
  {
    return itr.ref;
  }

  static void increment_itr( stc_uint64_struct448_murmur_map &map, stc_uint64_struct448_murmur_map_iter &itr )
  {
    stc_uint64_struct448_murmur_map_next( &itr );
  }

  static const uint64_struct448_murmur::key_type &get_key_from_itr(
    stc_uint64_struct448_murmur_map &map,
    stc_uint64_struct448_murmur_map_iter &itr
  )
  {
    return itr.ref->first;
  }

  static const uint64_struct448_murmur::value_type &get_value_from_itr(
    stc_uint64_struct448_murmur_map &map,
    stc_uint64_struct448_murmur_map_iter &itr
  )
  {
    return itr.ref->second;
  }

  static void destroy_map( stc_uint64_struct448_murmur_map &map )
  {
    stc_uint64_struct448_murmur_map_drop( &map );
  }
};

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED

#define i_type               stc_cstring_uint64_fnv1a_map
#define i_key                cstring_uint64_fnv1a::key_type
#define i_val                cstring_uint64_fnv1a::value_type
#define i_hash( key )        cstring_uint64_fnv1a::hash_key( *(key) )
#define i_eq( key_1, key_2 ) cstring_uint64_fnv1a::cmpr_keys( *(key_1), *(key_2) )
#define i_max_load_factor    MAX_LOAD_FACTOR
#include "stc/hmap.h"

template<> struct stc_hmap< cstring_uint64_fnv1a >
{
  static stc_cstring_uint64_fnv1a_map create_map()
  {
    return stc_cstring_uint64_fnv1a_map_init();
  }

  static stc_cstring_uint64_fnv1a_map_iter find(
    stc_cstring_uint64_fnv1a_map &map,
    const cstring_uint64_fnv1a::key_type &key
  )
  {
    return stc_cstring_uint64_fnv1a_map_find( &map, key );
  }

  static void insert( stc_cstring_uint64_fnv1a_map &map, const cstring_uint64_fnv1a::key_type &key )
  {
    stc_cstring_uint64_fnv1a_map_insert_or_assign( &map, key, cstring_uint64_fnv1a::value_type() );
  }

  static void erase( stc_cstring_uint64_fnv1a_map &map, const cstring_uint64_fnv1a::key_type &key )
  {
    stc_cstring_uint64_fnv1a_map_erase( &map, key );
  }

  static stc_cstring_uint64_fnv1a_map_iter begin_itr( stc_cstring_uint64_fnv1a_map &map )
  {
    return stc_cstring_uint64_fnv1a_map_begin( &map );
  }

  static bool is_itr_valid( stc_cstring_uint64_fnv1a_map &map, stc_cstring_uint64_fnv1a_map_iter &itr )
  {
    return itr.ref;
  }

  static void increment_itr( stc_cstring_uint64_fnv1a_map &map, stc_cstring_uint64_fnv1a_map_iter &itr )
  {
    stc_cstring_uint64_fnv1a_map_next( &itr );
  }

  static const cstring_uint64_fnv1a::key_type &get_key_from_itr(
    stc_cstring_uint64_fnv1a_map &map,
    stc_cstring_uint64_fnv1a_map_iter &itr
  )
  {
    return itr.ref->first;
  }

  static const cstring_uint64_fnv1a::value_type &get_value_from_itr(
    stc_cstring_uint64_fnv1a_map &map,
    stc_cstring_uint64_fnv1a_map_iter &itr
  )
  {
    return itr.ref->second;
  }

  static void destroy_map( stc_cstring_uint64_fnv1a_map &map )
  {
    stc_cstring_uint64_fnv1a_map_drop( &map );
  }
};

#endif
