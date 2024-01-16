// hash_map_benchmarks/main.cpp
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
  std::same_as< decltype( shim< void >::color ), const char * const >
;

// Concept to check that a shim is correctly formed in relation to a specific blueprint.
template<
  template< typename > typename shim,
  typename blueprint,
  // Helpers to simplify the concept's definition:
  typename map_type = decltype( shim< blueprint >::create_map() ),
  typename itr_type = decltype( shim< blueprint >::begin_itr( std::declval< map_type & >() ) )
>
concept check_shim_against_blueprint =
  std::same_as< decltype( shim< blueprint >::create_map ), map_type () > &&
  std::same_as< decltype( shim< blueprint >::insert ), void ( map_type &, const typename blueprint::key_type & ) > &&
  std::same_as< decltype( shim< blueprint >::erase ), void ( map_type &, const typename blueprint::key_type & ) > &&
  std::same_as< decltype( shim< blueprint >::find ), itr_type ( map_type &, const typename blueprint::key_type & ) > &&
  std::same_as< decltype( shim< blueprint >::begin_itr ), itr_type ( map_type & ) > &&
  std::same_as< decltype( shim< blueprint >::is_itr_valid ), bool ( map_type &, itr_type & ) > &&
  std::same_as< decltype( shim< blueprint >::increment_itr ), void ( map_type &, itr_type & ) > &&
  std::same_as< 
    decltype( shim< blueprint >::get_key_from_itr ),
    const typename blueprint::key_type &( map_type &, itr_type & )
  > &&
  std::same_as< 
    decltype( shim< blueprint >::get_value_from_itr ),
    const typename blueprint::value_type &( map_type &, itr_type & )
  > &&
  std::same_as< decltype( shim< blueprint >::destroy_map ), void ( map_type & ) >
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

// Function for recording and accessing a single result, i.e. one measurement for a particular map, blueprint, and
// benchmark during a particular run.
template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
uint64_t &results( size_t run_index, size_t result_index )
{
  static auto results = std::vector< uint64_t >( RUN_COUNT * ( KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL ) );
  return results[ run_index * ( KEY_COUNT / KEY_COUNT_MEASUREMENT_INTERVAL ) + result_index ];
}

