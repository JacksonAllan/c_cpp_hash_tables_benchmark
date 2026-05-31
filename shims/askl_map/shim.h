// c_cpp_hash_tables_benchmark/shims/askl_map/shim.h
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <sstream>

extern "C" {

#ifndef _ENABLE_HASHMAP
#define _ENABLE_HASHMAP
#endif

#include "compat/askl_compat_layer.h"
#include "compat/askl_compat_layer.c"
#include "askl_variant.h"
#include "askl_variant.c"
#include "askl_htable.h"
#include "askl_rwlock.c"
#include "askl_htable.c"

}

template< typename > struct askl_map
{
  static constexpr const char *label = "ASKL Map";
  static constexpr const char *color = "rgb( 170, 90, 190 )";
  static constexpr bool tombstone_like_mechanism = true;
};

/* -------------------------------------------------------------------------- */
/* Blueprint-specific adapters                                                */
/* -------------------------------------------------------------------------- */

#define ASKL_MAP_BINARY_KEY_ADAPTER( blueprint )                                             \
                                                                                             \
static inline const char *askl_map_##blueprint##_key_data(                                   \
  const blueprint::key_type &key                                                             \
)                                                                                            \
{                                                                                            \
  return reinterpret_cast< const char * >( &key );                                           \
}                                                                                            \
                                                                                             \
static inline std::size_t askl_map_##blueprint##_key_size(                                   \
  const blueprint::key_type &key                                                             \
)                                                                                            \
{                                                                                            \
  (void) key;                                                                                \
  return sizeof( blueprint::key_type );                                                      \
}                                                                                            \
                                                                                             \
static inline blueprint::key_type askl_map_##blueprint##_key_from_askl(                      \
  const char *ptr,                                                                           \
  std::size_t len                                                                            \
)                                                                                            \
{                                                                                            \
  blueprint::key_type key;                                                                   \
  (void) len;                                                                                \
  std::memcpy( &key, ptr, sizeof( key ) );                                                   \
  return key;                                                                                \
}

#define ASKL_MAP_INTEGRAL_VALUE_ADAPTER( blueprint )                                         \
                                                                                             \
static inline void askl_map_##blueprint##_value_free_askl( Variant value )                  \
{                                                                                            \
  (void) value;                                                                              \
}                                                                                            \
                                                                                             \
static inline Variant askl_map_##blueprint##_value_to_askl(                                  \
  const blueprint::value_type &value                                                         \
)                                                                                            \
{                                                                                            \
  return variant_from_integer( static_cast< uint64_t >( value ) );                           \
}                                                                                            \
                                                                                             \
static inline const blueprint::value_type *askl_map_##blueprint##_value_ref_from_askl(       \
  Variant value,                                                                             \
  blueprint::value_type &scratch                                                             \
)                                                                                            \
{                                                                                            \
  scratch = static_cast< blueprint::value_type >( variant_to_integer( value ) );             \
  return &scratch;                                                                           \
}

/*
 * ASKL Variant cannot hold uint64_struct448_murmur::value_type by value.
 * Store it out-of-line and put the pointer in the integer Variant payload.
 *
 * This is a benchmark adapter, not a recommended public ASKL value model.
 */
#define ASKL_MAP_HEAP_VALUE_ADAPTER( blueprint )                                             \
                                                                                             \
static inline void askl_map_##blueprint##_value_free_askl( Variant value )                  \
{                                                                                            \
  if ( is_integer( value ) ) {                                                               \
    uintptr_t ptr = static_cast< uintptr_t >( variant_to_integer( value ) );                \
    delete reinterpret_cast< blueprint::value_type * >( ptr );                              \
  }                                                                                          \
}                                                                                            \
                                                                                             \
static inline Variant askl_map_##blueprint##_value_to_askl(                                  \
  const blueprint::value_type &value                                                         \
)                                                                                            \
{                                                                                            \
  blueprint::value_type *copy = new ( std::nothrow ) blueprint::value_type( value );        \
  return variant_from_integer( static_cast< uint64_t >(                                     \
    reinterpret_cast< uintptr_t >( copy )                                                    \
  ) );                                                                                       \
}                                                                                            \
                                                                                             \
