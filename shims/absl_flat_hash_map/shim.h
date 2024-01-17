// hash_map_benchmarks/shims/absl_flat_hash_map/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Woverflow"
#include "absl/container/internal/raw_hash_set.cc"
#include "absl/base/internal/raw_logging.cc"
#include "absl/hash/internal/hash.cc"
#include "absl/hash/internal/city.cc"
#include "absl/hash/internal/low_level_hash.cc"
#include "absl/container/flat_hash_map.h"
#pragma GCC diagnostic pop

template< typename blueprint > struct absl_flat_hash_map
{
  struct hash
  {
    using is_avalanching = void;

    std::size_t operator()( const blueprint::key_type &key ) const
    {
      return blueprint::hash_key( key );
    }
  };

  struct cmpr
  {
    bool operator()( const blueprint::key_type &key_1, const blueprint::key_type &key_2 ) const
    {
      return blueprint::cmpr_keys( key_1, key_2 );
    }
  };

  using map_type = absl::flat_hash_map<
    typename blueprint::key_type,
    typename blueprint::value_type,
    hash,
    cmpr
  >;

  static map_type create_map()
  {
    map_type map;
    map.max_load_factor( MAX_LOAD_FACTOR );
    return map;
  }

  static map_type::iterator find( map_type &map, const blueprint::key_type &key )
  {
    return map.find( key );
  }

  static void insert( map_type &map, const blueprint::key_type &key )
  {
    map[ key ] = typename blueprint::value_type();
  }

  static void erase( map_type &map, const blueprint::key_type &key )
  {
    map.erase( key );
  }

  static map_type::iterator begin_itr( map_type &map )
  {
    return map.begin();
  }

  static bool is_itr_valid( map_type &map, map_type::iterator &itr )
  {
    return itr != map.end();
  }

  static void increment_itr( map_type &map, map_type::iterator &itr )
  {
    ++itr;
  }

  static const blueprint::key_type &get_key_from_itr( map_type &map, map_type::iterator &itr )
  {
    return itr->first;
  }

  static const blueprint::value_type &get_value_from_itr( map_type &map, map_type::iterator &itr )
  {
    return itr->second;
  }

  static void destroy_map( map_type &map )
  {
    // RAII handles destruction.
  }
};

template<> struct absl_flat_hash_map< void >
{
  static constexpr const char *label = "absl::flat_hash_map";
  static constexpr const char *color = "rgb( 81, 169, 240 )";
};
