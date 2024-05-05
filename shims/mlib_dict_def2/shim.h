// hash_map_benchmarks/shims/mlib_dict/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#include <limits>

#define M_D1CT_OA_UPPER_BOUND MAX_LOAD_FACTOR
#include "mlib/m-dict.h"

template< typename > struct mlib_dict_def2
{
  static constexpr const char *label = "M*LIB dict";
  static constexpr const char *color = "rgb( 255, 0, 0 )";
};

#define MLIB_DICT_DEF2_SPECIALIZATION( blueprint )                                                              \
                                                                                                                   \
DICT_DEF2(                                                                                                      \
  mlib_dict_def2_##blueprint,                                                                                   \
  blueprint::key_type,                                                                                             \
  (                                                                                                                \
    HASH( blueprint::hash_key ),                                                                                   \
    EQUAL( blueprint::cmpr_keys ),                                                                                 \
    INIT( M_INIT_DEFAULT ),                                                                                        \
    INIT_SET( M_SET_DEFAULT ),                                                                                     \
    SET( M_SET_DEFAULT ),                                                                                          \
    CLEAR( M_NOTHING_DEFAULT )                                                                                     \
  ),                                                                                                               \
  blueprint::value_type,                                                                                           \
  M_POD_OPLIST                                                                                                     \
)                                                                                                                  \
                                                                                                                   \
typedef struct                                                                                                     \
{                                                                                                                  \
  mlib_dict_def2_##blueprint##_t map;                                                                           \
} mlib_dict_def2_##blueprint;                                                                                   \
                                                                                                                   \
typedef struct                                                                                                     \
{                                                                                                                  \
  mlib_dict_def2_##blueprint##_it_t itr;                                                                        \
} mlib_dict_def2_##blueprint##_itr;                                                                             \
\
\
\
  static inline void mlib_dict_def2_##blueprint##_get_itr(                    \
    const mlib_dict_def2_##blueprint##_t map,                                 \
    blueprint::key_type const key,                                            \
    mlib_dict_def2_##blueprint##_it_t itr                                     \
  )                                                                           \
  {                                                                           \
    M_D1CT_CONTRACT(map);                                                     \
    const m_index_t mask = map->mask;                                         \
    m_index_t hash = (m_index_t) blueprint::hash_key(key);                    \
    m_index_t p = hash & mask;                                                \
    m_index_t s = 1;                                                          \
    /* We are likely to find the correct bucket first */                      \
    while (true) {                                                            \
      if (M_LIKELY (hash == map->index[p].hash)) {                            \
        m_index_t d = map->index[p].index;                                    \
        if (M_LIKELY(d >=2 && blueprint::cmpr_keys(map->data[d].pair.key, key))) { \
          itr->dict = map;                                                     \
          itr->index = p;                                                     \
          return;                                                             \
        }                                                                     \
      }                                                                       \
      if (M_LIKELY (map->index[p].index == 0)) {                              \
        mlib_dict_def2_##blueprint##_it_end( itr, map );                      \
        return;                                                               \
      }                                                                       \
      p = (p + M_D1CT_OA_PROBING(s)) & mask;                                  \
    }                                                                         \
  }                                                                           \
\
\
\
template<> struct mlib_dict_def2< blueprint >                                                                   \
{                                                                                                                  \
  using map_type = mlib_dict_def2_##blueprint;                                                                  \
  using itr_type = mlib_dict_def2_##blueprint##_itr;                                                            \
                                                                                                                   \
  static map_type create_map()                                                                                     \
  {                                                                                                                \
    map_type map;                                                                                                  \
    mlib_dict_def2_##blueprint##_init( map.map );                                                               \
    return map;                                                                                                    \
  }                                                                                                                \
                                                                                                                   \
  static itr_type find( map_type &map, const blueprint::key_type &key )                                            \
  {                                                                                                                \
    mlib_dict_def2_##blueprint##_itr itr;                                                                          \
    mlib_dict_def2_##blueprint##_get_itr( map.map, key, itr.itr );                                                 \
    return itr;                                                                                                    \
  }                                                                                                                \
                                                                                                                   \
  static void insert( map_type &map, const blueprint::key_type &key )                                              \
  {                                                                                                                \
    mlib_dict_def2_##blueprint##_set_at( map.map, key, typename blueprint::value_type() );                      \
  }                                                                                                                \
                                                                                                                   \
  static void erase( map_type &map, const blueprint::key_type &key )                                               \
  {                                                                                                                \
    mlib_dict_def2_##blueprint##_erase( map.map, key );                                                         \
  }                                                                                                                \
                                                                                                                   \
  static itr_type begin_itr( map_type &map )                                                                       \
  {                                                                                                                \
    itr_type itr;                                                                                                  \
    mlib_dict_def2_##blueprint##_it( itr.itr, map.map );                                                        \
    return itr;                                                                                                    \
  }                                                                                                                \
                                                                                                                   \
  static bool is_itr_valid( map_type &map, itr_type &itr )                                                         \
  {                                                                                                                \
    return !mlib_dict_def2_##blueprint##_end_p( itr.itr );                                                      \
  }                                                                                                                \
                                                                                                                   \
  static void increment_itr( map_type &map, itr_type &itr )                                                        \
  {                                                                                                                \
    mlib_dict_def2_##blueprint##_next( itr.itr );                                                               \
  }                                                                                                                \
                                                                                                                   \
  static const blueprint::key_type &get_key_from_itr( map_type &map, itr_type &itr )                               \
  {                                                                                                                \
    return mlib_dict_def2_##blueprint##_ref( itr.itr )->key;                                                    \
  }                                                                                                                \
                                                                                                                   \
  static const blueprint::value_type &get_value_from_itr( map_type &map, itr_type &itr )                           \
  {                                                                                                                \
    return mlib_dict_def2_##blueprint##_ref( itr.itr )->value;                                                  \
  }                                                                                                                \
                                                                                                                   \
  static void destroy_map( map_type &map )                                                                         \
  {                                                                                                                \
    mlib_dict_def2_##blueprint##_clear( map.map );                                                              \
  }                                                                                                                \
};                                                                                                                 \

#ifdef UINT32_UINT32_MURMUR_ENABLED
MLIB_DICT_DEF2_SPECIALIZATION( uint32_uint32_murmur )
#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED
MLIB_DICT_DEF2_SPECIALIZATION( uint64_struct448_murmur )
#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED
MLIB_DICT_DEF2_SPECIALIZATION( cstring_uint64_fnv1a )
#endif
