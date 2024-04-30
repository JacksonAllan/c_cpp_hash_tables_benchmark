// hash_map_benchmarks/shims/mlib_dict_oa_def2/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#include <limits>

#define M_D1CT_OA_UPPER_BOUND MAX_LOAD_FACTOR
#include "mlib/m-dict.h"

template< typename > struct mlib_dict_oa_def2
{
  static constexpr const char *label = "M*LIB";
  static constexpr const char *color = "rgb( 143, 215, 116 )";
};

#define MLIB_DICT_OA_DEF2_SPECIALIZATION( blueprint )                                                              \
                                                                                                                   \
DICT_OA_DEF2(                                                                                                      \
  mlib_dict_oa_def2_##blueprint,                                                                                   \
  blueprint::key_type,                                                                                             \
  (                                                                                                                \
    OOR_EQUAL( mlib_dict_oa_def2_##blueprint##_oor_equal ),                                                        \
    OOR_SET( API_2( mlib_dict_oa_def2_##blueprint##_oor_set ) ),                                                   \
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
  mlib_dict_oa_def2_##blueprint##_t map;                                                                           \
} mlib_dict_oa_def2_##blueprint;                                                                                   \
                                                                                                                   \
typedef struct                                                                                                     \
{                                                                                                                  \
  mlib_dict_oa_def2_##blueprint##_it_t itr;                                                                        \
} mlib_dict_oa_def2_##blueprint##_itr;                                                                             \
                                                                                                                   \
template<> struct mlib_dict_oa_def2< blueprint >                                                                   \
{                                                                                                                  \
  using map_type = mlib_dict_oa_def2_##blueprint;                                                                  \
  using itr_type = mlib_dict_oa_def2_##blueprint##_itr;                                                            \
                                                                                                                   \
  static map_type create_map()                                                                                     \
  {                                                                                                                \
    map_type map;                                                                                                  \
    mlib_dict_oa_def2_##blueprint##_init( map.map );                                                               \
    return map;                                                                                                    \
  }                                                                                                                \
                                                                                                                   \
  static itr_type find( map_type &map, const blueprint::key_type &key )                                            \
  {                                                                                                                \
    itr_type itr;                                                                                                  \
                                                                                                                   \
    blueprint::value_type *value = mlib_dict_oa_def2_##blueprint##_get( map.map, key );                            \
    if( !value )                                                                                                   \
    {                                                                                                              \
      mlib_dict_oa_def2_##blueprint##_it_end( itr.itr, map.map );                                                  \
      return itr;                                                                                                  \
    }                                                                                                              \
                                                                                                                   \
    itr.itr->dict = map.map;                                                                                       \
    itr.itr->index =                                                                                               \
      ( (char *)value - offsetof( struct mlib_dict_oa_def2_##blueprint##_pair_s, value ) - (char *)map.map->data ) \
      /                                                                                                            \
      sizeof( struct mlib_dict_oa_def2_##blueprint##_pair_s );                                                     \
                                                                                                                   \
    assert( &mlib_dict_oa_def2_##blueprint##_ref( itr.itr )->value == value );                                     \
                                                                                                                   \
    return itr;                                                                                                    \
  }                                                                                                                \
                                                                                                                   \
  static void insert( map_type &map, const blueprint::key_type &key )                                              \
  {                                                                                                                \
    mlib_dict_oa_def2_##blueprint##_set_at( map.map, key, typename blueprint::value_type() );                      \
  }                                                                                                                \
                                                                                                                   \
  static void erase( map_type &map, const blueprint::key_type &key )                                               \
  {                                                                                                                \
    mlib_dict_oa_def2_##blueprint##_erase( map.map, key );                                                         \
  }                                                                                                                \
                                                                                                                   \
  static itr_type begin_itr( map_type &map )                                                                       \
  {                                                                                                                \
    itr_type itr;                                                                                                  \
    mlib_dict_oa_def2_##blueprint##_it( itr.itr, map.map );                                                        \
    return itr;                                                                                                    \
  }                                                                                                                \
                                                                                                                   \
  static bool is_itr_valid( map_type &map, itr_type &itr )                                                         \
  {                                                                                                                \
    return !mlib_dict_oa_def2_##blueprint##_end_p( itr.itr );                                                      \
  }                                                                                                                \
                                                                                                                   \
  static void increment_itr( map_type &map, itr_type &itr )                                                        \
  {                                                                                                                \
    mlib_dict_oa_def2_##blueprint##_next( itr.itr );                                                               \
  }                                                                                                                \
                                                                                                                   \
  static const blueprint::key_type &get_key_from_itr( map_type &map, itr_type &itr )                               \
  {                                                                                                                \
    return mlib_dict_oa_def2_##blueprint##_ref( itr.itr )->key;                                                    \
  }                                                                                                                \
                                                                                                                   \
  static const blueprint::value_type &get_value_from_itr( map_type &map, itr_type &itr )                           \
  {                                                                                                                \
    return mlib_dict_oa_def2_##blueprint##_ref( itr.itr )->value;                                                  \
  }                                                                                                                \
                                                                                                                   \
  static void destroy_map( map_type &map )                                                                         \
  {                                                                                                                \
    mlib_dict_oa_def2_##blueprint##_clear( map.map );                                                              \
  }                                                                                                                \
};                                                                                                                 \

// M*LIB requires the user to reserve two key values for bookkeeping.
// This process is handled via the oor_equal and oor_set "operators".

#ifdef UINT32_UINT32_MURMUR_ENABLED

static inline bool mlib_dict_oa_def2_uint32_uint32_murmur_oor_equal( uint32_uint32_murmur::key_type k, unsigned char n )
{
  return k == std::numeric_limits< uint32_uint32_murmur::key_type >::max() - n;
}

static inline void mlib_dict_oa_def2_uint32_uint32_murmur_oor_set( uint32_uint32_murmur::key_type *k, unsigned char n )
{
  *k = std::numeric_limits< uint32_uint32_murmur::key_type >::max() - n;
}

MLIB_DICT_OA_DEF2_SPECIALIZATION( uint32_uint32_murmur )

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

static inline bool mlib_dict_oa_def2_uint64_struct448_murmur_oor_equal(
  uint64_struct448_murmur::key_type k,
  unsigned char n
)
{
  return k == std::numeric_limits< uint64_struct448_murmur::key_type >::max() - n;
}

static inline void mlib_dict_oa_def2_uint64_struct448_murmur_oor_set(
  uint64_struct448_murmur::key_type *k,
  unsigned char n
)
{
  *k = std::numeric_limits< uint64_struct448_murmur::key_type >::max() - n;
}

MLIB_DICT_OA_DEF2_SPECIALIZATION( uint64_struct448_murmur )

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED

static char mlib_cstring_oor_sentinels[ 2 ];

static inline bool mlib_dict_oa_def2_cstring_uint64_fnv1a_oor_equal( cstring_uint64_fnv1a::key_type k, unsigned char n )
{
  return k == &mlib_cstring_oor_sentinels[ n ];
}

static inline void mlib_dict_oa_def2_cstring_uint64_fnv1a_oor_set( cstring_uint64_fnv1a::key_type *k, unsigned char n )
{
  *k = &mlib_cstring_oor_sentinels[ n ];
}

MLIB_DICT_OA_DEF2_SPECIALIZATION( cstring_uint64_fnv1a )

#endif