static inline const blueprint::value_type *askl_map_##blueprint##_value_ref_from_askl(       \
  Variant value,                                                                             \
  blueprint::value_type &scratch                                                             \
)                                                                                            \
{                                                                                            \
  if ( is_integer( value ) ) {                                                               \
    uintptr_t ptr = static_cast< uintptr_t >( variant_to_integer( value ) );                \
    if ( ptr )                                                                               \
      return reinterpret_cast< const blueprint::value_type * >( ptr );                      \
  }                                                                                          \
  scratch = blueprint::value_type();                                                         \
  return &scratch;                                                                           \
}

/* uint32 -> uint32 --------------------------------------------------------- */

#ifdef UINT32_UINT32_MURMUR_ENABLED

ASKL_MAP_BINARY_KEY_ADAPTER( uint32_uint32_murmur )
ASKL_MAP_INTEGRAL_VALUE_ADAPTER( uint32_uint32_murmur )

#endif

/* uint64 -> struct448 ------------------------------------------------------ */

#ifdef UINT64_STRUCT448_MURMUR_ENABLED

ASKL_MAP_BINARY_KEY_ADAPTER( uint64_struct448_murmur )
ASKL_MAP_HEAP_VALUE_ADAPTER( uint64_struct448_murmur )

#endif

/* cstring -> uint64 -------------------------------------------------------- */

#ifdef CSTRING_UINT64_FNV1A_ENABLED

static inline const char *askl_map_cstring_uint64_fnv1a_key_data(
  const cstring_uint64_fnv1a::key_type &key
)
{
  return key;
}

static inline std::size_t askl_map_cstring_uint64_fnv1a_key_size(
  const cstring_uint64_fnv1a::key_type &key
)
{
  (void) key;
  return std::strlen( key );
}

static inline cstring_uint64_fnv1a::key_type askl_map_cstring_uint64_fnv1a_key_from_askl(
  const char *ptr,
  std::size_t len
)
{
  (void) len;
  return const_cast< char * >( ptr );
}

ASKL_MAP_INTEGRAL_VALUE_ADAPTER( cstring_uint64_fnv1a )

#endif

/* -------------------------------------------------------------------------- */
/* Specialization macro                                                       */
/* -------------------------------------------------------------------------- */

#define ASKL_MAP_SPECIALIZATION( blueprint )                                                 \
                                                                                             \
typedef struct                                                                               \
{                                                                                            \
  Map *table;                                                                                \
} askl_map_##blueprint;                                                                      \
                                                                                             \
typedef struct                                                                               \
{                                                                                            \
  _Bucket *bucket;                                                                           \
  bool from_find;                                                                            \
  blueprint::key_type key;                                                                   \
  blueprint::value_type value;                                                               \
} askl_map_##blueprint##_itr;                                                                \
                                                                                             \
static inline _Bucket *askl_map_##blueprint##_next_live( _Bucket *bucket )                   \
{                                                                                            \
  while ( bucket && !bucket->item.key.len )                                                  \
    bucket = bucket->next;                                                                   \
  return bucket;                                                                             \
}                                                                                            \
                                                                                             \
static inline void askl_map_##blueprint##_free_live_values( Map *table )                    \
{                                                                                            \
  _Bucket *bucket = nullptr;                                                                 \
  if ( !table )                                                                              \
    return;                                                                                  \
  for ( bucket = table->_index; bucket; bucket = bucket->next )                              \
    if ( bucket->item.key.len )                                                              \
      askl_map_##blueprint##_value_free_askl( bucket->item.val );                           \
}                                                                                            \
                                                                                             \
