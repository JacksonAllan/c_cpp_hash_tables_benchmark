// c_cpp_hash_tables_benchmark/main.cpp
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#include <algorithm>
#include <array>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include "config.h"

// Check configuration.
static_assert( KEY_COUNT % KEY_COUNT_MEASUREMENT_INTERVAL == 0 );
static_assert( DISCARDED_RUNS_COUNT < RUN_COUNT );
static_assert( DISCARDED_RUNS_COUNT % 2 == 0 );

// Standard stringification macro, used to form the file paths of the blueprints and shims.
#define STRINGIFY_( x ) #x
#define STRINGIFY( x ) STRINGIFY_( x )

// Concept to check that a blueprint is correctly formed.
template< typename blueprint >
concept check_blueprint =
  std::is_object< typename blueprint::value_type >::value &&
  std::same_as< decltype( blueprint::label ), const char * const > &&
  std::same_as< decltype( blueprint::hash_key ), uint64_t ( const typename blueprint::key_type & ) > &&
  std::same_as<
    decltype( blueprint::cmpr_keys ), 
    bool ( const typename blueprint::key_type &, const typename blueprint::key_type & )
  > &&
  std::same_as< decltype( blueprint::fill_unique_keys ), void ( std::vector< typename blueprint::key_type > & ) >
;

// #include blueprints and check them for correctness.
#ifdef BLUEPRINT_1
#include STRINGIFY( blueprints/BLUEPRINT_1/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
#include STRINGIFY( blueprints/BLUEPRINT_2/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
#include STRINGIFY( blueprints/BLUEPRINT_3/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
#include STRINGIFY( blueprints/BLUEPRINT_4/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
#include STRINGIFY( blueprints/BLUEPRINT_5/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
#include STRINGIFY( blueprints/BLUEPRINT_6/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
#include STRINGIFY( blueprints/BLUEPRINT_7/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
#include STRINGIFY( blueprints/BLUEPRINT_8/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
#include STRINGIFY( blueprints/BLUEPRINT_9/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
#include STRINGIFY( blueprints/BLUEPRINT_10/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
#include STRINGIFY( blueprints/BLUEPRINT_11/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
#include STRINGIFY( blueprints/BLUEPRINT_12/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_13
#include STRINGIFY( blueprints/BLUEPRINT_13/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_13 > );
#endif
#ifdef BLUEPRINT_14
#include STRINGIFY( blueprints/BLUEPRINT_14/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
#include STRINGIFY( blueprints/BLUEPRINT_15/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
#include STRINGIFY( blueprints/BLUEPRINT_16/blueprint.h )
static_assert( check_blueprint< BLUEPRINT_16 > );
#endif

// Concept to check that a shim is, in isolation, correctly formed.
template< template< typename > typename shim >
concept check_shim =
  std::same_as< decltype( shim< void >::label ), const char * const > &&
  std::same_as< decltype( shim< void >::color ), const char * const > &&
  std::same_as< decltype( shim< void >::tombstone_like_mechanism ), const bool >
;

// Concept to check that a shim is correctly formed in relation to a specific blueprint.
template<
  template< typename > typename shim,
  typename blueprint,
  // Helpers to simplify the concept's definition:
  typename table_type = decltype( shim< blueprint >::create_table() ),
  typename itr_type = decltype( shim< blueprint >::begin_itr( std::declval< table_type & >() ) )
>
concept check_shim_against_blueprint =
  std::same_as< decltype( shim< blueprint >::create_table ), table_type () > &&
  std::same_as< decltype( shim< blueprint >::insert ), void ( table_type &, const typename blueprint::key_type & ) > &&
  std::same_as< decltype( shim< blueprint >::erase ), void ( table_type &, const typename blueprint::key_type & ) > &&
  std::same_as<
    decltype( shim< blueprint >::find ),
    itr_type ( table_type &, const typename blueprint::key_type & )
  > &&
  std::same_as< decltype( shim< blueprint >::begin_itr ), itr_type ( table_type & ) > &&
  std::same_as< decltype( shim< blueprint >::is_itr_valid ), bool ( table_type &, itr_type & ) > &&
  std::same_as< decltype( shim< blueprint >::increment_itr ), void ( table_type &, itr_type & ) > &&
  std::same_as< 
    decltype( shim< blueprint >::get_key_from_itr ),
    const typename blueprint::key_type &( table_type &, itr_type & )
  > &&
  std::same_as< 
    decltype( shim< blueprint >::get_value_from_itr ),
    const typename blueprint::value_type &( table_type &, itr_type & )
  > &&
  std::same_as< decltype( shim< blueprint >::destroy_table ), void ( table_type & ) >
;

// #include shims and check them for correctness.