// Function that attempts to reset the cache to the same state before each map is benchmarked.
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

  volatile unsigned char do_not_optimize;

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
  // This ensures that each map starts each benchmarking run on equal footing, but it does not prevent the cache effects
  // of one benchmark from potentially influencing latter benchmarks.
  flush_cache();

  #ifdef BENCHMARK_INSERT_NONEXISTING
  {
    auto map = shim< blueprint >::create_map();

    std::this_thread::sleep_for( std::chrono::milliseconds( MILLISECOND_COOLDOWN_BETWEEN_BENCHMARKS ) );

    size_t i = 0;
    size_t j = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while( i < KEY_COUNT )
    {
      shim< blueprint >::insert( map, shuffled_unique_key< blueprint >( i ) );

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

    shim< blueprint >::destroy_map( map );
  }
  #endif

  #ifdef BENCHMARK_ERASE_EXISTING
  {
    auto map = shim< blueprint >::create_map();

    std::this_thread::sleep_for( std::chrono::milliseconds( MILLISECOND_COOLDOWN_BETWEEN_BENCHMARKS ) );

    size_t i = 0;
    size_t j = 0;
    while( i < KEY_COUNT )
    {
      shim< blueprint >::insert( map, shuffled_unique_key< blueprint >( i ) );

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
          shim< blueprint >::erase( map, shuffled_unique_key< blueprint >( l ) );

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
          shim< blueprint >::insert( map, shuffled_unique_key< blueprint >( l ) );
 
          if( ++k == 1000 )
            break;
          if( ++l == i )
            l = 0;
        }

        j = 0;
      }
    }

    shim< blueprint >::destroy_map( map );
  }
  #endif

  #if                                         \
    defined( BENCHMARK_INSERT_EXISTING )   || \
    defined( BENCHMARK_ERASE_NONEXISTING ) || \
    defined( BENCHMARK_GET_EXISTING )      || \
    defined( BENCHMARK_GET_NONEXISTING )   || \
    defined( BENCHMARK_ITERATION )
  {
    auto map = shim< blueprint >::create_map();

    std::this_thread::sleep_for( std::chrono::milliseconds( MILLISECOND_COOLDOWN_BETWEEN_BENCHMARKS ) );

    size_t i = 0;
    size_t j = 0;
    while( i < KEY_COUNT )
    {
      shim< blueprint >::insert( map, shuffled_unique_key< blueprint >( i ) );

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
            shim< blueprint >::insert( map, shuffled_unique_key< blueprint >( l ) );

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
            shim< blueprint >::erase( map, shuffled_unique_key< blueprint >( l ) );

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
            auto itr = shim< blueprint >::find( map, shuffled_unique_key< blueprint >( l ) );

            // Accessing the first byte of the value prevents the above call from being optimized out and ensures that
            // maps that can preform look-ups without accessing the values (because the keys are stored separately)
            // actually incur the additional cache miss that they would incur during normal use.
            do_not_optimize = *(unsigned char *)&shim< blueprint >::get_value_from_itr( map, itr );

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
            auto itr = shim< blueprint >::find( map, shuffled_unique_key< blueprint >( l ) );
            do_not_optimize = *(unsigned char *)&itr;

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
          // To determine where inside the map to begin iteration, we randomly choose an existing key.
          // This ensures that we are not just hitting the same, cached memory every time we measure.
          auto itr = shim< blueprint >::find(
            map,
            shuffled_unique_key< blueprint >(
              std::uniform_int_distribution<size_t>( 0, i - 1 )( random_number_generator )
            )
          );

          size_t k = 0;
          auto start = std::chrono::high_resolution_clock::now();
          while( true )
          {
            // Accessing the first bytes of the key and value ensures that maps that iterate without directly accessing
            // the keys and/or values actually incur the additional cache misses that they would incur during normal
            // use.
            do_not_optimize = *(unsigned char *)&shim< blueprint >::get_key_from_itr( map, itr );
            do_not_optimize = *(unsigned char *)&shim< blueprint >::get_value_from_itr( map, itr );

            if( ++k == 1000 )
              break;

            shim< blueprint >::increment_itr( map, itr );
            if( !shim< blueprint >::is_itr_valid( map, itr ) )
              itr = shim< blueprint >::begin_itr( map );
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

    shim< blueprint >::destroy_map( map );
  }
  #endif

  (void)do_not_optimize;
}

// Functions for outputting an SVG graph for a particular benchmark.

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

template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
void svg_shim_styling_out( std::ofstream &file )
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

template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
void svg_shim_label_out( std::ofstream &file )
{
  file <<
    "      <input id='Plot" << plot_id< shim, blueprint, benchmark_id >() <<
             "Checkbox' type='checkbox' checked='true'/><label for='Plot" << plot_id< shim, blueprint, benchmark_id >()
             << "Checkbox'>" << shim< void >::label << "</label>\n"
  ;
}

template< template< typename > typename shim, typename blueprint, benchmark_ids benchmark_id >
void svg_shim_plot_out( std::ofstream &file )
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

template< typename blueprint, benchmark_ids benchmark_id > void svg_out( std::ofstream &file )
{
  // Output the general styling.
  file <<
    "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='1002px' height='618px'"
      " style='background-color: white'>\n"
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
  svg_shim_styling_out< SHIM_1, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_2
  svg_shim_styling_out< SHIM_2, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_3
  svg_shim_styling_out< SHIM_3, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_4
  svg_shim_styling_out< SHIM_4, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_5
  svg_shim_styling_out< SHIM_5, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_6
  svg_shim_styling_out< SHIM_6, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_7
  svg_shim_styling_out< SHIM_7, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_8
  svg_shim_styling_out< SHIM_8, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_9
  svg_shim_styling_out< SHIM_9, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_10
  svg_shim_styling_out< SHIM_10, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_11
  svg_shim_styling_out< SHIM_11, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_12
  svg_shim_styling_out< SHIM_12, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_13
  svg_shim_styling_out< SHIM_13, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_14
  svg_shim_styling_out< SHIM_14, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_15
  svg_shim_styling_out< SHIM_15, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_16
  svg_shim_styling_out< SHIM_16, blueprint, benchmark_id >( file );
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
    max_average_result< SHIM_12, blueprint, benchmark_id >()
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
    "      <div class='heading'>" << blueprint::label << ": " << std::array{
               "Total time to insert N nonexisting keys",
               "Total time to erase 1000 existing keys with N keys in the map",
               "Time to replace 1000 existing keys with N keys in the map",
               "Time to erase 1000 nonexisting keys with N keys in the map",
               "Time to lookup 1000 existing keys with N keys in the map",
               "Time to lookup 1000 nonexisting keys with N keys in the map",
               "Time to iterate over 5000 keys with N keys in the map"
             }[ benchmark_id ] << "</div>\n"
    "      <div class='verticalSpacer'></div>\n"
  ;

  #ifdef SHIM_1
  svg_shim_label_out< SHIM_1, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_2
  svg_shim_label_out< SHIM_2, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_3
  svg_shim_label_out< SHIM_3, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_4
  svg_shim_label_out< SHIM_4, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_5
  svg_shim_label_out< SHIM_5, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_6
  svg_shim_label_out< SHIM_6, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_7
  svg_shim_label_out< SHIM_7, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_8
  svg_shim_label_out< SHIM_8, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_9
  svg_shim_label_out< SHIM_9, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_10
  svg_shim_label_out< SHIM_10, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_11
  svg_shim_label_out< SHIM_11, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_12
  svg_shim_label_out< SHIM_12, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_13
  svg_shim_label_out< SHIM_13, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_14
  svg_shim_label_out< SHIM_14, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_15
  svg_shim_label_out< SHIM_15, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_16
  svg_shim_label_out< SHIM_16, blueprint, benchmark_id >( file );
  #endif

  // Output the actual graph.

  file <<
    "      <div xmlns='http://www.w3.org/1999/xhtml' class='verticalSpacer'></div>\n"
    "      <svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' class='innerSVG'>\n"
    "        <line class='axis' x1='18' x2='18' y1='0' y2='540'></line>\n"
    "        <line class='axis' x1='18' x2='990' y1='540' y2='540'></line>\n"
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
        " style='font: 12px sans-serif;'>" << i * (uint64_t)( KEY_COUNT / 5 ) << "</text>\n"
    ;
  }

  // Output each shim's plot.

  file <<
    "        <g>\n";
  ;

  #ifdef SHIM_1
  svg_shim_plot_out< SHIM_1, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_2
  svg_shim_plot_out< SHIM_2, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_3
  svg_shim_plot_out< SHIM_3, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_4
  svg_shim_plot_out< SHIM_4, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_5
  svg_shim_plot_out< SHIM_5, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_6
  svg_shim_plot_out< SHIM_6, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_7
  svg_shim_plot_out< SHIM_7, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_8
  svg_shim_plot_out< SHIM_8, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_9
  svg_shim_plot_out< SHIM_9, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_10
  svg_shim_plot_out< SHIM_10, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_11
  svg_shim_plot_out< SHIM_11, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_12
  svg_shim_plot_out< SHIM_12, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_13
  svg_shim_plot_out< SHIM_13, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_14
  svg_shim_plot_out< SHIM_14, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_15
  svg_shim_plot_out< SHIM_15, blueprint, benchmark_id >( file );
  #endif
  #ifdef SHIM_16
  svg_shim_plot_out< SHIM_16, blueprint, benchmark_id >( file );
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

int main()
{
  // Run the benchmarks.
  for( unsigned int run = 0; run < RUN_COUNT; ++run )
  {
    std::cout << "Run " << run << '\n';

    #ifdef SHIM_1
    #ifdef BLUEPRINT_1
    benchmark< SHIM_1, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_1, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_1, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_1, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_1, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_1, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_1, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_1, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_1, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_1, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_1, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_1, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_1, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_1, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_1, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_1, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_2
    #ifdef BLUEPRINT_1
    benchmark< SHIM_2, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_2, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_2, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_2, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_2, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_2, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_2, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_2, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_2, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_2, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_2, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_2, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_2, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_2, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_2, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_2, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_3
    #ifdef BLUEPRINT_1
    benchmark< SHIM_3, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_3, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_3, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_3, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_3, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_3, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_3, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_3, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_3, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_3, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_3, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_3, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_3, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_3, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_3, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_3, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_4
    #ifdef BLUEPRINT_1
    benchmark< SHIM_4, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_4, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_4, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_4, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_4, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_4, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_4, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_4, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_4, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_4, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_4, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_4, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_4, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_4, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_4, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_4, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_5
    #ifdef BLUEPRINT_1
    benchmark< SHIM_5, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_5, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_5, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_5, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_5, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_5, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_5, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_5, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_5, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_5, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_5, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_5, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_5, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_5, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_5, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_5, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_6
    #ifdef BLUEPRINT_1
    benchmark< SHIM_6, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_6, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_6, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_6, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_6, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_6, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_6, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_6, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_6, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_6, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_6, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_6, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_6, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_6, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_6, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_6, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_7
    #ifdef BLUEPRINT_1
    benchmark< SHIM_7, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_7, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_7, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_7, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_7, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_7, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_7, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_7, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_7, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_7, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_7, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_7, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_7, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_7, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_7, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_7, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_8
    #ifdef BLUEPRINT_1
    benchmark< SHIM_8, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_8, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_8, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_8, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_8, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_8, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_8, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_8, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_8, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_8, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_8, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_8, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_8, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_8, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_8, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_8, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_9
    #ifdef BLUEPRINT_1
    benchmark< SHIM_9, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_9, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_9, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_9, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_9, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_9, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_9, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_9, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_9, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_9, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_9, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_9, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_9, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_9, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_9, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_9, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_10
    #ifdef BLUEPRINT_1
    benchmark< SHIM_10, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_10, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_10, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_10, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_10, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_10, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_10, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_10, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_10, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_10, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_10, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_10, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_10, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_10, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_10, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_10, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_11
    #ifdef BLUEPRINT_1
    benchmark< SHIM_11, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_11, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_11, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_11, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_11, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_11, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_11, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_11, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_11, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_11, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_11, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_11, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_11, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_11, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_11, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_11, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_12
    #ifdef BLUEPRINT_1
    benchmark< SHIM_12, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_12, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_12, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_12, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_12, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_12, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_12, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_12, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_12, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_12, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_12, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_12, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_12, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_12, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_12, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_12, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_13
    #ifdef BLUEPRINT_1
    benchmark< SHIM_13, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_13, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_13, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_13, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_13, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_13, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_13, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_13, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_13, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_13, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_13, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_13, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_13, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_13, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_13, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_13, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_14
    #ifdef BLUEPRINT_1
    benchmark< SHIM_14, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_14, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_14, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_14, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_14, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_14, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_14, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_14, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_14, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_14, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_14, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_14, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_14, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_14, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_14, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_14, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_15
    #ifdef BLUEPRINT_1
    benchmark< SHIM_15, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_15, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_15, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_15, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_15, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_15, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_15, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_15, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_15, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_15, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_15, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_15, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_15, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_15, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_15, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_15, BLUEPRINT_16 >( run );
    #endif
    #endif

    #ifdef SHIM_16
    #ifdef BLUEPRINT_1
    benchmark< SHIM_16, BLUEPRINT_1 >( run );
    #endif
    #ifdef BLUEPRINT_2
    benchmark< SHIM_16, BLUEPRINT_2 >( run );
    #endif
    #ifdef BLUEPRINT_3
    benchmark< SHIM_16, BLUEPRINT_3 >( run );
    #endif
    #ifdef BLUEPRINT_4
    benchmark< SHIM_16, BLUEPRINT_4 >( run );
    #endif
    #ifdef BLUEPRINT_5
    benchmark< SHIM_16, BLUEPRINT_5 >( run );
    #endif
    #ifdef BLUEPRINT_6
    benchmark< SHIM_16, BLUEPRINT_6 >( run );
    #endif
    #ifdef BLUEPRINT_7
    benchmark< SHIM_16, BLUEPRINT_7 >( run );
    #endif
    #ifdef BLUEPRINT_8
    benchmark< SHIM_16, BLUEPRINT_8 >( run );
    #endif
    #ifdef BLUEPRINT_9
    benchmark< SHIM_16, BLUEPRINT_9 >( run );
    #endif
    #ifdef BLUEPRINT_10
    benchmark< SHIM_16, BLUEPRINT_10 >( run );
    #endif
    #ifdef BLUEPRINT_11
    benchmark< SHIM_16, BLUEPRINT_11 >( run );
    #endif
    #ifdef BLUEPRINT_12
    benchmark< SHIM_16, BLUEPRINT_12 >( run );
    #endif
    #ifdef BLUEPRINT_13
    benchmark< SHIM_16, BLUEPRINT_13 >( run );
    #endif
    #ifdef BLUEPRINT_14
    benchmark< SHIM_16, BLUEPRINT_14 >( run );
    #endif
    #ifdef BLUEPRINT_15
    benchmark< SHIM_16, BLUEPRINT_15 >( run );
    #endif
    #ifdef BLUEPRINT_16
    benchmark< SHIM_16, BLUEPRINT_16 >( run );
    #endif
    #endif
  }

  // Output the HTML file containing the results.

  std::cout << "Outputting results\n";

  // Get UTC time string with colons replaced by underscores.
  auto itt = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
  std::ostringstream ss;
  ss << std::put_time( gmtime( &itt), "%Y-%m-%dT%H:%M:%S" );
  auto time_str = ss.str();
  std::replace( time_str.begin(), time_str.end(), ':', '_' );

  // Use time as filename.
  std::ofstream file( "results/result_" + time_str + ".html" );

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
       << "</div>"
       << "</div>"
  ;

  #ifdef BENCHMARK_INSERT_NONEXISTING
  #ifdef BLUEPRINT_1
  svg_out< BLUEPRINT_1, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_2
  svg_out< BLUEPRINT_2, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_3
  svg_out< BLUEPRINT_3, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_4
  svg_out< BLUEPRINT_4, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_5
  svg_out< BLUEPRINT_5, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_6
  svg_out< BLUEPRINT_6, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_7
  svg_out< BLUEPRINT_7, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_8
  svg_out< BLUEPRINT_8, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_9
  svg_out< BLUEPRINT_9, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_10
  svg_out< BLUEPRINT_10, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_11
  svg_out< BLUEPRINT_11, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_12
  svg_out< BLUEPRINT_12, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_13
  svg_out< BLUEPRINT_13, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_14
  svg_out< BLUEPRINT_14, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_15
  svg_out< BLUEPRINT_15, insert_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_16
  svg_out< BLUEPRINT_16, insert_nonexisting >( file );
  #endif
  #endif

  #ifdef BENCHMARK_ERASE_EXISTING
  #ifdef BLUEPRINT_1
  svg_out< BLUEPRINT_1, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_2
  svg_out< BLUEPRINT_2, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_3
  svg_out< BLUEPRINT_3, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_4
  svg_out< BLUEPRINT_4, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_5
  svg_out< BLUEPRINT_5, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_6
  svg_out< BLUEPRINT_6, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_7
  svg_out< BLUEPRINT_7, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_8
  svg_out< BLUEPRINT_8, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_9
  svg_out< BLUEPRINT_9, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_10
  svg_out< BLUEPRINT_10, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_11
  svg_out< BLUEPRINT_11, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_12
  svg_out< BLUEPRINT_12, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_13
  svg_out< BLUEPRINT_13, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_14
  svg_out< BLUEPRINT_14, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_15
  svg_out< BLUEPRINT_15, erase_existing >( file );
  #endif
  #ifdef BLUEPRINT_16
  svg_out< BLUEPRINT_16, erase_existing >( file );
  #endif
  #endif

  #ifdef BENCHMARK_INSERT_EXISTING
  #ifdef BLUEPRINT_1
  svg_out< BLUEPRINT_1, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_2
  svg_out< BLUEPRINT_2, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_3
  svg_out< BLUEPRINT_3, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_4
  svg_out< BLUEPRINT_4, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_5
  svg_out< BLUEPRINT_5, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_6
  svg_out< BLUEPRINT_6, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_7
  svg_out< BLUEPRINT_7, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_8
  svg_out< BLUEPRINT_8, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_9
  svg_out< BLUEPRINT_9, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_10
  svg_out< BLUEPRINT_10, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_11
  svg_out< BLUEPRINT_11, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_12
  svg_out< BLUEPRINT_12, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_13
  svg_out< BLUEPRINT_13, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_14
  svg_out< BLUEPRINT_14, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_15
  svg_out< BLUEPRINT_15, insert_existing >( file );
  #endif
  #ifdef BLUEPRINT_16
  svg_out< BLUEPRINT_16, insert_existing >( file );
  #endif
  #endif

  #ifdef BENCHMARK_ERASE_NONEXISTING
  #ifdef BLUEPRINT_1
  svg_out< BLUEPRINT_1, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_2
  svg_out< BLUEPRINT_2, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_3
  svg_out< BLUEPRINT_3, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_4
  svg_out< BLUEPRINT_4, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_5
  svg_out< BLUEPRINT_5, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_6
  svg_out< BLUEPRINT_6, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_7
  svg_out< BLUEPRINT_7, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_8
  svg_out< BLUEPRINT_8, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_9
  svg_out< BLUEPRINT_9, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_10
  svg_out< BLUEPRINT_10, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_11
  svg_out< BLUEPRINT_11, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_12
  svg_out< BLUEPRINT_12, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_13
  svg_out< BLUEPRINT_13, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_14
  svg_out< BLUEPRINT_14, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_15
  svg_out< BLUEPRINT_15, erase_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_16
  svg_out< BLUEPRINT_16, erase_nonexisting >( file );
  #endif
  #endif

  #ifdef BENCHMARK_GET_EXISTING
  #ifdef BLUEPRINT_1
  svg_out< BLUEPRINT_1, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_2
  svg_out< BLUEPRINT_2, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_3
  svg_out< BLUEPRINT_3, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_4
  svg_out< BLUEPRINT_4, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_5
  svg_out< BLUEPRINT_5, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_6
  svg_out< BLUEPRINT_6, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_7
  svg_out< BLUEPRINT_7, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_8
  svg_out< BLUEPRINT_8, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_9
  svg_out< BLUEPRINT_9, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_10
  svg_out< BLUEPRINT_10, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_11
  svg_out< BLUEPRINT_11, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_12
  svg_out< BLUEPRINT_12, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_13
  svg_out< BLUEPRINT_13, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_14
  svg_out< BLUEPRINT_14, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_15
  svg_out< BLUEPRINT_15, get_existing >( file );
  #endif
  #ifdef BLUEPRINT_16
  svg_out< BLUEPRINT_16, get_existing >( file );
  #endif
  #endif

  #ifdef BENCHMARK_GET_NONEXISTING
  #ifdef BLUEPRINT_1
  svg_out< BLUEPRINT_1, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_2
  svg_out< BLUEPRINT_2, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_3
  svg_out< BLUEPRINT_3, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_4
  svg_out< BLUEPRINT_4, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_5
  svg_out< BLUEPRINT_5, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_6
  svg_out< BLUEPRINT_6, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_7
  svg_out< BLUEPRINT_7, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_8
  svg_out< BLUEPRINT_8, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_9
  svg_out< BLUEPRINT_9, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_10
  svg_out< BLUEPRINT_10, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_11
  svg_out< BLUEPRINT_11, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_12
  svg_out< BLUEPRINT_12, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_13
  svg_out< BLUEPRINT_13, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_14
  svg_out< BLUEPRINT_14, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_15
  svg_out< BLUEPRINT_15, get_nonexisting >( file );
  #endif
  #ifdef BLUEPRINT_16
  svg_out< BLUEPRINT_16, get_nonexisting >( file );
  #endif
  #endif

  #ifdef BENCHMARK_ITERATION
  #ifdef BLUEPRINT_1
  svg_out< BLUEPRINT_1, iteration >( file );
  #endif
  #ifdef BLUEPRINT_2
  svg_out< BLUEPRINT_2, iteration >( file );
  #endif
  #ifdef BLUEPRINT_3
  svg_out< BLUEPRINT_3, iteration >( file );
  #endif
  #ifdef BLUEPRINT_4
  svg_out< BLUEPRINT_4, iteration >( file );
  #endif
  #ifdef BLUEPRINT_5
  svg_out< BLUEPRINT_5, iteration >( file );
  #endif
  #ifdef BLUEPRINT_6
  svg_out< BLUEPRINT_6, iteration >( file );
  #endif
  #ifdef BLUEPRINT_7
  svg_out< BLUEPRINT_7, iteration >( file );
  #endif
  #ifdef BLUEPRINT_8
  svg_out< BLUEPRINT_8, iteration >( file );
  #endif
  #ifdef BLUEPRINT_9
  svg_out< BLUEPRINT_9, iteration >( file );
  #endif
  #ifdef BLUEPRINT_10
  svg_out< BLUEPRINT_10, iteration >( file );
  #endif
  #ifdef BLUEPRINT_11
  svg_out< BLUEPRINT_11, iteration >( file );
  #endif
  #ifdef BLUEPRINT_12
  svg_out< BLUEPRINT_12, iteration >( file );
  #endif
  #ifdef BLUEPRINT_13
  svg_out< BLUEPRINT_13, iteration >( file );
  #endif
  #ifdef BLUEPRINT_14
  svg_out< BLUEPRINT_14, iteration >( file );
  #endif
  #ifdef BLUEPRINT_15
  svg_out< BLUEPRINT_15, iteration >( file );
  #endif
  #ifdef BLUEPRINT_16
  svg_out< BLUEPRINT_16, iteration >( file );
  #endif
  #endif

  file << "</body>\n"
       << "</html>\n";

  std::cout << "Done\n";
}
