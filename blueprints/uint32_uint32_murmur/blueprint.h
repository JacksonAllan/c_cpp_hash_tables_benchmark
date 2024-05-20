// c_cpp_hash_tables_benchmark/blueprints/uint32_uint32_murmur/blueprint.h
// Copyright (c) 2024 Jackson L. Allan.
// Distributed under the MIT License (see the accompanying LICENSE file).

#include <numeric>

#define UINT32_UINT32_MURMUR_ENABLED

struct uint32_uint32_murmur
{
  using key_type = uint32_t;
  using value_type = uint32_t;
  static constexpr const char *label = "32-bit integer key, 32-bit value";
  
  // MurmurHash3’s 64-bit finalizer.
  static uint64_t hash_key( const key_type &key )
  {
    uint64_t result = key;
    result ^= result >> 33;
    result *= 0xff51afd7ed558ccdull;
    result ^= result >> 33;
    result *= 0xc4ceb9fe1a85ec53ull;
    result ^= result >> 33;
    return result;
  }

  static bool cmpr_keys( const key_type &key_1, const key_type &key_2 )
  {
    return key_1 == key_2;
  }

  static void fill_unique_keys( std::vector<key_type> &keys )
  {
    std::iota( keys.begin(), keys.end(), 0 );
  }
};
