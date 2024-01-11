## Introduction

This repository contains an extendible, comparative benchmark of C and C++ hash map libraries.
An example of the output can be found [here].

## Installing

To install:

- Download and extract the repository.
- Install [Boost](...), or disable the `boost_unordered_flat_map` shim by editing `config.h`.

## Compiling

For GCC or Clang, compile with `...` or `...` from the master directory.

## Running

Close background processes, lock your CPU's frequency, and then run the executable.

Under the out-of-the-box configuration, the benchmarks take about ... to complete an on AMD Ryzen 7 5800H with the CPU frequency locked at 90%.

The resulting graphs are outputted to the 'results' directory as a HMTL file named with a GMT timestamp.

The graphs are interactive. Hover over a label to highlight the associated plot, and click the label to toggle the plot's visibility. The graphs automatically scale to the visible plots.

## Configuring

Modify global settings, including the total key count, measurement frequency, maximum load factor, and blueprints and shims (see below) enabled by editing `config.h`.

## Included blueprints

A "blueprint" is a combination of a key type and set, value type, hash function, and comparison function against which to test each map.
Three blueprints are included out-of-the-box:

- The "32-bit integer key, 32-bit value" blueprint tests how the hash tables perform when the hash function and key comparison function are cheap, traversing buckets is cheap (i.e. does not cause many cache misses), and moving elements is cheap.
This blueprint disadvantages tables that store metadata in a separate array because doing so necessarily causes at least one extra cache miss per lookup.

 - The "64-bit integer key, 448-bit value" blueprint test how the tables perform when the hash function and key comparison function are cheap, traversing buckets is expensive (a cache miss per bucket), and moving elements is expensive.
This blueprint disadvantages tables that do not store metadata in a separate array (or do but access the buckets array with every probe anyway to check the key) and that move elements around a lot (e.g. Robin Hood).

- The "16-char string key, 64-bit value" blueprint test how tables perform when the hash function and key comparison function are expensive.
This blueprint disadvantages hash tables that lack a (metadata) mechanism to avoid most key comparisons or do a lot of rehashing.

## Adding new maps

Each map plugs into the benchmarks via a custom "shim" that provides a standard interface for basic operations.

To add a new map:

1. Create a directory with your chosen name for the shim in the `shims` directory.
2. Create a file name `shim.h` in the new directory.
3. Insert the name of the shim to an empty `#define SHIM_<N>` slot in `config.h`.
4. In `shim.h`, define a struct template with the name of the shim (`shim` below) and that satisfies the following requirements:

shim< void > should provide the following members:

static constexpr const char *label = /* a string literal containing the label of the map to appear in the outputted graphs */;

// The color of the map's label and plot to appear in the outputed graphs.
static constexpr const char *color = "rgb( 250, 0, 0 )";

For each blueprint, our_shim< blueprint > should provide the following member functions, where "blueprint" is the name of the blueprint, "map_type" is the map type for that blueprint, and "itr_type" is the type of the associated itr:

static map_type create_map()
{
  // Returns an initialized instance of a map that uses blueprint::hash_key as the hash function,
  // blueprint::cmpr_keys as the comparison function, and MAX_LOAD_FACTOR as the maximum load factor.
}

static void insert( map_type &map, const blueprint::key_type &key )
{
  // Inserts the specified key, along with a dummy value, into the map, replacing any matching key already in the map if it exists.
}

static void erase( map_type &map, const blueprint::key_type &key )
{
  // Erases the specified key and associated value from the map, if the key exists.
}

static itr_type find( map_type &map, const blueprint::key_type &key )
{
  // Returns an iterator to the specified key and associated value, if the key exists,
  // or an iterator indicating a nonexistent key (e.g. an end iterator, for maps that follow the std::unordered_map API), if the key does not exist.
}

static itr_type begin_itr( map_type &map )
{
  // Returns an iterator to the first key and associated value in the map, or an iterator indicating a nonexistent key if the map is empty.
}

static bool is_itr_valid( map_type &map, itr_type &itr )
{
  // Returns true if the specified iterator points to a key inside the map.
}

static void increment_itr( map_type &map, itr_type &itr )
{
  // Increments the specified iterator to point to the next key in the map, or an iterator indicating a nonexistent key if key to which the iterator currently points is the last one in the map.
}

static blueprint::key_type &get_key_from_itr( map_type &map, itr_type &itr )
{
  // Returns a reference to the key pointed to by the specified iterator.
}

static blueprint::value_type &get_value_from_itr( map_type &map, itr_type &itr )
{
  // Returns a reference to the value pointed to by the specified iterator.
}

static void destroy_map( map_type &map )
{
  // Frees all memory associated with the map.
  // For C++ maps, if freeing memory is handled by map_type's destructor, this function can be left empty.
}

Refer to the built-in shims for examples of how a shim can be implemented for C++ maps that follow the std::unordered_map API and for C maps, which typically require explicit template specializations for each blueprint.

## Adding new blueprints

To add a new blueprint:

1. Create a directory with your chosen name for the blueprint in the `blueprint` directory.

2. Create a file name `blueprint.h` in the new directory.

3. Insert the name of the blueprint to an empty `#define BLUEPRINT_<N>` slot in `config.h`.

4. In `blueprint.h`, define a struct with the name of the blueprint (`blueprint` below) and that contains the following members:

static constexpr const char *label = /* a string literal containing the label of the blueprint to appear in the outputted graphs */;

using key_type = /* the type of the keys that the maps should contain */;

using value_type = /* the value associated with each key */;

static uint64_t hash_key( const key_type &key )
{
  // Returns the hash code of the specified key.
}

bool cmpr_keys( const key_type &key_1, const key_type &key_2 )
{
  // Returns true if the two specified keys are equal.
}

`blueprint.h` should also declare a suitably named macro (e.g. `NEW_SHIM_ENABLE`) that enables each shim to use the preprocessor to optionally disclude a specialization for the blueprint.

5. Ensure that each shim supports the new blueprint struct as a template argument (either via the base template or via an explicit template specialization).

## Sharing results

The SVG embedded in the outputted HTML file are self-contained, so they can be trivially extracted and turned into standalone SVG files or embedded into other HTML files.

They can even be used in GitHub READMEs (via ...), albeit without the interactive features.
