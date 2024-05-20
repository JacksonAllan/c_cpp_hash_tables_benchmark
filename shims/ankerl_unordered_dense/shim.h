// c_cpp_hash_tables_benchmark/shims/ankerl_unordered_dense/shim.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#include "unordered_dense.h"

template< typename blueprint > struct ankerl_unordered_dense
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

  using table_type = ankerl::unordered_dense::map<
    typename blueprint::key_type,
    typename blueprint::value_type,
    hash,
    cmpr
  >;

  static table_type create_table()
  {
    table_type table;
    table.max_load_factor( MAX_LOAD_FACTOR );
    return table;
  }

  static table_type::iterator find( table_type &table, const blueprint::key_type &key )
  {
    return table.find( key );
  }

  static void insert( table_type &table, const blueprint::key_type &key )
  {
    table[ key ] = typename blueprint::value_type();
  }

  static void erase( table_type &table, const blueprint::key_type &key )
  {
    table.erase( key );
  }

  static table_type::iterator begin_itr( table_type &table )
  {
    return table.begin();
  }

  static bool is_itr_valid( table_type &table, table_type::iterator &itr )
  {
    return itr != table.end();
  }

  static void increment_itr( table_type &table, table_type::iterator &itr )
  {
    ++itr;
  }

  static const blueprint::key_type &get_key_from_itr( table_type &table, table_type::iterator &itr )
  {
    return itr->first;
  }

  static const blueprint::value_type &get_value_from_itr( table_type &table, table_type::iterator &itr )
  {
    return itr->second;
  }

  static void destroy_table( table_type &table )
  {
    // RAII handles destruction.
  }
};

template<> struct ankerl_unordered_dense< void >
{
  static constexpr const char *label = "ankerl";
  static constexpr const char *color = "rgb( 241, 199, 78 )";
  static constexpr bool tombstone_like_mechanism = false;
};
