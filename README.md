## Introduction

This repository contains a comparative, extendible benchmarking suite for C and C++ hash map libraries.

The benchmarks test the speed of inserting keys, replacing keys, looking up existing keys, look up nonexisting keys, deleting existing keys, deleting nonexisting keys, and iteration.

The following is an example of one outputted graph:

<picture><img src="example_graph.svg" alt="Example graph"></picture>

More results can be seen here: [20 000 000 keys](https://verstablebenchmarks.netlify.app/result_2024-01-15T13_50_24_20000000.html), [2 000 000 keys](https://verstablebenchmarks.netlify.app/result_2024-01-15T20_00_27_2000000.html), [200 000 keys](https://verstablebenchmarks.netlify.app/result_2024-01-15T20_44_58_200000.html).

## Installing

1. Download and extract the repository.

2. Install [Boost](https://www.boost.org/) if necessary, or disable the `boost_unordered_flat_map` shim by editing `config.h`.

## Compiling

Using GCC, compile with `g++ -I. -std=c++20 -O3 -Wall -Wpedantic main.cpp` from the master directory.

## Running

Close background processes, lock your CPU's frequency, and then run the executable. Under the out-of-the-box configuration, the benchmarks take approximately 45 minutes to complete on my AMD Ryzen 7 5800H with the CPU frequency locked at 90%.

The resulting graphs are outputted to the `results` directory as a HMTL file named with a GMT timestamp.

The graphs are interactive. Hover over a label to highlight the associated plot, and click the label to toggle the plot's visibility. The graphs automatically scale to the visible plots.

## Configuring

Modify global settings, including the total key count, the measurement frequency, the maximum load factor, and the blueprints and shims (see below) enabled by editing `config.h`.

## Built-in maps

The following hash map libraries are included out-of-the-box:

* TODO ONCE FINALIZED

## Built-in blueprints

A _blueprint_ is a combination of a key type, value type, hash function, comparison function, and key set against which to measure each map's performance.

Three blueprints are included out-of-the-box:

- The `uint32_uint32_murmur` blueprint (_32-bit integer key, 32-bit value_) tests how the maps perform when the hash function and key comparison function are cheap, traversing buckets is cheap (i.e. does not cause many cache misses), and moving keys and values is cheap. This blueprint disadvantages maps that store metadata in a separate array because doing so necessarily causes at least one extra cache miss per lookup.

 - The `uint64_struct448_murmur` blueprint (_64-bit integer key, 448-bit value_) tests how the maps perform when the hash function and key comparison function are cheap, traversing buckets is expensive (typically a cache miss per bucket), and moving keys and values is expensive. This blueprint disadvantages maps that do not store metadata in a separate array (or do but access the buckets array with every probe anyway to check the key) and that move elements around often (e.g. Robin Hood).

- The `cstring_uint64_fnv1a` (_16-char c-string key, 64-bit value_) blueprint tests how the maps perform when the hash function and key comparison function are expensive. This blueprint disadvantages maps that lack a (metadata) mechanism to avoid most key comparisons or that rehash existing keys often.

## Adding a new map (via a shim)

Each hash map library plugs into the benchmarks via a custom _shim_ that provides a standard API for basic hash map operations.

To add a new map, follow these steps:

1. Create a directory, with your chosen name for the shim, in the `shims` directory and, ideally, install the hash map library's headers there.

2. Create a file name `shim.h` in the new directory.

3. Insert the name of the shim to an empty shim slot in `config.h`.

4. In `shim.h`, define a struct template with the name of the shim (`new_shim` in the below documentation) and that satisfies the following requirements:

    `new_shim< void >` should contain these members:

    ```c++
    static constexpr const char *label =
      // A string literal containing the label of the map to appear in the outputted graphs.
    ;
 
    static constexpr const char *color =
      // A string literal containing the color of the map's label and plot to appear in the outputted
      //graphs, e.g. rgb( 255, 0, 0 ).
    ;
    ```

    For each blueprint, `new_shim< blueprint >` should contain the following members, where `blueprint` is the name of the blueprint, `map_type` is the map type for that blueprint, and `itr_type` is the type of the associated iterator:

    ```c++
    static map_type create_map()
    {
      // Returns an initialized instance of a map that uses blueprint::hash_key as the hash function,
      // blueprint::cmpr_keys as the comparison function, and MAX_LOAD_FACTOR as the maximum load
      // factor.
    }
    
    static void insert( map_type &map, const blueprint::key_type &key )
    {
      // Inserts the specified key, along with a dummy value, into the map, replacing any matching
      // key already in the map if it exists.
    }

    static void erase( map_type &map, const blueprint::key_type &key )
    {
      // Erases the specified key and associated value from the map, if the key exists.
    }
    
    static itr_type find( map_type &map, const blueprint::key_type &key )
    {
      // Returns an iterator to the specified key and associated value, if the key exists,
      // or an iterator indicating a nonexisting key (e.g. an end iterator, for maps that follow the
      // std::unordered_map API), if the key does not exist.
    }
    
    static itr_type begin_itr( map_type &map )
    {
      // Returns an iterator to the first key and associated value in the map, or an iterator
      // indicating a nonexisting key if the map is empty.
    }
    
    static bool is_itr_valid( map_type &map, itr_type &itr )
    {
      // Returns true if the specified iterator points to a key inside the map.
    }

    static void increment_itr( map_type &map, itr_type &itr )
    {
      // Increments the specified iterator to point to the next key in the map, or an iterator
      // indicating a nonexisting key if key to which the iterator currently points is the last one
      // in the map.
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
      // For C++ maps, if freeing memory is handled by map_type's destructor, this function can be
      // left empty.
    }

Refer to the built-in shims for examples of how a shim can be implemented for C++ maps that follow the `std::unordered_map` API and for C maps, which typically require explicit template specializations for each blueprint.

## Adding a new blueprint

To add a new blueprint, follow these steps:

1. Create a directory with your chosen name for the blueprint in the `blueprint` directory.

2. Create a file name `blueprint.h` in the new directory.

3. Insert the name of the blueprint into an empty blueprint slot in `config.h`.

4. In `blueprint.h`, define a struct with the name of the blueprint that contains the following members:

    ```c++
    static constexpr const char *label =
      // A string literal containing the label of the blueprint toappear in the outputted graphs.
    ;
    
    using key_type =
      // The type of the keys that the maps should contain.
    ;
    
    using value_type =
      // The value associated with each key.
    ;
    
    static uint64_t hash_key( const key_type &key )
    {
      // Returns the hash code of the specified key.
    }
    
    bool cmpr_keys( const key_type &key_1, const key_type &key_2 )
    {
      // Returns true if the two specified keys are equal.
    }
    
    void fill_unique_keys( std::vector< blueprint::key_type > &keys )
    {
      // Fills the specified vector with a set of unique keys.
      // The size vector is already initialized with the size KEY_COUNT + KEY_COUNT /
      // KEY_COUNT_MEASUREMENT_INTERVAL * 1000.
      // The order is unimportant because the keys will be shuffled.
      // This function is called exactly once.
    }
    
    ```

    `blueprint.h` should also declare a suitably named macro (e.g. `NEW_SHIM_ENABLED`) that allows each shim to use the preprocessor to optionally exclude a specialization for the blueprint.

5. Ensure that each shim supports the new blueprint struct as a template argument (either via the base template or via an explicit template specialization).

## Sharing results

The graphs embedded in the outputted HTML file are self-contained SVGs, so they can be trivially extracted and turned into standalone SVG files or embedded into other HTML files. They can even be used in GitHub READMEs (via `<img src="./graph.svg">`), albeit without the interactive features.
