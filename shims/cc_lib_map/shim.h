// c_cpp_hash_tables_benchmark/shims/cc/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#define CC_NO_SHORT_NAMES
#include "cc.h"

template< typename > struct cc_lib_map
{
  static constexpr const char *label = "CC";
  static constexpr const char *color = "rgb( 174, 97, 175 )";
  static constexpr bool tombstone_like_mechanism = false;
};

#define CC_SPECIALIZATION( blueprint )                                                                     \
template<> struct cc_lib_map< blueprint >                                                                  \
{                                                                                                          \
  using table_type = cc_map( cc_##blueprint##_key, blueprint::value_type );                                \
                                                                                                           \
  static table_type create_table()                                                                         \
  {                                                                                                        \
    table_type table;                                                                                      \
    cc_init( &table );                                                                                     \
    return table;                                                                                          \
  }                                                                                                        \
                                                                                                           \
  static blueprint::value_type *find( table_type &table, const blueprint::key_type &key )                  \
  {                                                                                                        \
    return cc_get( &table, cc_##blueprint##_key{ key } );                                                  \
  }                                                                                                        \
                                                                                                           \
  static void insert( table_type &table, const blueprint::key_type &key )                                  \
  {                                                                                                        \
    cc_insert( &table, cc_##blueprint##_key{ key }, typename blueprint::value_type() );                    \
  }                                                                                                        \
                                                                                                           \
  static void erase( table_type &table, const blueprint::key_type &key )                                   \
  {                                                                                                        \
    cc_erase( &table, cc_##blueprint##_key{ key } );                                                       \
  }                                                                                                        \
                                                                                                           \
  static blueprint::value_type *begin_itr( table_type &table )                                             \
  {                                                                                                        \
    return cc_first( &table );                                                                             \
  }                                                                                                        \
                                                                                                           \
  static bool is_itr_valid( table_type &table, blueprint::value_type *&itr )                               \
  {                                                                                                        \
    return itr && itr != cc_end( &table );                                                                 \
  }                                                                                                        \
                                                                                                           \
  static void increment_itr( table_type &table, blueprint::value_type *&itr )                              \
  {                                                                                                        \
    itr = cc_next( &table, itr );                                                                          \
  }                                                                                                        \
                                                                                                           \
  static const blueprint::key_type &get_key_from_itr( table_type &table, blueprint::value_type *&itr )     \
  {                                                                                                        \
    return cc_key_for( &table, itr )->data;                                                                \
  }                                                                                                        \
                                                                                                           \
  static const blueprint::value_type &get_value_from_itr( table_type &table, blueprint::value_type *&itr ) \
  {                                                                                                        \
    return *itr;                                                                                           \
  }                                                                                                        \
                                                                                                           \
  static void destroy_table( table_type &table )                                                           \
  {                                                                                                        \
    cc_cleanup( &table );                                                                                  \
  }                                                                                                        \
};                                                                                                         \

#ifdef UINT32_UINT32_MURMUR_ENABLED

struct cc_uint32_uint32_murmur_key{ uint32_uint32_murmur::key_type data; };

#define CC_HASH cc_uint32_uint32_murmur_key, { return uint32_uint32_murmur::hash_key( val.data ); }
#define CC_CMPR cc_uint32_uint32_murmur_key, { return !uint32_uint32_murmur::cmpr_keys( val_1.data, val_2.data ); }
#define CC_LOAD cc_uint32_uint32_murmur_key, MAX_LOAD_FACTOR
#include "cc.h"

CC_SPECIALIZATION( uint32_uint32_murmur )

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

struct cc_uint64_struct448_murmur_key{ uint64_struct448_murmur::key_type data; };

#define CC_HASH cc_uint64_struct448_murmur_key, { return uint64_struct448_murmur::hash_key( val.data ); }
#define CC_CMPR cc_uint64_struct448_murmur_key, { return !uint64_struct448_murmur::cmpr_keys( val_1.data, val_2.data ); }
#define CC_LOAD cc_uint64_struct448_murmur_key, MAX_LOAD_FACTOR
#include "cc.h"

CC_SPECIALIZATION( uint64_struct448_murmur )

#endif


#ifdef CSTRING_UINT64_FNV1A_ENABLED

struct cc_cstring_uint64_fnv1a_key{ cstring_uint64_fnv1a::key_type data; };

#define CC_HASH cc_cstring_uint64_fnv1a_key, { return cstring_uint64_fnv1a::hash_key( val.data ); }
#define CC_CMPR cc_cstring_uint64_fnv1a_key, { return !cstring_uint64_fnv1a::cmpr_keys( val_1.data, val_2.data ); }
#define CC_LOAD cc_cstring_uint64_fnv1a_key, MAX_LOAD_FACTOR
#include "cc.h"

CC_SPECIALIZATION( cstring_uint64_fnv1a )

#endif