#ifdef SHIM_1
#include STRINGIFY( shims/SHIM_1/shim.h )
static_assert( check_shim< SHIM_1 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_1, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_2
#include STRINGIFY( shims/SHIM_2/shim.h )
static_assert( check_shim< SHIM_2 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_2, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_3
#include STRINGIFY( shims/SHIM_3/shim.h )
static_assert( check_shim< SHIM_3 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_3, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_4
#include STRINGIFY( shims/SHIM_4/shim.h )
static_assert( check_shim< SHIM_4 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_4, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_5
#include STRINGIFY( shims/SHIM_5/shim.h )
static_assert( check_shim< SHIM_5 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_5, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_6
#include STRINGIFY( shims/SHIM_6/shim.h )
static_assert( check_shim< SHIM_6 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_6, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_7
#include STRINGIFY( shims/SHIM_7/shim.h )
static_assert( check_shim< SHIM_7 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_7, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_8
#include STRINGIFY( shims/SHIM_8/shim.h )
static_assert( check_shim< SHIM_8 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_8, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_9
#include STRINGIFY( shims/SHIM_9/shim.h )
static_assert( check_shim< SHIM_9 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_9, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_10
#include STRINGIFY( shims/SHIM_10/shim.h )
static_assert( check_shim< SHIM_10 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_10, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_11
#include STRINGIFY( shims/SHIM_11/shim.h )
static_assert( check_shim< SHIM_11 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_11, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_12
#include STRINGIFY( shims/SHIM_12/shim.h )
static_assert( check_shim< SHIM_12 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_12, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_13
#include STRINGIFY( shims/SHIM_13/shim.h )
static_assert( check_shim< SHIM_13 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_13, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_14
#include STRINGIFY( shims/SHIM_14/shim.h )
static_assert( check_shim< SHIM_14 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_14, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_15
#include STRINGIFY( shims/SHIM_15/shim.h )
static_assert( check_shim< SHIM_15 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_15, BLUEPRINT_16 > );
#endif
#endif

#ifdef SHIM_16
#include STRINGIFY( shims/SHIM_16/shim.h )
static_assert( check_shim< SHIM_16 > );
#ifdef BLUEPRINT_1
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_1 > );
#endif
#ifdef BLUEPRINT_2
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_2 > );
#endif
#ifdef BLUEPRINT_3
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_3 > );
#endif
#ifdef BLUEPRINT_4
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_4 > );
#endif
#ifdef BLUEPRINT_5
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_5 > );
#endif
#ifdef BLUEPRINT_6
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_6 > );
#endif
#ifdef BLUEPRINT_7
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_7 > );
#endif
#ifdef BLUEPRINT_8
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_8 > );
#endif
#ifdef BLUEPRINT_9
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_9 > );
#endif
#ifdef BLUEPRINT_10
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_10 > );
#endif
#ifdef BLUEPRINT_11
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_11 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_12
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_12 > );
#endif
#ifdef BLUEPRINT_14
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_14 > );
#endif
#ifdef BLUEPRINT_15
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_15 > );
#endif
#ifdef BLUEPRINT_16
static_assert( check_shim_against_blueprint< SHIM_16, BLUEPRINT_16 > );
#endif
#endif

// Random number generator.
std::default_random_engine random_number_generator( std::chrono::steady_clock::now().time_since_epoch().count() );

// Function for providing unique keys for a given blueprint in random order.
// Besides the KEY_COUNT keys to be inserted, it also provides an extra KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL *
// 1000 keys for testing failed look-ups.
template< typename blueprint > const blueprint::key_type &shuffled_unique_key( size_t index )
{
  static auto keys = []()
  {
    std::vector<typename blueprint::key_type> keys( KEY_COUNT + KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL * 1000 );
    blueprint::fill_unique_keys( keys );
    std::shuffle( keys.begin(), keys.end(), random_number_generator );
    return keys;
  }();

  return keys[ index ];
}

// Benchmark ids.
enum benchmark_ids
{
  insert_nonexisting,
  erase_existing,
  insert_existing,
  erase_nonexisting,
  get_existing,
  get_nonexisting,
  iteration
};

// Benchmark names used in the graphs.
const char *benchmark_names[] = {
  "Total time to insert N nonexisting keys",
  "Time to erase 1,000 existing keys with N keys in the table",
  "Time to replace 1,000 existing keys with N keys in the table",
  "Time to erase 1,000 nonexisting keys with N keys in the table",
  "Time to look up 1,000 existing keys with N keys in the table",
  "Time to look up 1,000 nonexisting keys with N keys in the table",
  "Time to iterate over 5,000 keys with N keys in the table"
};

// Benchmark names used in the heatmap.
const char *benchmark_alt_names[] = {
  "Insert nonexisting",
  "Erase existing",
  "Replace existing",
  "Erase nonexisting",
  "Look up existing",
  "Look up nonexisting",
  "Iterate"
};

// Function for recording and accessing a single result, i.e. one measurement for a particular table, blueprint, and
// benchmark during a particular run.
template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
uint64_t &results( size_t run_index, size_t result_index )
{
  static auto results = std::vector< uint64_t >( RUN_COUNT * ( KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL ) );
  return results[ run_index * ( KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL ) + result_index ];
}

// Function that attempts to reset the cache to the same state before each table is benchmarked.
// The strategy is to iterate over an array that is at least as large as the L1, L2, and L3 caches combined.
void flush_cache()
{
  static auto buffer = std::vector< uint64_t >( APPROXIMATE_CACHE_SIZE / sizeof( uint64_t ) + 1 );

  volatile uint64_t do_not_optimize;
  for( auto itr = buffer.begin(); itr != buffer.end(); ++itr )
    do_not_optimize = *itr;

  (void)do_not_optimize;
}
 
// Actual benchmarking function.
template< template< typename > typename shim, typename blueprint >void benchmark( unsigned int run )
{
  std::cout << shim<void>::label << ": " << blueprint::label << "\n";

  volatile unsigned char do_not_optimize = 0;

  // Ugly workaround to ensure that the static unique keys and results arrays inside their respective template functions
  // are initialized outside of the timed code below.
  // This is necessary because C++ has no built-in mechanism to specify that local static variables should be fully
  // initialized at program start-up.
  do_not_optimize = *(unsigned char *)&shuffled_unique_key< blueprint >( 0 );
  do_not_optimize = *(unsigned char *)&results< shim, blueprint, insert_nonexisting >( 0, 0 );
  do_not_optimize = *(unsigned char *)&results< shim, blueprint, erase_existing >( 0, 0 );
  do_not_optimize = *(unsigned char *)&results< shim, blueprint, insert_existing >( 0, 0 );
  do_not_optimize = *(unsigned char *)&results< shim, blueprint, erase_nonexisting >( 0, 0 );
  do_not_optimize = *(unsigned char *)&results< shim, blueprint, get_existing >( 0, 0 );
  do_not_optimize = *(unsigned char *)&results< shim, blueprint, get_nonexisting >( 0, 0 );
  do_not_optimize = *(unsigned char *)&results< shim, blueprint, iteration >( 0, 0 );

  // Reset the cache state.
  // This ensures that each table starts each benchmarking run on equal footing, but it does not prevent the cache
  // effects of one benchmark from potentially influencing latter benchmarks.
  flush_cache();

  #ifdef BENCHMARK_INSERT_NONEXISTING
  {
    auto table = shim< blueprint >::create_table();

    std::this_thread::sleep_for( std::chrono::milliseconds( MILLISECOND_COOLDOWN_BETWEEN_BENCHMARKS ) );

    size_t i = 0;
    size_t j = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while( i < KEY_COUNT )
    {
      shim< blueprint >::insert( table, shuffled_unique_key< blueprint >( i ) );

      ++i;
      if( ++j == KEY_COUNT_MEASUREMENT_INTERVAL )
      {
        results< shim, blueprint, insert_nonexisting >( run, i / KEY_COUNT_MEASUREMENT_INTERVAL - 1 ) =
          std::chrono::duration_cast< std::chrono::microseconds >(
            std::chrono::high_resolution_clock::now() - start
          ).count();

        j = 0;
      }
    }

    shim< blueprint >::destroy_table( table );
  }
  #endif

  #ifdef BENCHMARK_ERASE_EXISTING
  {
    auto table = shim< blueprint >::create_table();

    std::this_thread::sleep_for( std::chrono::milliseconds( MILLISECOND_COOLDOWN_BETWEEN_BENCHMARKS ) );

    size_t i = 0;
    size_t j = 0;
    while( i < KEY_COUNT )
    {
      shim< blueprint >::insert( table, shuffled_unique_key< blueprint >( i ) );

      ++i;
      if( ++j == KEY_COUNT_MEASUREMENT_INTERVAL )
      {
        size_t result_index = i / KEY_COUNT_MEASUREMENT_INTERVAL - 1;

        // To determine which keys to erase, we randomly chose a position in the sequence of keys already inserted and
        // then erase the subsequent 1000 keys, wrapping around to the start of the sequence if necessary.
        // This strategy has the potential drawback that keys are erased in the same order in which they were inserted.
        size_t erase_keys_begin = std::uniform_int_distribution<size_t>( 0, i - 1 )( random_number_generator );

        size_t k = 0;
        size_t l = erase_keys_begin;

        auto start = std::chrono::high_resolution_clock::now();
        while( true )
        {
          shim< blueprint >::erase( table, shuffled_unique_key< blueprint >( l ) );

          if( ++k == 1000 )
            break;
          if( ++l == i )
            l = 0;
        }

        results< shim, blueprint, erase_existing >( run, result_index ) =
          std::chrono::duration_cast< std::chrono::nanoseconds >(
            std::chrono::high_resolution_clock::now() - start
          ).count();

        // Re-insert the erased keys.
        // This has the drawback that if tombstones are being used, those tombstones created by the above erasures will
        // all be replaced by the re-inserted keys.
        // Hence, this benchmark cannot show the lingering effect of tombstones.
        k = 0;
        l = erase_keys_begin;
        while( true )
        {
          shim< blueprint >::insert( table, shuffled_unique_key< blueprint >( l ) );
  
          if( ++k == 1000 )
            break;
          if( ++l == i )
            l = 0;
        }

        j = 0;
      }
    }

    shim< blueprint >::destroy_table( table );
  }
  #endif

  #if                                         \
    defined( BENCHMARK_INSERT_EXISTING )   || \
    defined( BENCHMARK_ERASE_NONEXISTING ) || \
    defined( BENCHMARK_GET_EXISTING )      || \
    defined( BENCHMARK_GET_NONEXISTING )   || \
    defined( BENCHMARK_ITERATION )
  {
    auto table = shim< blueprint >::create_table();

    std::this_thread::sleep_for( std::chrono::milliseconds( MILLISECOND_COOLDOWN_BETWEEN_BENCHMARKS ) );

    size_t i = 0;
    size_t j = 0;
    while( i < KEY_COUNT )
    {
      shim< blueprint >::insert( table, shuffled_unique_key< blueprint >( i ) );

      ++i;
      if( ++j == KEY_COUNT_MEASUREMENT_INTERVAL )
      {
        size_t result_index = i / KEY_COUNT_MEASUREMENT_INTERVAL - 1;

        #ifdef BENCHMARK_INSERT_EXISTING
        {
          size_t k = 0;

          // To determine which existing keys to replace, we apply the same strategy that we applied when erasing
          // existing keys.
          size_t l = std::uniform_int_distribution<size_t>( 0, i - 1 )( random_number_generator );

          auto start = std::chrono::high_resolution_clock::now();
          while( true )
          {
            shim< blueprint >::insert( table, shuffled_unique_key< blueprint >( l ) );

            if( ++k == 1000 )
              break;
            if( ++l == i )
              l = 0;
          }

          results< shim, blueprint, insert_existing >( run, result_index ) =
            std::chrono::duration_cast< std::chrono::nanoseconds >(
              std::chrono::high_resolution_clock::now() - start
            ).count();
        }
        #endif

        #ifdef BENCHMARK_ERASE_NONEXISTING
        {
          size_t k = 0;

          // To determine which nonexisting keys to attempt to erase, we randomly chose a position in the sequence of
          // nonexisting keys (which starts at KEY_COUNT) and then call erase for the subsequent 1000 keys, wrapping
          // around to the start of the sequence if necessary.
          size_t l = std::uniform_int_distribution<size_t>(
            KEY_COUNT,
            KEY_COUNT + KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL * 1000 - 1
          )( random_number_generator );

          auto start = std::chrono::high_resolution_clock::now();
          while( true )
          {
            shim< blueprint >::erase( table, shuffled_unique_key< blueprint >( l ) );

            if( ++k == 1000 )
              break;
            if( ++l == KEY_COUNT + KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL * 1000 )
              l = KEY_COUNT;
          }

          results< shim, blueprint, erase_nonexisting >( run, result_index ) =
            std::chrono::duration_cast< std::chrono::nanoseconds >(
              std::chrono::high_resolution_clock::now() - start
            ).count();
        }
        #endif

        #ifdef BENCHMARK_GET_EXISTING
        {
          size_t k = 0;

          // To determine which existing keys to look-up, we apply the same strategy that we applied when erasing
          // and inserting existing keys.
          size_t l = std::uniform_int_distribution<size_t>( 0, i - 1 )( random_number_generator );

          auto start = std::chrono::high_resolution_clock::now();
          while( true )
          {
            auto itr = shim< blueprint >::find( table, shuffled_unique_key< blueprint >( l ) );

            // Accessing the first byte of the value prevents the above call from being optimized out and ensures that
            // tables that can preform look-ups without accessing the values (because the keys are stored separately)
            // actually incur the additional cache miss that they would incur during normal use.
            do_not_optimize = *(unsigned char *)&shim< blueprint >::get_value_from_itr( table, itr );

            if( ++k == 1000 )
              break;
            if( ++l == i )
              l = 0;
          }

          results< shim, blueprint, get_existing >( run, result_index ) =
            std::chrono::duration_cast< std::chrono::nanoseconds >(
              std::chrono::high_resolution_clock::now() - start
            ).count();
        }
        #endif

        #ifdef BENCHMARK_GET_NONEXISTING
        {
          size_t k = 0;

          // To determine which nonexisting keys to attempt to look-up, we apply the same strategy that we applied when
          // erasing nonexisting keys.
          size_t l = std::uniform_int_distribution<size_t>(
            KEY_COUNT,
            KEY_COUNT + KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL * 1000 - 1
          )( random_number_generator );

          auto start = std::chrono::high_resolution_clock::now();
          while( true )
          {
            volatile auto itr = shim< blueprint >::find( table, shuffled_unique_key< blueprint >( l ) );
            (void)itr;

            if( ++k == 1000 )
              break;
            if( ++l == KEY_COUNT + KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL * 1000 )
              l = KEY_COUNT;
          }

          results< shim, blueprint, get_nonexisting >( run, result_index ) =
            std::chrono::duration_cast< std::chrono::nanoseconds >(
              std::chrono::high_resolution_clock::now() - start
            ).count();
        }
        #endif

        #ifdef BENCHMARK_ITERATION
        {
          // To determine where inside the table to begin iteration, we randomly choose an existing key.
          // This ensures that we are not just hitting the same, cached memory every time we measure.
          auto itr = shim< blueprint >::find(
            table,
            shuffled_unique_key< blueprint >(
              std::uniform_int_distribution<size_t>( 0, i - 1 )( random_number_generator )
            )
          );

          size_t k = 0;
          auto start = std::chrono::high_resolution_clock::now();
          while( true )
          {
            // Accessing the first bytes of the key and value ensures that tables that iterate without directly accessing
            // the keys and/or values actually incur the additional cache misses that they would incur during normal
            // use.
            do_not_optimize = *(unsigned char *)&shim< blueprint >::get_key_from_itr( table, itr );
            do_not_optimize = *(unsigned char *)&shim< blueprint >::get_value_from_itr( table, itr );

            if( ++k == 1000 )
              break;

            shim< blueprint >::increment_itr( table, itr );
            if( !shim< blueprint >::is_itr_valid( table, itr ) )
              itr = shim< blueprint >::begin_itr( table );
          }

          results< shim, blueprint, iteration >( run, result_index ) =
            std::chrono::duration_cast< std::chrono::nanoseconds >(
              std::chrono::high_resolution_clock::now() - start
          ).count();
        }
        #endif

        j = 0;
      }
    }   

    shim< blueprint >::destroy_table( table );
  }
  #endif

  (void)do_not_optimize;
}

// Function for benchmarking a shim against all blueprints.
template< template< typename > typename shim >
void benchmarks( unsigned int run )
{
  #ifdef BLUEPRINT_1
  benchmark< shim, BLUEPRINT_1 >( run );
  #endif
  #ifdef BLUEPRINT_2
  benchmark< shim, BLUEPRINT_2 >( run );
  #endif
  #ifdef BLUEPRINT_3
  benchmark< shim, BLUEPRINT_3 >( run );
  #endif
  #ifdef BLUEPRINT_4
  benchmark< shim, BLUEPRINT_4 >( run );
  #endif
  #ifdef BLUEPRINT_5
  benchmark< shim, BLUEPRINT_5 >( run );
  #endif
  #ifdef BLUEPRINT_6
  benchmark< shim, BLUEPRINT_6 >( run );
  #endif
  #ifdef BLUEPRINT_7
  benchmark< shim, BLUEPRINT_7 >( run );
  #endif
  #ifdef BLUEPRINT_8
  benchmark< shim, BLUEPRINT_8 >( run );
  #endif
  #ifdef BLUEPRINT_9
  benchmark< shim, BLUEPRINT_9 >( run );
  #endif
  #ifdef BLUEPRINT_10
  benchmark< shim, BLUEPRINT_10 >( run );
  #endif
  #ifdef BLUEPRINT_11
  benchmark< shim, BLUEPRINT_11 >( run );
  #endif
  #ifdef BLUEPRINT_12
  benchmark< shim, BLUEPRINT_12 >( run );
  #endif
  #ifdef BLUEPRINT_13
  benchmark< shim, BLUEPRINT_13 >( run );
  #endif
  #ifdef BLUEPRINT_14
  benchmark< shim, BLUEPRINT_14 >( run );
  #endif
  #ifdef BLUEPRINT_15
  benchmark< shim, BLUEPRINT_15 >( run );
  #endif
  #ifdef BLUEPRINT_16
  benchmark< shim, BLUEPRINT_16 >( run );
  #endif
}

// Function for determining the average result across all runs for a particular measurement, excluding the number of
// highest and lowest results set via DISCARDED_RUNS_COUNT.
template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
double adjusted_average_result( size_t result_index )
{
  std::array< uint64_t, RUN_COUNT > results_temp;

  for( size_t run_index = 0; run_index < RUN_COUNT; ++run_index )
    results_temp[ run_index ] = results< shim, blueprint, benchmark_id >( run_index, result_index );

  std::sort(
    results_temp.begin(),
    results_temp.end(),
    []( uint64_t &left, uint64_t &right )
    {
      return left < right;
    }
  );

  return (double)std::accumulate(
    results_temp.begin() + DISCARDED_RUNS_COUNT / 2,
    results_temp.end() - DISCARDED_RUNS_COUNT / 2,
    (uint64_t)0
  ) / ( results_temp.size() - DISCARDED_RUNS_COUNT );
}

// Function for determining the maximum adjusted average result for an entire plot.
// This is necessary to determine graph scaling based on the visible plots.
template< template< typename> typename shim, typename blueprint, benchmark_ids benchmark_id >
double max_adjusted_average_result()
{
  double max = 0;
  for( size_t result_index = 0; result_index < KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL; ++result_index )
  {
    double adjusted_average = adjusted_average_result< shim, blueprint, benchmark_id >( result_index );
    if( adjusted_average > max )
      max = adjusted_average;
  }

  return max;
}

// Function for generating a unique id for each plot across the entire outputted HTML document.
size_t plot_id_counter;
template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id > size_t plot_id()
{
  static const size_t id = plot_id_counter++;
  return id;
}

// Function for outputting the CSS for a particular plot.
template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
void graph_shim_styling_out( std::ofstream &file )
{
  file <<
    "  \n"
    "  #Plot" << plot_id< shim, blueprint, benchmark_id >() << "Checkbox + label:before\n"
    "  {\n"
    "    background-color: " << shim< void >::color << ";\n"
    "    border-color: " << shim< void >::color << ";\n"
    "  }\n"
    "  \n"
    "  #Plot" << plot_id< shim, blueprint, benchmark_id >() << "Checkbox:not(:checked) + label:before\n"
    "  {\n"
    "    background-color: transparent;\n"
    "  }\n"
    "  \n"
    "  #Plot" << plot_id< shim, blueprint, benchmark_id >() << "Checkbox:not(:checked) ~ svg #Plot" <<
         plot_id< shim, blueprint, benchmark_id >() << "Line\n"
    "  {\n"
    "    visibility: hidden;\n"
    "  }\n"
    "  \n"
    "  #Plot" << plot_id< shim, blueprint, benchmark_id >() << "Line\n"
    "  {\n"
    "    stroke: " << shim< void >::color << ";\n"
    "  }\n"
    "  \n"
    "  #Plot" << plot_id< shim, blueprint, benchmark_id >() << "Checkbox + label:hover ~ svg #Plot" <<
         plot_id< shim, blueprint, benchmark_id >() << "Line\n"
    "  {\n"
    "    stroke-width: 3;\n"
    "  }\n"
  ;
}

// Function for outputting a shim's label at the top of the graph.
template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
void graph_shim_label_out( std::ofstream &file )
{
  file <<
    "      <input id='Plot" << plot_id< shim, blueprint, benchmark_id >() <<
             "Checkbox' type='checkbox' checked='true'/><label for='Plot" << plot_id< shim, blueprint, benchmark_id >()
             << "Checkbox'>" << shim< void >::label << "</label>\n"
  ;
}

// Function for outputting a shim's actual plot.
template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
void graph_shim_plot_out( std::ofstream &file )
{
  file <<
    "          <polyline id='Plot" << plot_id< shim, blueprint, benchmark_id >() <<
                  "Line' class='plotLine' vector-effect='non-scaling-stroke' points='"
  ;

  for( size_t result = 0; result < KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL; ++result )
    file << ( result + 1 ) * KEY_COUNT_MEASUREMENT_INTERVAL << "," <<
      adjusted_average_result< shim, blueprint, benchmark_id >( result ) << " ";

  file << "'/>\n";
}

// Function to help output x-axis labels with thousands separators.
std::string uint_to_string_with_separators( uint64_t value )
{
    std::string result = std::to_string( value );
    int pos = result.length() - 3;

    while( pos > 0 )
    {
        result.insert( pos, 1, ',' );
        pos -= 3;
    }

    return result;
}

// Function for outputting one graph.
template< typename blueprint, benchmark_ids benchmark_id > void graph_out( std::ofstream &file )
{
  // Output the general styling.
  file <<
    "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='1002px'"
       " viewBox='0 0 1002 618' style='background-color: white;'>\n"
    "  <style>\n"
    "  .outerDiv\n"
    "  {\n"
    "    padding: 6px;\n"
    "    width: 100%;\n"
    "    height: 100%;\n"
    "    box-sizing: border-box;\n"
    "    line-height: 0;\n"
    "    text-align: center;\n"
    "  }\n"
    "  \n"
    "  .verticalSpacer\n"
    "  {\n"
    "    height: 6px;\n"
    "  }\n"
    "  \n"
    "  .heading\n"
    "  {\n"
    "    height: 18px;\n"
    "    font: 14px sans-serif;\n"
    "    line-height: 18px;\n"
    "  }\n"
    "  \n"
    "  input\n"
    "  {\n"
    "    position: absolute;\n"
    "    display: none;\n"
    "  }\n"
    "  \n"
    "  input + label:before\n"
    "  {\n"
    "    content: '';\n"
    "    display: inline-block;\n"
    "    width: 12px;\n"
    "    height: 12px;\n"
    "    border-radius: 6px;\n"
    "    box-sizing: border-box;\n"
    "    border-width: 1px;\n"
    "    border-style: solid;\n"
    "    margin-right: 3px;\n"
    "    vertical-align: bottom;\n"
    "  }\n"
    "  \n"
    "  input:checked + label:before\n"
    "  {\n"
    "    background-color: transparent;\n"
    "  }\n"
    "  \n"
    "  label\n"
    "  {\n"
    "    height: 12px;\n"
    "    font: 12px sans-serif;\n"
    "    line-height: 12px;\n"
    "    margin-right: 3px;\n"
    "    cursor: pointer;\n"
    "    user-select: none;\n"
    "  }\n"
    "  \n"
    "  text\n"
    "  {\n"
    "    fill: black;\n"
    "  }\n"
    "  \n"
    "  .innerSVG\n"
    "  {\n"
    "    width: 990px;\n"
    "    height: 564px;\n"
    "  }\n"
    "  \n"
    "  .axis\n"
    "  {\n"
    "    stroke: black;\n"
    "  }\n"
    "  \n"
    "  .axisLabel\n"
    "  {\n"
    "    font: 12px sans-serif;\n"
    "  }\n"
    "  \n"
    "  .plotLine\n"
    "  {\n"
    "    fill: none;\n"
    "  }\n"
  ;

  // Output the styling for each shim's label and plot.
  #ifdef SHIM_1
  graph_shim_styling_out< SHIM_1, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_2
  graph_shim_styling_out< SHIM_2, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_3
  graph_shim_styling_out< SHIM_3, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_4
  graph_shim_styling_out< SHIM_4, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_5
  graph_shim_styling_out< SHIM_5, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_6
  graph_shim_styling_out< SHIM_6, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_7
  graph_shim_styling_out< SHIM_7, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_8
  graph_shim_styling_out< SHIM_8, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_9
  graph_shim_styling_out< SHIM_9, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_10
  graph_shim_styling_out< SHIM_10, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_11
  graph_shim_styling_out< SHIM_11, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_12
  graph_shim_styling_out< SHIM_12, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_13
  graph_shim_styling_out< SHIM_13, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_14
  graph_shim_styling_out< SHIM_14, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_15
  graph_shim_styling_out< SHIM_15, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_16
  graph_shim_styling_out< SHIM_16, blueprint, benchmark_id >( file );
  #endif

  // Output the ideal graph y-axis scale for each plot.
  // This requires outputting the scale styling in increasing order so that the graph's scale is ultimately determined
  // by the visible plot with the highest scale requirement.
  // The advantage of this approach is that it only requires CSS.
  // Consequently, the SVG can be embedded (albeit without interactivity) in markdown files displayed in GitHub
  // repositories.

  std::vector< std::pair< size_t, double > > max_results;

  #ifdef SHIM_1
  max_results.emplace_back(
    plot_id< SHIM_1, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_1, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_2
  max_results.emplace_back(
    plot_id< SHIM_2, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_2, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_3
  max_results.emplace_back(
    plot_id< SHIM_3, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_3, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_4
  max_results.emplace_back(
    plot_id< SHIM_4, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_4, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_5
  max_results.emplace_back(
    plot_id< SHIM_5, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_5, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_6
  max_results.emplace_back(
    plot_id< SHIM_6, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_6, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_7
  max_results.emplace_back(
    plot_id< SHIM_7, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_7, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_8
  max_results.emplace_back(
    plot_id< SHIM_8, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_8, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_9
  max_results.emplace_back(
    plot_id< SHIM_9, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_9, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_10
  max_results.emplace_back(
    plot_id< SHIM_10, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_10, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_11
  max_results.emplace_back(
    plot_id< SHIM_11, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_11, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_12
  max_results.emplace_back(
    plot_id< SHIM_12, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_12, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_13
  max_results.emplace_back(
    plot_id< SHIM_13, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_13, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_14
  max_results.emplace_back(
    plot_id< SHIM_14, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_14, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_15
  max_results.emplace_back(
    plot_id< SHIM_15, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_15, blueprint, benchmark_id >()
  );
  #endif
  #ifdef SHIM_16
  max_results.emplace_back(
    plot_id< SHIM_16, blueprint, benchmark_id >(),
    max_adjusted_average_result< SHIM_16, blueprint, benchmark_id >()
  );
  #endif

  std::sort(
    max_results.begin(),
    max_results.end(),
    []( std::pair< size_t, double > &left, std::pair< size_t, double > &right )
    {
      return left.second < right.second;
    }
  );

  for( auto &max_result: max_results )
  {
    file <<
      "  \n"
      "  #Plot" << max_result.first << "Checkbox:checked ~ svg g\n"
      "  {\n"
      "    transform: translate( 18px, 540px ) scale( " << 972.0 / KEY_COUNT << ", -" << 540.0 / ( max_result.second *
             1.05 ) << " );\n"
      "  }\n"
    ;
  }

  // Output all labels.

  file <<
    "  </style>\n"
    "  <foreignObject x='0' y='0' width='100%' height='100%'>\n"
    "    <div xmlns='http://www.w3.org/1999/xhtml' class='outerDiv'>\n"
    "      <div class='heading'>" << blueprint::label << ": " << benchmark_names[ benchmark_id ] << "</div>\n"
    "      <div class='verticalSpacer'></div>\n"
  ;

  #ifdef SHIM_1
  graph_shim_label_out< SHIM_1, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_2
  graph_shim_label_out< SHIM_2, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_3
  graph_shim_label_out< SHIM_3, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_4
  graph_shim_label_out< SHIM_4, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_5
  graph_shim_label_out< SHIM_5, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_6
  graph_shim_label_out< SHIM_6, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_7
  graph_shim_label_out< SHIM_7, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_8
  graph_shim_label_out< SHIM_8, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_9
  graph_shim_label_out< SHIM_9, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_10
  graph_shim_label_out< SHIM_10, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_11
  graph_shim_label_out< SHIM_11, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_12
  graph_shim_label_out< SHIM_12, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_13
  graph_shim_label_out< SHIM_13, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_14
  graph_shim_label_out< SHIM_14, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_15
  graph_shim_label_out< SHIM_15, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_16
  graph_shim_label_out< SHIM_16, blueprint, benchmark_id >( file );
  #endif

  // Output the actual graph.

  file <<
    "      <div xmlns='http://www.w3.org/1999/xhtml' class='verticalSpacer'></div>\n"
    "      <svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' class='innerSVG'>\n"
    "        <line class='axis' x1='18' x2='18' y1='0' y2='540'/>\n"
    "        <line class='axis' x1='18' x2='990' y1='540' y2='540'/>\n"
    "        <text class='axisLabel' x='6' y='270' alignment-baseline='middle' text-anchor='middle'"
               " transform='rotate(-90,6,270)'>Linear time from zero &#10230;</text>\n"
    "        <text class='axisLabel' x='504' y='558' alignment-baseline='middle' text-anchor='middle'>N keys</text>\n"
  ;

  // Output the x-axis' five key-count labels.
  for( unsigned int i = 1; i < 5; ++i )
  {
    unsigned int x = 18 + 972 * ( i / 5.0 );

    file <<
      "        <line x1='" << x << "' x2='" << x << "' y1='540' y2='542' stroke='black'/>\n"
    ;

    file <<
      "        <text x='" << x << "' y='" << 548 << "' alignment-baseline='middle' text-anchor='middle'"
        " style='font: 12px sans-serif;'>" << uint_to_string_with_separators( i * (uint64_t)( KEY_COUNT / 5 ) ) <<
        "</text>\n"
    ;
  }

  // Output each shim's plot.

  file <<
    "        <g>\n";
  ;

  #ifdef SHIM_1
  graph_shim_plot_out< SHIM_1, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_2
  graph_shim_plot_out< SHIM_2, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_3
  graph_shim_plot_out< SHIM_3, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_4
  graph_shim_plot_out< SHIM_4, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_5
  graph_shim_plot_out< SHIM_5, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_6
  graph_shim_plot_out< SHIM_6, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_7
  graph_shim_plot_out< SHIM_7, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_8
  graph_shim_plot_out< SHIM_8, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_9
  graph_shim_plot_out< SHIM_9, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_10
  graph_shim_plot_out< SHIM_10, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_11
  graph_shim_plot_out< SHIM_11, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_12
  graph_shim_plot_out< SHIM_12, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_13
  graph_shim_plot_out< SHIM_13, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_14
  graph_shim_plot_out< SHIM_14, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_15
  graph_shim_plot_out< SHIM_15, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_16
  graph_shim_plot_out< SHIM_16, blueprint, benchmark_id >( file );
  #endif

  // Finish up.
  file <<
    "        </g>\n"
    "      </svg>\n"
    "    </div>\n"
    "  </foreignObject>\n"
    "</svg>\n"
  ;
}

// Function for outputting all the graphs corresponding to a particular benchmark.
template< benchmark_ids benchmark_id > void graphs_out( std::ofstream &file )
{
  #ifdef BLUEPRINT_1
  graph_out< BLUEPRINT_1, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_2
  graph_out< BLUEPRINT_2, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_3
  graph_out< BLUEPRINT_3, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_4
  graph_out< BLUEPRINT_4, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_5
  graph_out< BLUEPRINT_5, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_6
  graph_out< BLUEPRINT_6, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_7
  graph_out< BLUEPRINT_7, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_8
  graph_out< BLUEPRINT_8, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_9
  graph_out< BLUEPRINT_9, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_10
  graph_out< BLUEPRINT_10, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_11
  graph_out< BLUEPRINT_11, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_12
  graph_out< BLUEPRINT_12, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_13
  graph_out< BLUEPRINT_13, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_14
  graph_out< BLUEPRINT_14, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_15
  graph_out< BLUEPRINT_15, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_16
  graph_out< BLUEPRINT_16, benchmark_id >( file );
  #endif
}

template< template< typename > typename shim >
void heatmap_shim_label_out( std::ofstream &file, unsigned int col, double cell_width )
{
  file << "  <text x='" << 200 + ( col + 0.5 ) * cell_width << "' y='37' text-anchor='middle'>" << shim< void >::label
       << "</text>\n"
  ;
}

// Function for determining the total elapsed time measured for a particular benchmark.
// In all benchmarks besides INSERT_NONEXISTING, this is the sum of all the plot's data points displayed in the graph.
template< template< typename> typename shim, typename blueprint, benchmark_ids benchmark_id >
double total_adjusted_average_result()
{
  // Since the INSERT_NOEXISTING benchmark is already cumulative, there's no need to total the results for each
  // measurement taken in the benchmark.
  // Instead, we just use the final measurement.
  if( benchmark_id == insert_nonexisting )
    return adjusted_average_result< shim, blueprint, benchmark_id >( KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL - 1 );

  // Otherwise, sum all displayed data points.
  double total = 0.0;
  for( size_t result = 0; result < KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL; ++result )
    total += adjusted_average_result< shim, blueprint, benchmark_id >( result );

  return total;
}

// Function for outputting an individual heatmap cell.
template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
void heatmap_cell_out( std::ofstream &file, unsigned int row, unsigned int col, double cell_width, double lowest_total )
{
  double normalized_total = total_adjusted_average_result<shim, blueprint, benchmark_id>() / lowest_total;

  double x = 200 + col * cell_width;
  double y = 44 + row * 32;

  static const uint8_t colors [ 256 ][ 3 ] =
  {
    { 255, 255, 255 }, { 255, 255, 237 }, { 255, 255, 217 }, { 255, 255, 195 }, { 255, 255, 171 }, { 255, 255, 152 },
    { 254, 253, 149 }, { 254, 252, 145 }, { 254, 250, 142 }, { 254, 249, 139 }, { 254, 247, 136 }, { 254, 246, 132 },
    { 254, 244, 129 }, { 254, 242, 126 }, { 254, 241, 123 }, { 254, 239, 120 }, { 254, 238, 117 }, { 254, 236, 113 },
    { 254, 234, 110 }, { 255, 233, 107 }, { 255, 231, 104 }, { 255, 229, 102 }, { 255, 228, 100 }, { 255, 226, 98 },
    { 255, 224, 97 }, { 255, 223, 95 }, { 255, 221, 93 }, { 255, 219, 92 }, { 255, 218, 90 }, { 255, 216, 88 },
    { 255, 214, 87 }, { 255, 213, 85 }, { 255, 211, 84 }, { 255, 209, 82 }, { 255, 208, 80 }, { 255, 206, 79 },
    { 255, 204, 77 }, { 255, 203, 75 }, { 255, 201, 74 }, { 255, 199, 72 }, { 255, 198, 70 }, { 255, 196, 69 },
    { 255, 194, 67 }, { 255, 193, 65 }, { 255, 191, 63 }, { 255, 189, 62 }, { 255, 188, 60 }, { 255, 186, 58 },
    { 255, 184, 57 }, { 255, 182, 55 }, { 255, 181, 53 }, { 255, 179, 52 }, { 255, 177, 50 }, { 255, 176, 48 },
    { 255, 174, 46 }, { 255, 172, 45 }, { 255, 170, 43 }, { 255, 169, 41 }, { 255, 167, 39 }, { 255, 165, 38 },
    { 255, 163, 36 }, { 255, 162, 34 }, { 255, 160, 32 }, { 255, 158, 30 }, { 255, 156, 29 }, { 255, 154, 27 },
    { 255, 153, 25 }, { 255, 151, 23 }, { 255, 149, 21 }, { 255, 147, 19 }, { 255, 145, 18 }, { 255, 143, 16 },
    { 255, 141, 14 }, { 255, 139, 12 }, { 255, 137, 10 }, { 255, 135, 8 }, { 255, 133, 6 }, { 255, 131, 4 },
    { 255, 130, 4 }, { 254, 128, 5 }, { 253, 127, 6 }, { 252, 126, 5 }, { 251, 125, 5 }, { 250, 124, 5 },
    { 249, 123, 4 }, { 248, 122, 4 }, { 247, 120, 4 }, { 246, 119, 3 }, { 245, 118, 3 }, { 244, 117, 3 },
    { 243, 116, 3 }, { 242, 115, 2 }, { 241, 114, 2 }, { 240, 113, 2 }, { 239, 112, 2 }, { 238, 111, 2 },
    { 237, 110, 1 }, { 236, 109, 1 }, { 235, 108, 1 }, { 234, 107, 1 }, { 233, 106, 1 }, { 232, 104, 1 },
    { 231, 103, 1 }, { 230, 102, 1 }, { 229, 101, 0 }, { 228, 100, 0 }, { 227, 99, 0 }, { 226, 98, 0 },
    { 225, 97, 0 }, { 224, 96, 0 }, { 223, 95, 0 }, { 222, 94, 0 }, { 221, 93, 0 }, { 220, 92, 0 },
    { 218, 91, 0 }, { 217, 90, 0 }, { 216, 89, 0 }, { 215, 88, 0 }, { 214, 87, 0 }, { 213, 86, 0 },
    { 212, 85, 0 }, { 211, 84, 0 }, { 210, 83, 0 }, { 209, 82, 0 }, { 208, 81, 0 }, { 207, 80, 0 },
    { 206, 79, 0 }, { 205, 78, 0 }, { 204, 77, 0 }, { 203, 76, 0 }, { 202, 75, 0 }, { 201, 74, 0 },
    { 200, 73, 0 }, { 199, 72, 0 }, { 198, 71, 0 }, { 197, 70, 0 }, { 196, 69, 1 }, { 195, 68, 1 },
    { 194, 67, 1 }, { 193, 66, 1 }, { 192, 65, 1 }, { 191, 64, 1 }, { 190, 63, 1 }, { 189, 62, 1 },
    { 188, 61, 1 }, { 187, 60, 1 }, { 186, 59, 1 }, { 185, 58, 1 }, { 183, 57, 1 }, { 182, 56, 1 },
    { 181, 55, 1 }, { 180, 54, 1 }, { 179, 53, 1 }, { 178, 52, 1 }, { 177, 51, 1 }, { 176, 50, 1 },
    { 175, 49, 1 }, { 174, 48, 1 }, { 173, 47, 1 }, { 172, 46, 1 }, { 171, 45, 1 }, { 170, 44, 1 },
    { 169, 43, 1 }, { 168, 42, 1 }, { 167, 41, 1 }, { 166, 40, 1 }, { 164, 39, 1 }, { 163, 38, 1 },
    { 162, 37, 1 }, { 161, 35, 1 }, { 160, 34, 1 }, { 159, 33, 1 }, { 158, 32, 1 }, { 157, 31, 1 },
    { 156, 30, 1 }, { 155, 29, 1 }, { 154, 28, 1 }, { 153, 27, 1 }, { 151, 26, 1 }, { 150, 26, 2 },
    { 148, 26, 3 }, { 147, 26, 3 }, { 145, 25, 4 }, { 144, 25, 4 }, { 143, 25, 5 }, { 141, 24, 5 },
    { 140, 24, 5 }, { 138, 24, 6 }, { 137, 23, 6 }, { 136, 23, 6 }, { 134, 23, 7 }, { 133, 22, 7 },
    { 131, 22, 7 }, { 130, 22, 7 }, { 129, 21, 8 }, { 127, 21, 8 }, { 126, 20, 8 }, { 125, 20, 8 },
    { 123, 19, 8 }, { 122, 19, 8 }, { 121, 19, 8 }, { 119, 18, 8 }, { 118, 18, 8 }, { 116, 18, 7 },
    { 115, 17, 7 }, { 114, 17, 7 }, { 112, 16, 7 }, { 111, 16, 6 }, { 110, 15, 6 }, { 108, 15, 6 },
    { 107, 14, 6 }, { 106, 14, 5 }, { 104, 13, 5 }, { 103, 13, 5 }, { 102, 12, 5 }, { 101, 12, 4 },
    { 99, 11, 4 }, { 98, 10, 4 }, { 97, 10, 4 }, { 95, 9, 3 }, { 94, 8, 3 }, { 93, 8, 3 },
    { 92, 7, 3 }, { 90, 6, 2 }, { 89, 6, 2 }, { 88, 5, 2 }, { 87, 5, 2 }, { 85, 4, 1 },
    { 84, 3, 1 }, { 83, 3, 1 }, { 82, 2, 1 }, { 80, 2, 1 }, { 79, 1, 0 }, { 78, 1, 0 },
    { 77, 0, 0 }, { 75, 0, 0 }, { 74, 0, 0 }, { 72, 0, 0 }, { 71, 0, 0 }, { 69, 0, 0 },
    { 68, 0, 0 }, { 66, 0, 0 }, { 65, 0, 0 }, { 63, 0, 0 }, { 62, 0, 0 }, { 60, 0, 0 },
    { 59, 0, 0 }, { 57, 0, 0 }, { 55, 0, 0 }, { 54, 0, 0 }, { 52, 0, 0 }, { 48, 4, 1 },
    { 42, 9, 4 }, { 36, 13, 8 }, { 29, 16, 13 }, { 24, 17, 16 }
  };

  uint8_t color = std::min( ( normalized_total - 1.0 ) / 9.0, 1.0 ) * 255;

  file << "  <rect x='" << x + 1 << "' y='" << y + 1 << "' width='" << cell_width - 2 << "' height='30' "
       <<     "style='fill: rgb(" << (unsigned int)colors[ color ][ 0 ] << "," << (unsigned int)colors[ color ][ 1 ]
       <<     "," << (unsigned int)colors[ color ][ 2 ] << " ) !important;'/>\n"
  ;

  file << "  <text x='" << x + cell_width * 0.5 << "' y='" << y + 16 + 1 << "' text-anchor='middle' "
       <<     "style='fill: " << ( normalized_total >= 5.0 ? "white" : "black" ) << " !important;'>" << normalized_total
       <<     ( benchmark_id == erase_existing && shim< void >::tombstone_like_mechanism ? "&#10013;" : "" )
       <<     "</text>\n"
  ;
}

// Function for outputting an entire heatmap row.
template< typename blueprint, benchmark_ids benchmark_id >
void heatmap_row_out( std::ofstream &file, unsigned int row, double cell_width )
{
  double lowest_total = std::numeric_limits<double>::max();

  #ifdef SHIM_1
  lowest_total = std::min( total_adjusted_average_result< SHIM_1, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_2
  lowest_total = std::min( total_adjusted_average_result< SHIM_2, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_3
  lowest_total = std::min( total_adjusted_average_result< SHIM_3, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_4
  lowest_total = std::min( total_adjusted_average_result< SHIM_4, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_5
  lowest_total = std::min( total_adjusted_average_result< SHIM_5, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_6
  lowest_total = std::min( total_adjusted_average_result< SHIM_6, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_7
  lowest_total = std::min( total_adjusted_average_result< SHIM_7, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_8
  lowest_total = std::min( total_adjusted_average_result< SHIM_8, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_9
  lowest_total = std::min( total_adjusted_average_result< SHIM_9, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_10
  lowest_total = std::min( total_adjusted_average_result< SHIM_10, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_11
  lowest_total = std::min( total_adjusted_average_result< SHIM_11, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_12
  lowest_total = std::min( total_adjusted_average_result< SHIM_12, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_13
  lowest_total = std::min( total_adjusted_average_result< SHIM_13, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_14
  lowest_total = std::min( total_adjusted_average_result< SHIM_14, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_15
  lowest_total = std::min( total_adjusted_average_result< SHIM_15, blueprint, benchmark_id >(), lowest_total );
  #endif
  #ifdef SHIM_16
  lowest_total = std::min( total_adjusted_average_result< SHIM_16, blueprint, benchmark_id >(), lowest_total );
  #endif

  double center_y = 44 + ( row + 0.5 ) * 32 + 1;

  file << "  <text x='7' y='" << center_y - 7 << "'>" << blueprint::label << ":</text>\n"
          "  <text x='7' y='" << center_y + 7 << "'>" << benchmark_alt_names[ benchmark_id ] << "</text>\n"
  ;

  int col = 0;

  #ifdef SHIM_1
  heatmap_cell_out< SHIM_1, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_2
  heatmap_cell_out< SHIM_2, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_3
  heatmap_cell_out< SHIM_3, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_4
  heatmap_cell_out< SHIM_4, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_5
  heatmap_cell_out< SHIM_5, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_6
  heatmap_cell_out< SHIM_6, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_7
  heatmap_cell_out< SHIM_7, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_8
  heatmap_cell_out< SHIM_8, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_9
  heatmap_cell_out< SHIM_9, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_10
  heatmap_cell_out< SHIM_10, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_11
  heatmap_cell_out< SHIM_11, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_12
  heatmap_cell_out< SHIM_12, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_13
  heatmap_cell_out< SHIM_13, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_14
  heatmap_cell_out< SHIM_14, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_15
  heatmap_cell_out< SHIM_15, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
  #ifdef SHIM_16
  heatmap_cell_out< SHIM_16, blueprint, benchmark_id >( file, row, col++, cell_width, lowest_total );
  #endif
}

// Function for outputting all the heatmap rows that correspond to one benchmark.
template< benchmark_ids benchmark_id >
void heatmap_rows_out( std::ofstream &file, unsigned int &row, double cell_width )
{
  #ifdef BLUEPRINT_1
  heatmap_row_out< BLUEPRINT_1, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_2
  heatmap_row_out< BLUEPRINT_2, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_3
  heatmap_row_out< BLUEPRINT_3, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_4
  heatmap_row_out< BLUEPRINT_4, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_5
  heatmap_row_out< BLUEPRINT_5, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_6
  heatmap_row_out< BLUEPRINT_6, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_7
  heatmap_row_out< BLUEPRINT_7, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_8
  heatmap_row_out< BLUEPRINT_8, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_9
  heatmap_row_out< BLUEPRINT_9, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_10
  heatmap_row_out< BLUEPRINT_10, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_11
  heatmap_row_out< BLUEPRINT_11, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_12
  heatmap_row_out< BLUEPRINT_12, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_13
  heatmap_row_out< BLUEPRINT_13, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_14
  heatmap_row_out< BLUEPRINT_14, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_15
  heatmap_row_out< BLUEPRINT_15, benchmark_id >( file, row++, cell_width );
  #endif
  #ifdef BLUEPRINT_16
  heatmap_row_out< BLUEPRINT_16, benchmark_id >( file, row++, cell_width );
  #endif
}

// Function for outputting the entire heatmap.
void heatmap_out( std::ofstream &file )
{
  unsigned int cell_cols = 0;

  #ifdef SHIM_1
  ++cell_cols;
  #endif
  #ifdef SHIM_2
  ++cell_cols;
  #endif
  #ifdef SHIM_3
  ++cell_cols;
  #endif
  #ifdef SHIM_4
  ++cell_cols;
  #endif
  #ifdef SHIM_5
  ++cell_cols;
  #endif
  #ifdef SHIM_6
  ++cell_cols;
  #endif
  #ifdef SHIM_7
  ++cell_cols;
  #endif
  #ifdef SHIM_8
  ++cell_cols;
  #endif
  #ifdef SHIM_9
  ++cell_cols;
  #endif
  #ifdef SHIM_10
  ++cell_cols;
  #endif
  #ifdef SHIM_11
  ++cell_cols;
  #endif
  #ifdef SHIM_12
  ++cell_cols;
  #endif
  #ifdef SHIM_13
  ++cell_cols;
  #endif
  #ifdef SHIM_14
  ++cell_cols;
  #endif
  #ifdef SHIM_15
  ++cell_cols;
  #endif
  #ifdef SHIM_16
  ++cell_cols;
  #endif

  double cell_width = 797 / (double)cell_cols;

  unsigned int benchmarks = 0;

  #ifdef BENCHMARK_INSERT_NONEXISTING
  ++benchmarks;
  #endif
  #ifdef BENCHMARK_ERASE_EXISTING
  ++benchmarks;
  #endif
  #ifdef BENCHMARK_INSERT_EXISTING
  ++benchmarks;
  #endif
  #ifdef BENCHMARK_ERASE_NONEXISTING
  ++benchmarks;
  #endif
  #ifdef BENCHMARK_GET_EXISTING
  ++benchmarks;
  #endif
  #ifdef BENCHMARK_GET_NONEXISTING
  ++benchmarks;
  #endif
  #ifdef BENCHMARK_ITERATION
  ++benchmarks;
  #endif

  unsigned int cell_rows = 0;

  #ifdef BLUEPRINT_1
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_2
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_3
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_4
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_5
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_6
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_7
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_8
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_9
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_10
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_11
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_12
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_13
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_14
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_15
  cell_rows += benchmarks;
  #endif
  #ifdef BLUEPRINT_16
  cell_rows += benchmarks;
  #endif

  file << "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='1002px' "
            "viewBox='0 0 1002 " << 44 + cell_rows * 32 + 5 + 12 + 5 << "' style='background-color: white;'>\n"
          "  <style>\n"
          "  text\n"
          "  {\n"
          "    fill: black;\n"
          "    font: 12px sans-serif;\n"
          "    dominant-baseline: middle;\n"
          "  }\n"
          "  </style>\n"
          "  <text x='501' y='16' style='font-size: 14px;' text-anchor='middle'>Total time taken relative to the "
               "fastest table in the benchmark (lower/lighter is better)</text>\n"
  ;

  // Output shim labels.

  unsigned int col = 0;

  #ifdef SHIM_1
  heatmap_shim_label_out< SHIM_1 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_2
  heatmap_shim_label_out< SHIM_2 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_3
  heatmap_shim_label_out< SHIM_3 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_4
  heatmap_shim_label_out< SHIM_4 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_5
  heatmap_shim_label_out< SHIM_5 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_6
  heatmap_shim_label_out< SHIM_6 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_7
  heatmap_shim_label_out< SHIM_7 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_8
  heatmap_shim_label_out< SHIM_8 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_9
  heatmap_shim_label_out< SHIM_9 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_10
  heatmap_shim_label_out< SHIM_10 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_11
  heatmap_shim_label_out< SHIM_11 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_12
  heatmap_shim_label_out< SHIM_12 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_13
  heatmap_shim_label_out< SHIM_13 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_14
  heatmap_shim_label_out< SHIM_14 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_15
  heatmap_shim_label_out< SHIM_15 >( file, col++, cell_width );
  #endif
  #ifdef SHIM_16
  heatmap_shim_label_out< SHIM_16 >( file, col++, cell_width );
  #endif

  // Output rows.

  file << std::fixed << std::setprecision( 2 );

  unsigned int row = 0;

  #ifdef BENCHMARK_INSERT_NONEXISTING
  heatmap_rows_out< insert_nonexisting >( file, row, cell_width );
  #endif
  #ifdef BENCHMARK_ERASE_EXISTING
  heatmap_rows_out< erase_existing >( file, row, cell_width );
  #endif
  #ifdef BENCHMARK_INSERT_EXISTING
  heatmap_rows_out< insert_existing >( file, row, cell_width );
  #endif
  #ifdef BENCHMARK_ERASE_NONEXISTING
  heatmap_rows_out< erase_nonexisting >( file, row, cell_width );
  #endif
  #ifdef BENCHMARK_GET_EXISTING
  heatmap_rows_out< get_existing >( file, row, cell_width );
  #endif
  #ifdef BENCHMARK_GET_NONEXISTING
  heatmap_rows_out< get_nonexisting >( file, row, cell_width );
  #endif
  #ifdef BENCHMARK_ITERATION
  heatmap_rows_out< iteration >( file, row, cell_width );
  #endif

  file << "  <text x='995' y='"<< 44 + cell_rows * 32 + 5 + 6
       <<    "' text-anchor='end'>&#10013; Relies on tombstones or a tombstone-like mechanism</text>"
  ;

  file << "</svg>\n";
}

// Function for outputting both the graphs and the heatmap as a HTML file.
void html_out( std::string &file_id )
{
  // Use time as filename.
  std::ofstream file( "result_" + file_id + ".html" );

  file << "<!doctype html>\n"
       << "<html>\n"
       << "<head>\n"
       << "<meta charset='utf-8'/>\n"
       << "<title>Benchmark Results</title>\n"
       << "</head>\n"
       << "<body style='background-color: rgb( 35, 33, 29 );  text-align: center;'>\n"
       << "<div style='display: inline-block; background-color: rgb( 256, 256, 256 ); width: 978px; padding: 12px; " <<
             "margin-top: 8px; margin-bottom: 8px; line-height: 1.5; text-align: left;'>\n"
       << "<div style='font-family: sans-serif; font-weight: bold;'>Settings</div>\n"
       << "<div style='font-family: monospace;'>"
       << "KEY_COUNT: " << KEY_COUNT << "<br>\n"
       << "KEY_COUNT_MEASUREMENT_INTERVAL: " << KEY_COUNT_MEASUREMENT_INTERVAL << "<br>\n"
       << "RUN_COUNT: " << RUN_COUNT << "<br>\n"
       << "DISCARDED_RUNS_COUNT: " << DISCARDED_RUNS_COUNT << "<br>\n"
       << "MAX_LOAD_FACTOR: " << MAX_LOAD_FACTOR << "<br>\n"
       << "APPROXIMATE_CACHE_SIZE: " << APPROXIMATE_CACHE_SIZE << "<br>\n"
       << "MILLISECOND_COOLDOWN_BETWEEN_BENCHMARKS: " << MILLISECOND_COOLDOWN_BETWEEN_BENCHMARKS << '\n'
       << "</div>\n"
       << "</div>\n"
  ;

  #ifdef BENCHMARK_INSERT_NONEXISTING
  graphs_out< insert_nonexisting >( file );
  #endif
  #ifdef BENCHMARK_ERASE_EXISTING
  graphs_out< erase_existing >( file );
  #endif
  #ifdef BENCHMARK_INSERT_EXISTING
  graphs_out< insert_existing >( file );
  #endif
  #ifdef BENCHMARK_ERASE_NONEXISTING
  graphs_out< erase_nonexisting >( file );
  #endif
  #ifdef BENCHMARK_GET_EXISTING
  graphs_out< get_existing >( file );
  #endif
  #ifdef BENCHMARK_GET_NONEXISTING
  graphs_out< get_nonexisting >( file );
  #endif
  #ifdef BENCHMARK_ITERATION
  graphs_out< iteration >( file );
  #endif

  heatmap_out( file );


  file << "</body>\n"
       << "</html>\n";
}

// Functions for outputting raw data as a CSV file.

template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
void csv_shim_out( std::ofstream &file )
{
  file << blueprint::label << ":" << benchmark_names[ benchmark_id ] << ":" << shim< void >::label << "\n";

  for( unsigned int run_index = 0; run_index < RUN_COUNT; ++run_index )
  {
    file << "Run " << run_index << ";";

    for( size_t result_index = 0; result_index < KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL; ++result_index )
      file << results< shim, blueprint, benchmark_id >( run_index, result_index ) << ";";

    file << "\n";
  }

  file << "Adjusted average;";
  for( size_t result_index = 0; result_index < KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL; ++result_index )
    file << adjusted_average_result< shim, blueprint, benchmark_id >( result_index ) << ";";
  file << "\n\n";
}

template< typename blueprint, benchmark_ids benchmark_id > void csv_blueprint_out( std::ofstream &file )
{
  #ifdef SHIM_1
  csv_shim_out< SHIM_1, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_2
  csv_shim_out< SHIM_2, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_3
  csv_shim_out< SHIM_3, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_4
  csv_shim_out< SHIM_4, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_5
  csv_shim_out< SHIM_5, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_6
  csv_shim_out< SHIM_6, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_7
  csv_shim_out< SHIM_7, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_8
  csv_shim_out< SHIM_8, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_9
  csv_shim_out< SHIM_9, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_10
  csv_shim_out< SHIM_10, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_11
  csv_shim_out< SHIM_11, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_12
  csv_shim_out< SHIM_12, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_13
  csv_shim_out< SHIM_13, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_14
  csv_shim_out< SHIM_14, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_15
  csv_shim_out< SHIM_15, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_16
  csv_shim_out< SHIM_16, blueprint, benchmark_id >( file );
  #endif
}

template< benchmark_ids benchmark_id > void csv_benchmark_out( std::ofstream &file )
{
  #ifdef BLUEPRINT_1
  csv_blueprint_out< BLUEPRINT_1, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_2
  csv_blueprint_out< BLUEPRINT_2, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_3
  csv_blueprint_out< BLUEPRINT_3, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_4
  csv_blueprint_out< BLUEPRINT_4, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_5
  csv_blueprint_out< BLUEPRINT_5, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_6
  csv_blueprint_out< BLUEPRINT_6, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_7
  csv_blueprint_out< BLUEPRINT_7, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_8
  csv_blueprint_out< BLUEPRINT_8, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_9
  csv_blueprint_out< BLUEPRINT_9, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_10
  csv_blueprint_out< BLUEPRINT_10, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_11
  csv_blueprint_out< BLUEPRINT_11, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_12
  csv_blueprint_out< BLUEPRINT_12, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_13
  csv_blueprint_out< BLUEPRINT_13, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_14
  csv_blueprint_out< BLUEPRINT_14, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_15
  csv_blueprint_out< BLUEPRINT_15, benchmark_id >( file );
  #endif
  #ifdef BLUEPRINT_16
  csv_blueprint_out< BLUEPRINT_16, benchmark_id >( file );
  #endif
}

void csv_out( std::string &file_id )
{
  std::ofstream file( "result_" + file_id + ".csv" );

  file << "sep=;\n";

  file << "N;";
  for( size_t result_index = 0; result_index < KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL; ++result_index )
    file << ( result_index + 1 ) * KEY_COUNT_MEASUREMENT_INTERVAL << ";";
  file << "\n\n";

  #ifdef BENCHMARK_INSERT_NONEXISTING
  csv_benchmark_out< insert_nonexisting >( file );
  #endif
  #ifdef BENCHMARK_ERASE_EXISTING
  csv_benchmark_out< erase_existing >( file );
  #endif
  #ifdef BENCHMARK_INSERT_EXISTING
  csv_benchmark_out< insert_existing >( file );
  #endif
  #ifdef BENCHMARK_ERASE_NONEXISTING
  csv_benchmark_out< erase_nonexisting >( file );
  #endif
  #ifdef BENCHMARK_GET_EXISTING
  csv_benchmark_out< get_existing >( file );
  #endif
  #ifdef BENCHMARK_GET_NONEXISTING
  csv_benchmark_out< get_nonexisting >( file );
  #endif
  #ifdef BENCHMARK_ITERATION
  csv_benchmark_out< iteration >( file );
  #endif
}

// Program entry.

int main()
{
  for( unsigned int run = 0; run < RUN_COUNT; ++run )
  {
    std::cout << "Run " << run << '\n';

    #ifdef SHIM_1
    benchmarks< SHIM_1 >( run );
    #endif
    #ifdef SHIM_2
    benchmarks< SHIM_2 >( run );
    #endif
    #ifdef SHIM_3
    benchmarks< SHIM_3 >( run );
    #endif
    #ifdef SHIM_4
    benchmarks< SHIM_4 >( run );
    #endif
    #ifdef SHIM_5
    benchmarks< SHIM_5 >( run );
    #endif
    #ifdef SHIM_6
    benchmarks< SHIM_6 >( run );
    #endif
    #ifdef SHIM_7
    benchmarks< SHIM_7 >( run );
    #endif
    #ifdef SHIM_8
    benchmarks< SHIM_8 >( run );
    #endif
    #ifdef SHIM_9
    benchmarks< SHIM_9 >( run );
    #endif
    #ifdef SHIM_10
    benchmarks< SHIM_10 >( run );
    #endif
    #ifdef SHIM_11
    benchmarks< SHIM_11 >( run );
    #endif
    #ifdef SHIM_12
    benchmarks< SHIM_12 >( run );
    #endif
    #ifdef SHIM_13
    benchmarks< SHIM_13 >( run );
    #endif
    #ifdef SHIM_14
    benchmarks< SHIM_14 >( run );
    #endif
    #ifdef SHIM_15
    benchmarks< SHIM_15 >( run );
    #endif
    #ifdef SHIM_16
    benchmarks< SHIM_16 >( run );
    #endif
  }

  std::cout << "Outputting results\n";

  // Get UTC time string with colons replaced by underscores.
  auto itt = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
  std::ostringstream ss;
  ss << std::put_time( gmtime( &itt), "%Y-%m-%dT%H:%M:%S" );
  auto time_str = ss.str();
  std::replace( time_str.begin(), time_str.end(), ':', '_' );

  html_out( time_str );

  csv_out( time_str );

  std::cout << "Done\n";
}
