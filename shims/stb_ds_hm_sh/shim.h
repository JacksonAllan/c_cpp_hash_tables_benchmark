// hash_map_benchmarks/shims/stb_ds_hm_sh/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

// stb_ds does not provide users with the ability to specify a custom hash function.
// It simply uses one internally defined hash functioh for strings and another for all other keys.
// Hence, we must comment-out these functions inside the library and replace them with our own.
// Unfortunately, this is a fragile solution as the functions cannot easily be reliably and generically tailored to the
// each blueprint's hash function.
// The same issue exists for comparision functions and the maximum load factor - they require internal modification to
// the library (which has occurred in the latter case).

static inline size_t stbds_hash_bytes( void *p, size_t len, size_t seed )
{
  #ifdef UINT32_UINT32_MURMUR_ENABLED
  if( len == sizeof( uint32_uint32_murmur::key_type ) )
    return uint32_uint32_murmur::hash_key( *( uint32_uint32_murmur::key_type *)p );
  #endif

  #ifdef UINT64_STRUCT448_MURMUR_ENABLED
  if( len == sizeof( uint64_struct448_murmur::key_type ) )
    return uint64_struct448_murmur::hash_key( *( uint64_struct448_murmur::key_type *)p );
  #endif

  return 0;
}

static inline size_t stbds_hash_string( char *str, size_t seed )
{
  #ifdef CSTRING_UINT64_FNV1A_ENABLED
  return cstring_uint64_fnv1a::hash_key( str );
  #endif

  return 0;
}

#define STB_DS_IMPLEMENTATION
#include "stb_ds_modified.h"

template< typename > struct stb_ds_hm_sh
{
  static constexpr const char *label = "stb_ds";
  static constexpr const char *color = "rgb( 13, 127, 161 )";
};

#define STBDS_HM_SPECIALIZATION( blueprint )                                                           \
template<> struct stb_ds_hm_sh< blueprint >                                                            \
{                                                                                                      \
  using map_type = stb_ds_hm_sh_##blueprint *;                                                         \
                                                                                                       \
  static map_type create_map()                                                                         \
  {                                                                                                    \
    map_type map = NULL;                                                                               \
    return map;                                                                                        \
  }                                                                                                    \
                                                                                                       \
  static ptrdiff_t find( map_type &map, const blueprint::key_type &key )                               \
  {                                                                                                    \
    return stbds_hmgeti( map, key );                                                                   \
  }                                                                                                    \
                                                                                                       \
  static void insert( map_type &map, const blueprint::key_type &key )                                  \
  {                                                                                                    \
    stbds_hmput( map, key, typename blueprint::value_type() );                                         \
  }                                                                                                    \
                                                                                                       \
  static void erase( map_type &map, const blueprint::key_type &key )                                   \
  {                                                                                                    \
    stbds_hmdel( map, key );                                                                           \
  }                                                                                                    \
                                                                                                       \
  static ptrdiff_t begin_itr( map_type &map )                                                          \
  {                                                                                                    \
    return 0;                                                                                          \
  }                                                                                                    \
                                                                                                       \
  static bool is_itr_valid( map_type &map, ptrdiff_t &itr )                                            \
  {                                                                                                    \
    return itr != -1 && itr < stbds_hmlen( map );                                                      \
  }                                                                                                    \
                                                                                                       \
  static void increment_itr( map_type &map, ptrdiff_t &itr )                                           \
  {                                                                                                    \
    ++itr;                                                                                             \
  }                                                                                                    \
                                                                                                       \
  static const blueprint::key_type &get_key_from_itr( map_type &map, ptrdiff_t &itr )                  \
  {                                                                                                    \
    return map[ itr ].key;                                                                             \
  }                                                                                                    \
                                                                                                       \
  static const blueprint::value_type &get_value_from_itr( map_type &map, ptrdiff_t &itr )              \
  {                                                                                                    \
    return map[ itr ].value;                                                                           \
  }                                                                                                    \
                                                                                                       \
  static void destroy_map( map_type &map )                                                             \
  {                                                                                                    \
    stbds_hmfree( map );                                                                               \
  }                                                                                                    \
};                                                                                                     \

#define STBDS_SH_SPECIALIZATION( blueprint )                                                           \
template<> struct stb_ds_hm_sh< blueprint >                                                            \
{                                                                                                      \
  using map_type = stb_ds_hm_sh_##blueprint *;                                                         \
                                                                                                       \
  static map_type create_map()                                                                         \
  {                                                                                                    \
    map_type map = NULL;                                                                               \
    return map;                                                                                        \
  }                                                                                                    \
                                                                                                       \
  static ptrdiff_t find( map_type &map, const blueprint::key_type &key )                               \
  {                                                                                                    \
    return stbds_shgeti( map, key );                                                                   \
  }                                                                                                    \
                                                                                                       \
  static void insert( map_type &map, const blueprint::key_type &key )                                  \
  {                                                                                                    \
    stbds_shput( map, key, typename blueprint::value_type() );                                         \
  }                                                                                                    \
                                                                                                       \
  static void erase( map_type &map, const blueprint::key_type &key )                                   \
  {                                                                                                    \
    stbds_shdel( map, key );                                                                           \
  }                                                                                                    \
                                                                                                       \
  static ptrdiff_t begin_itr( map_type &map )                                                          \
  {                                                                                                    \
    return 0;                                                                                          \
  }                                                                                                    \
                                                                                                       \
  static bool is_itr_valid( map_type &map, ptrdiff_t &itr )                                            \
  {                                                                                                    \
    return itr != -1 && itr < stbds_shlen( map );                                                      \
  }                                                                                                    \
                                                                                                       \
  static void increment_itr( map_type &map, ptrdiff_t &itr )                                           \
  {                                                                                                    \
    ++itr;                                                                                             \
  }                                                                                                    \
                                                                                                       \
  static const blueprint::key_type &get_key_from_itr( map_type &map, ptrdiff_t &itr )                  \
  {                                                                                                    \
    return map[ itr ].key;                                                                             \
  }                                                                                                    \
                                                                                                       \
  static const blueprint::value_type &get_value_from_itr( map_type &map, ptrdiff_t &itr )              \
  {                                                                                                    \
    return map[ itr ].value;                                                                           \
  }                                                                                                    \
                                                                                                       \
  static void destroy_map( map_type &map )                                                             \
  {                                                                                                    \
    stbds_shfree( map );                                                                               \
  }                                                                                                    \
};                                                                                                     \

#ifdef UINT32_UINT32_MURMUR_ENABLED

struct stb_ds_hm_sh_uint32_uint32_murmur
{
  uint32_uint32_murmur::key_type key;
  uint32_uint32_murmur::value_type value;
};

STBDS_HM_SPECIALIZATION( uint32_uint32_murmur )

#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

struct stb_ds_hm_sh_uint64_struct448_murmur
{
  uint64_struct448_murmur::key_type key;
  uint64_struct448_murmur::value_type value;
};

STBDS_HM_SPECIALIZATION( uint64_struct448_murmur )

#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED

struct stb_ds_hm_sh_cstring_uint64_fnv1a
{
  cstring_uint64_fnv1a::key_type key;
  cstring_uint64_fnv1a::value_type value;
};

STBDS_SH_SPECIALIZATION( cstring_uint64_fnv1a )

#endif
