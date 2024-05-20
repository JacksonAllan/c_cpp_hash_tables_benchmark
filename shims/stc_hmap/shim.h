// c_cpp_hash_tables_benchmark/shims/stc/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

template< typename > struct stc_hmap
{
  static constexpr const char *label = "STC";
  static constexpr const char *color = "rgb( 235, 102, 107 )";
  static constexpr bool tombstone_like_mechanism = false;
};

#define STC_HMAP_SPECIALIZATION( blueprint )                                                                 \
                                                                                                             \
template<> struct stc_hmap< blueprint >                                                                      \
{                                                                                                            \
  static stc_##blueprint##_table create_table()                                                              \
  {                                                                                                          \
    return stc_##blueprint##_table_init();                                                                   \
  }                                                                                                          \
                                                                                                             \
  static stc_##blueprint##_table_iter find( stc_##blueprint##_table &table, const blueprint::key_type &key ) \
  {                                                                                                          \
    return stc_##blueprint##_table_find( &table, key );                                                      \
  }                                                                                                          \
                                                                                                             \
  static void insert( stc_##blueprint##_table &table, const blueprint::key_type &key )                       \
  {                                                                                                          \
    stc_##blueprint##_table_insert_or_assign( &table, key, blueprint::value_type() );                        \
  }                                                                                                          \
                                                                                                             \
  static void erase( stc_##blueprint##_table &table, const blueprint::key_type &key )                        \
  {                                                                                                          \
    stc_##blueprint##_table_erase( &table, key );                                                            \
  }                                                                                                          \
                                                                                                             \
  static stc_##blueprint##_table_iter begin_itr( stc_##blueprint##_table &table )                            \
  {                                                                                                          \
    return stc_##blueprint##_table_begin( &table );                                                          \
  }                                                                                                          \
                                                                                                             \
  static bool is_itr_valid( stc_##blueprint##_table &table, stc_##blueprint##_table_iter &itr )              \
  {                                                                                                          \
    return itr.ref;                                                                                          \
  }                                                                                                          \
                                                                                                             \
  static void increment_itr( stc_##blueprint##_table &table, stc_##blueprint##_table_iter &itr )             \
  {                                                                                                          \
    stc_##blueprint##_table_next( &itr );                                                                    \
  }                                                                                                          \
                                                                                                             \
  static const blueprint::key_type &get_key_from_itr(                                                        \
    stc_##blueprint##_table &table,                                                                          \
    stc_##blueprint##_table_iter &itr                                                                        \
  )                                                                                                          \
  {                                                                                                          \
    return itr.ref->first;                                                                                   \
  }                                                                                                          \
                                                                                                             \
  static const blueprint::value_type &get_value_from_itr(                                                    \
    stc_##blueprint##_table &table,                                                                          \
    stc_##blueprint##_table_iter &itr                                                                        \
  )                                                                                                          \
  {                                                                                                          \
    return itr.ref->second;                                                                                  \
  }                                                                                                          \
                                                                                                             \
  static void destroy_table( stc_##blueprint##_table &table )                                                \
  {                                                                                                          \
    stc_##blueprint##_table_drop( &table );                                                                  \
  }                                                                                                          \
};                                                                                                           \

#ifdef UINT32_UINT32_MURMUR_ENABLED

#define i_type               stc_uint32_uint32_murmur_table
#define i_key                uint32_uint32_murmur::key_type
#define i_val                uint32_uint32_murmur::value_type
#define i_hash( key )        uint32_uint32_murmur::hash_key( *(key) )
#define i_eq( key_1, key_2 ) uint32_uint32_murmur::cmpr_keys( *(key_1), *(key_2) )
#define i_max_load_factor    MAX_LOAD_FACTOR
#include "stc/hmap.h"

STC_HMAP_SPECIALIZATION( uint32_uint32_murmur )

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

#define i_type               stc_uint64_struct448_murmur_table
#define i_key                uint64_struct448_murmur::key_type
#define i_val                uint64_struct448_murmur::value_type
#define i_hash( key )        uint64_struct448_murmur::hash_key( *(key) )
#define i_eq( key_1, key_2 ) uint64_struct448_murmur::cmpr_keys( *(key_1), *(key_2) )
#define i_max_load_factor    MAX_LOAD_FACTOR
#include "stc/hmap.h"

STC_HMAP_SPECIALIZATION( uint64_struct448_murmur )

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED
#define i_type               stc_cstring_uint64_fnv1a_table
#define i_key                cstring_uint64_fnv1a::key_type
#define i_val                cstring_uint64_fnv1a::value_type
#define i_hash( key )        cstring_uint64_fnv1a::hash_key( *(key) )
#define i_eq( key_1, key_2 ) cstring_uint64_fnv1a::cmpr_keys( *(key_1), *(key_2) )
#define i_max_load_factor    MAX_LOAD_FACTOR
#include "stc/hmap.h"

STC_HMAP_SPECIALIZATION( cstring_uint64_fnv1a )

#endif