template<> struct askl_map< blueprint >                                                      \
{                                                                                            \
  using table_type = askl_map_##blueprint;                                                   \
  using itr_type = askl_map_##blueprint##_itr;                                               \
                                                                                             \
  static constexpr const char *label = "ASKL Map";                                           \
  static constexpr const char *color = "rgb( 170, 90, 190 )";                                \
  static constexpr bool tombstone_like_mechanism = true;                                     \
                                                                                             \
  static table_type create_table()                                                           \
  {                                                                                          \
    table_type table;                                                                        \
    table.table = map_alloc( nullptr );                                                      \
    return table;                                                                            \
  }                                                                                          \
                                                                                             \
  static itr_type find( table_type &table, const blueprint::key_type &key )                  \
  {                                                                                          \
    itr_type itr;                                                                            \
    itr.bucket = nullptr;                                                                    \
    itr.from_find = false;                                                                   \
    itr.key = key;                                                                           \
    itr.value = blueprint::value_type();                                                     \
                                                                                             \
    Variant value = {{ 0 }};                                                                 \
    _Item *item = _get_item(                                                                 \
      table.table,                                                                           \
      askl_map_##blueprint##_key_data( key ),                                                \
      askl_map_##blueprint##_key_size( key ),                                                \
      &value                                                                                 \
    );                                                                                       \
                                                                                             \
    if ( item ) {                                                                            \
      itr.bucket = (_Bucket *)((char *)item - offsetof( _Bucket, item ));                    \
      itr.from_find = true;                                                                  \
      itr.key = key;                                                                         \
      itr.value = *askl_map_##blueprint##_value_ref_from_askl( value, itr.value );          \
    }                                                                                        \
                                                                                             \
    return itr;                                                                              \
  }                                                                                          \
                                                                                             \
  static void insert( table_type &table, const blueprint::key_type &key )                    \
  {                                                                                          \
    blueprint::value_type value = blueprint::value_type();                                   \
    Variant rejected = map_set(                                                              \
      table.table,                                                                           \
      askl_map_##blueprint##_key_data( key ),                                                \
      askl_map_##blueprint##_key_size( key ),                                                \
      askl_map_##blueprint##_value_to_askl( value )                                          \
    );                                                                                       \
    askl_map_##blueprint##_value_free_askl( rejected );                                     \
  }                                                                                          \
                                                                                             \
  static void erase( table_type &table, const blueprint::key_type &key )                     \
  {                                                                                          \
    Variant removed = map_remove(                                                            \
      table.table,                                                                           \
      askl_map_##blueprint##_key_data( key ),                                                \
      askl_map_##blueprint##_key_size( key )                                                 \
    );                                                                                       \
    askl_map_##blueprint##_value_free_askl( removed );                                      \
  }                                                                                          \
                                                                                             \
  static itr_type begin_itr( table_type &table )                                             \
  {                                                                                          \
    itr_type itr;                                                                            \
    itr.bucket = askl_map_##blueprint##_next_live( table.table->_index );                    \
    itr.from_find = false;                                                                   \
    itr.key = blueprint::key_type();                                                         \
    itr.value = blueprint::value_type();                                                     \
    return itr;                                                                              \
  }                                                                                          \
                                                                                             \
  static bool is_itr_valid( table_type &table, itr_type &itr )                               \
  {                                                                                          \
    (void) table;                                                                            \
    return itr.bucket != nullptr;                                                            \
  }                                                                                          \
                                                                                             \
  static void increment_itr( table_type &table, itr_type &itr )                              \
  {                                                                                          \
    (void) table;                                                                            \
                                                                                             \
    itr.from_find = false;                                                                   \
    if ( itr.bucket )                                                                        \
      itr.bucket = askl_map_##blueprint##_next_live( itr.bucket->next );                     \
  }                                                                                          \
                                                                                             \
  static const blueprint::key_type &get_key_from_itr( table_type &table, itr_type &itr )     \
  {                                                                                          \
    (void) table;                                                                            \
                                                                                             \
    if ( itr.from_find )                                                                     \
      return itr.key;                                                                        \
                                                                                             \
    itr.key = askl_map_##blueprint##_key_from_askl(                                          \
      itr.bucket->item.key.str,                                                              \
      itr.bucket->item.key.len                                                               \
    );                                                                                       \
    return itr.key;                                                                          \
  }                                                                                          \
                                                                                             \
  static const blueprint::value_type &get_value_from_itr( table_type &table, itr_type &itr ) \
  {                                                                                          \
    (void) table;                                                                            \
    return *askl_map_##blueprint##_value_ref_from_askl(                                     \
      itr.bucket->item.val,                                                                  \
      itr.value                                                                              \
    );                                                                                       \
  }                                                                                          \
                                                                                             \
  static void destroy_table( table_type &table )                                             \
  {                                                                                          \
    askl_map_##blueprint##_free_live_values( table.table );                                  \
    map_free( table.table );                                                                 \
    table.table = nullptr;                                                                   \
  }                                                                                          \
};                                                                                           

#ifdef UINT32_UINT32_MURMUR_ENABLED
ASKL_MAP_SPECIALIZATION( uint32_uint32_murmur )
#endif

#ifdef UINT64_STRUCT448_MURMUR_ENABLED
ASKL_MAP_SPECIALIZATION( uint64_struct448_murmur )
#endif

#ifdef CSTRING_UINT64_FNV1A_ENABLED
ASKL_MAP_SPECIALIZATION( cstring_uint64_fnv1a )
#endif
