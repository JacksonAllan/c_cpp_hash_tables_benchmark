// c_cpp_hash_tables_benchmark/shims/verstable/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

template< typename > struct verstable
{
  static constexpr const char *label = "Verstable";
  static constexpr const char *color = "rgb( 236, 122, 191 )";
  static constexpr bool tombstone_like_mechanism = false;
};

#define VERSTABLE_SPECIALIZATION( blueprint )                                                               \
                                                                                                            \
template<> struct verstable< blueprint >                                                                    \
{                                                                                                           \
  static verstable_##blueprint##_table create_table()                                                       \
  {                                                                                                         \
    verstable_##blueprint##_table table;                                                                    \
    verstable_##blueprint##_table_init( &table );                                                           \
    return table;                                                                                           \
  }                                                                                                         \
                                                                                                            \
  static verstable_##blueprint##_table_itr find(                                                            \
    verstable_##blueprint##_table &table,                                                                   \
    const blueprint::key_type &key                                                                          \
  )                                                                                                         \
  {                                                                                                         \
    return verstable_##blueprint##_table_get( &table, key );                                                \
  }                                                                                                         \
                                                                                                            \
  static void insert( verstable_##blueprint##_table &table, const blueprint::key_type &key )                \
  {                                                                                                         \
    verstable_##blueprint##_table_insert( &table, key, blueprint::value_type() );                           \
  }                                                                                                         \
                                                                                                            \
  static void erase( verstable_##blueprint##_table &table, const blueprint::key_type &key )                 \
  {                                                                                                         \
    verstable_##blueprint##_table_erase( &table, key );                                                     \
  }                                                                                                         \
                                                                                                            \
  static verstable_##blueprint##_table_itr begin_itr( verstable_##blueprint##_table &table )                \
  {                                                                                                         \
    return verstable_##blueprint##_table_first( &table );                                                   \
  }                                                                                                         \
                                                                                                            \
  static bool is_itr_valid( verstable_##blueprint##_table &table, verstable_##blueprint##_table_itr &itr )  \
  {                                                                                                         \
    return !verstable_##blueprint##_table_is_end( itr );                                                    \
  }                                                                                                         \
                                                                                                            \
  static void increment_itr( verstable_##blueprint##_table &table, verstable_##blueprint##_table_itr &itr ) \
  {                                                                                                         \
    itr = verstable_##blueprint##_table_next( itr );                                                        \
  }                                                                                                         \
                                                                                                            \
  static const blueprint::key_type &get_key_from_itr(                                                       \
    verstable_##blueprint##_table &table,                                                                   \
    verstable_##blueprint##_table_itr &itr                                                                  \
  )                                                                                                         \
  {                                                                                                         \
    return itr.data->key;                                                                                   \
  }                                                                                                         \
                                                                                                            \
  static const blueprint::value_type &get_value_from_itr(                                                   \
    verstable_##blueprint##_table &table,                                                                   \
    verstable_##blueprint##_table_itr &itr                                                                  \
  )                                                                                                         \
  {                                                                                                         \
    return itr.data->val;                                                                                   \
  }                                                                                                         \
                                                                                                            \
  static void destroy_table( verstable_##blueprint##_table &table )                                         \
  {                                                                                                         \
    verstable_##blueprint##_table_cleanup( &table );                                                        \
  }                                                                                                         \
};                                                                                                          \

#ifdef UINT32_UINT32_MURMUR_ENABLED

#define NAME     verstable_uint32_uint32_murmur_table
#define KEY_TY   uint32_uint32_murmur::key_type
#define VAL_TY   uint32_uint32_murmur::value_type
#define HASH_FN  uint32_uint32_murmur::hash_key
#define CMPR_FN  uint32_uint32_murmur::cmpr_keys
#define MAX_LOAD MAX_LOAD_FACTOR
#include "verstable.h"

VERSTABLE_SPECIALIZATION( uint32_uint32_murmur )

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

#define NAME     verstable_uint64_struct448_murmur_table
#define KEY_TY   uint64_struct448_murmur::key_type
#define VAL_TY   uint64_struct448_murmur::value_type
#define HASH_FN  uint64_struct448_murmur::hash_key
#define CMPR_FN  uint64_struct448_murmur::cmpr_keys
#define MAX_LOAD MAX_LOAD_FACTOR
#include "verstable.h"

VERSTABLE_SPECIALIZATION( uint64_struct448_murmur )

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED

#define NAME     verstable_cstring_uint64_fnv1a_table
#define KEY_TY   cstring_uint64_fnv1a::key_type
#define VAL_TY   cstring_uint64_fnv1a::value_type
#define HASH_FN  cstring_uint64_fnv1a::hash_key
#define CMPR_FN  cstring_uint64_fnv1a::cmpr_keys
#define MAX_LOAD MAX_LOAD_FACTOR
#include "verstable.h"

VERSTABLE_SPECIALIZATION( cstring_uint64_fnv1a )

#endif
