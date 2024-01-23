// hash_map_benchmarks/config.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

// The total number of unique keys to include in the benchmarks.
#define KEY_COUNT 20000000

// The frequency at which to measure and record times.
// This must be a factor of KEY_COUNT.
#define KEY_COUNT_MEASUREMENT_INTERVAL 50000

// The number of times to repeat the benchmarks.
#define RUN_COUNT 12

// Each data point in the outputted graphs is the average of the measurements for that point across all runs, excluding
// the lowest and highest measurements.
// This macro specifies the total number of measurements to discard (e.g. 2 means that the lowest measurement and
// highest measurement will be discarded).
// It should be an even number below RUN_COUNT.
// This mechanism reduces the impact of background noise interfering with the benchmarks.
#define DISCARDED_RUNS_COUNT 2

// The maximum load factor that all shims should, in theory, apply when initializing their maps.
// In reality, some hash map libraries do not allow custom load factors.
#define MAX_LOAD_FACTOR 0.875

// The size (in bytes) of the buffer to iterate over between maps in order to reset the state of the L1, L2, and L3 CPU
// caches.
// This mechanism is intended to ensure that each map starts the benchmarks under approximately the same condition with
// regard to whether the keys are cached.
#define APPROXIMATE_CACHE_SIZE 20000000

// The amount of time to sleep between benchmarks.
#define MILLISECOND_COOLDOWN_BETWEEN_BENCHMARKS 1000

// The specific benchmarks to run (comment them out to disable them).
#define BENCHMARK_INSERT_NONEXISTING
#define BENCHMARK_ERASE_EXISTING
#define BENCHMARK_INSERT_EXISTING
#define BENCHMARK_ERASE_NONEXISTING
#define BENCHMARK_GET_EXISTING
#define BENCHMARK_GET_NONEXISTING
#define BENCHMARK_ITERATION

// Blueprint slots.
#define BLUEPRINT_1 uint32_uint32_murmur
#define BLUEPRINT_2 uint64_struct448_murmur
#define BLUEPRINT_3 cstring_uint64_fnv1a
// #define BLUEPRINT_4
// #define BLUEPRINT_5
// #define BLUEPRINT_6
// #define BLUEPRINT_7
// #define BLUEPRINT_8
// #define BLUEPRINT_9
// #define BLUEPRINT_10
// #define BLUEPRINT_11
// #define BLUEPRINT_12
// #define BLUEPRINT_13
// #define BLUEPRINT_14
// #define BLUEPRINT_15
// #define BLUEPRINT_16

// Shim slots.
#define SHIM_1 absl_flat_hash_map
#define SHIM_2 boost_unordered_flat_map
#define SHIM_3 ankerl_unordered_dense
#define SHIM_4 robin_hood_unordered_map
#define SHIM_5 tsl_robin_map
#define SHIM_6 ska_bytell_hash_map
#define SHIM_7 khash
#define SHIM_8 verstable
#define SHIM_9 stc_hmap
#define SHIM_10 cc_lib_map
// #define SHIM_11
// #define SHIM_12
// #define SHIM_13
// #define SHIM_14
// #define SHIM_15
// #define SHIM_16
