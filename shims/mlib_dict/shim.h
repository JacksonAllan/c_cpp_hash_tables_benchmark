// c_cpp_hash_tables_benchmark/shims/mlib_dict/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#include <limits>

#define M_D1CT_OA_UPPER_BOUND MAX_LOAD_FACTOR
#include "mlib/m-dict.h"

template< typename > struct mlib_dict
{
  static constexpr const char *label = "DICT";
  static constexpr const char *color = "rgb( 100, 166, 105 )";
  static constexpr bool tombstone_like_mechanism = true;
};

#define MLIB_DICT_SPECIALIZATION( blueprint )                                                \
                                                                                             \
DICT_DEF2(                                                                                   \
  mlib_dict_##blueprint,                                                                     \
  blueprint::key_type,                                                                       \
  (                                                                                          \
    HASH( blueprint::hash_key ),                                                             \
    EQUAL( blueprint::cmpr_keys ),                                                           \
    INIT( M_INIT_DEFAULT ),                                                                  \
    INIT_SET( M_SET_DEFAULT ),                                                               \
    SET( M_SET_DEFAULT ),                                                                    \
    CLEAR( M_NOTHING_DEFAULT )                                                               \
  ),                                                                                         \
  blueprint::value_type,                                                                     \
  M_POD_OPLIST                                                                               \
)                                                                                            \
                                                                                             \
typedef struct                                                                               \
{                                                                                            \
  mlib_dict_##blueprint##_t table;                                                           \
} mlib_dict_##blueprint;                                                                     \
                                                                                             \
typedef struct                                                                               \
{                                                                                            \
  mlib_dict_##blueprint##_it_t itr;                                                          \
} mlib_dict_##blueprint##_itr;                                                               \
                                                                                             \
                                                                                             \
/* M*LIB does not provide a lookup function that returns an iterator, and the pointer        \
   returned by _get cannot be converted into an iterator. Hence, I have implemented a        \
   _get_itr function that is identical to the library's _get, except that it returns an      \
   iterator. */                                                                              \
static inline void mlib_dict_##blueprint##_get_itr(                                          \
  const mlib_dict_##blueprint##_t map,                                                       \
  blueprint::key_type const key,                                                             \
  mlib_dict_##blueprint##_it_t itr                                                           \
)                                                                                            \
{                                                                                            \
  M_D1CT_CONTRACT(map);                                                                      \
  const m_index_t mask = map->mask;                                                          \
  m_index_t hash = (m_index_t) blueprint::hash_key(key);                                     \
  m_index_t p = hash & mask;                                                                 \
  m_index_t s = 1;                                                                           \
  /* We are likely to find the correct bucket first */                                       \
  while (true) {                                                                             \
    if (M_LIKELY (hash == map->index[p].hash)) {                                             \
      m_index_t d = map->index[p].index;                                                     \
      if (M_LIKELY(d >=2 && blueprint::cmpr_keys(map->data[d].pair.key, key))) {             \
        itr->dict = map;                                                                     \
        itr->index = p;                                                                      \
        return;                                                                              \
      }                                                                                      \
    }                                                                                        \
    if (M_LIKELY (map->index[p].index == 0)) {                                               \
      mlib_dict_##blueprint##_it_end( itr, map );                                            \
      return;                                                                                \
    }                                                                                        \
    p = (p + M_D1CT_OA_PROBING(s)) & mask;                                                   \
  }                                                                                          \
}                                                                                            \
                                                                                             \
template<> struct mlib_dict< blueprint >                                                     \
{                                                                                            \
  using table_type = mlib_dict_##blueprint;                                                  \
  using itr_type = mlib_dict_##blueprint##_itr;                                              \
                                                                                             \
  static table_type create_table()                                                           \
  {                                                                                          \
    table_type table;                                                                        \
    mlib_dict_##blueprint##_init( table.table );                                             \
    return table;                                                                            \
  }                                                                                          \
                                                                                             \
  static itr_type find( table_type &table, const blueprint::key_type &key )                  \
  {                                                                                          \
    mlib_dict_##blueprint##_itr itr;                                                         \
    mlib_dict_##blueprint##_get_itr( table.table, key, itr.itr );                            \
    return itr;                                                                              \
  }                                                                                          \
                                                                                             \
  static void insert( table_type &table, const blueprint::key_type &key )                    \
  {                                                                                          \
    mlib_dict_##blueprint##_set_at( table.table, key, typename blueprint::value_type() );    \
  }                                                                                          \
                                                                                             \
  static void erase( table_type &table, const blueprint::key_type &key )                     \
  {                                                                                          \
    mlib_dict_##blueprint##_erase( table.table, key );                                       \
  }                                                                                          \
                                                                                             \
  static itr_type begin_itr( table_type &table )                                             \
  {                                                                                          \
    itr_type itr;                                                                            \
    mlib_dict_##blueprint##_it( itr.itr, table.table );                                      \
    return itr;                                                                              \
  }                                                                                          \
                                                                                             \
  static bool is_itr_valid( table_type &table, itr_type &itr )                               \
  {                                                                                          \
    return !mlib_dict_##blueprint##_end_p( itr.itr );                                        \
  }                                                                                          \
                                                                                             \
  static void increment_itr( table_type &table, itr_type &itr )                              \
  {                                                                                          \
    mlib_dict_##blueprint##_next( itr.itr );                                                 \
  }                                                                                          \
                                                                                             \
  static const blueprint::key_type &get_key_from_itr( table_type &table, itr_type &itr )     \
  {                                                                                          \
    return mlib_dict_##blueprint##_ref( itr.itr )->key;                                      \
  }                                                                                          \
                                                                                             \
  static const blueprint::value_type &get_value_from_itr( table_type &table, itr_type &itr ) \
  {                                                                                          \
    return mlib_dict_##blueprint##_ref( itr.itr )->value;                                    \
  }                                                                                          \
                                                                                             \
  static void destroy_table( table_type &table )                                             \
  {                                                                                          \
    mlib_dict_##blueprint##_clear( table.table );                                            \
  }                                                                                          \
};                                                                                           \

#ifdef UINT32_UINT32_MURMUR_ENABLED
MLIB_DICT_SPECIALIZATION( uint32_uint32_murmur )
#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED
MLIB_DICT_SPECIALIZATION( uint64_struct448_murmur )
#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED
MLIB_DICT_SPECIALIZATION( cstring_uint64_fnv1a )
#endif
