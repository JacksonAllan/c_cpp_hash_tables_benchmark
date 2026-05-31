/*******************************************************************************
 *  ASKL.                                                                      *
 *  Copyright (c) 2026 Raphael Prevost <raph@el.bzh>                           *
 *                                                                             *
 *  This software is a computer program whose purpose is to provide a          *
 *  framework for developing and prototyping network services.                 *
 *                                                                             *
 *  This software is governed by the CeCILL  license under French law and      *
 *  abiding by the rules of distribution of free software.  You can  use,      *
 *  modify and/ or redistribute the software under the terms of the CeCILL     *
 *  license as circulated by CEA, CNRS and INRIA at the following URL          *
 *  "http://www.cecill.info".                                                  *
 *                                                                             *
 *  As a counterpart to the access to the source code and  rights to copy,     *
 *  modify and redistribute granted by the license, users are provided only    *
 *  with a limited warranty  and the software's author,  the holder of the     *
 *  economic rights,  and the successive licensors  have only  limited         *
 *  liability.                                                                 *
 *                                                                             *
 *  In this respect, the user's attention is drawn to the risks associated     *
 *  with loading,  using,  modifying and/or developing or reproducing the      *
 *  software by the user in light of its specific status of free software,     *
 *  that may mean  that it is complicated to manipulate,  and  that  also      *
 *  therefore means  that it is reserved for developers  and  experienced      *
 *  professionals having in-depth computer knowledge. Users are therefore      *
 *  encouraged to load and test the software's suitability as regards their    *
 *  requirements in conditions enabling the security of their systems and/or   *
 *  data to be ensured and,  more generally, to use and operate it in the      *
 *  same conditions as regards security.                                       *
 *                                                                             *
 *  The fact that you are presently reading this means that you have had       *
 *  knowledge of the CeCILL license and that you accept its terms.             *
 *                                                                             *
 ******************************************************************************/

#ifndef _BITOPS_C_

#define _BITOPS_C_

#if (defined(_MSC_VER) && (_MSC_VER >= 1400))
#include <intrin.h>
#endif

#define max32(a, b) \
((a) - (((a) - (b)) & -((int32_t)((uint32_t)((a) - (b)) >> 31))))

/* fast macros to test if at least one byte in a word is < n, or > n, or = 0 */
#define __zero(x)    (((x) - 0x01010101U) & ~(x) & 0x80808080U)
#define __less(x, n) (((x) - ~0U / 255 * (n)) & ~(x) & ~0U / 255 * 128)
#define __more(x, n) ((((x) + ~0U / 255 * (127 - (n))) | (x)) & ~0U / 255 * 128)
#define __between(x, m, n) \
(((~0U / 255 * (127 + (n)) - ((x) & ~0U / 255 * 127)) & ~(x) & \
 (((x) & ~0U / 255 * 127) + ~0U / 255 * (127 - (m)))) & ~0U / 255 * 128)

/* prefetch */
#if (defined(__GNUC__) && \
     ((__GNUC__ > 3) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)))
    #define L1_CACHE 3
    #define L2_CACHE 2
    #define L3_CACHE 1
    #define NTACCESS 0
    #define PREFETCH(addr, rw, locality) \
    __builtin_prefetch(addr, rw, locality)
#elif (defined(_MSC_VER) && (_MSC_VER >= 1600))
    #define L1_CACHE _MM_HINT_T0
    #define L2_CACHE _MM_HINT_T1
    #define L3_CACHE _MM_HINT_T2
    #define NTACCESS _MM_HINT_NTA
    #if defined(_M_IX86) || defined(_M_X64) || defined(_M_AMD64)
        #include <xmmintrin.h>
        #define PREFETCH(addr, rw, locality) \
        _mm_prefetch((const char *)(addr), locality)
    #elif (defined(_M_ARM64) && (_MSC_VER >= 1920))
        #define PREFETCH(addr, rw, locality) \
        __prefetch(addr)
    #else
        #define PREFETCH(addr, rw, locality)
    #endif
#else
    #define L1_CACHE
    #define L2_CACHE
    #define L3_CACHE
    #define NTACCESS
    #define PREFETCH(addr, rw, locality)
#endif

/* -------------------------------------------------------------------------- */
/* Bitwise operations */
/* -------------------------------------------------------------------------- */

static inline unsigned int __ctz(uint32_t i)
{
    unsigned long c;

    if (likely(i)) {
        /* hardware implementation */
        #if (defined(__GNUC__) && \
             ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))) && \
            (defined(__i386__) || defined(__x86_64__)) || \
            (defined(__arm__) || defined(__aarch64__))
        
        c = __builtin_ctz(i);

        #elif (defined(_MSC_VER) && (_MSC_VER >= 1400)) && \
              (defined(_M_IX86) || defined(_M_AMD64) || defined(_M_ARM))

        #pragma intrinsic(_BitScanForward)

        _BitScanForward(& c, (unsigned long) i);

        #else
        /* portable software implementation */
        i &= -i;
        c = 0;
        if (i & 0xaaaaaaaaU) c |= 1;
        if (i & 0xccccccccU) c |= 2;
        if (i & 0xf0f0f0f0U) c |= 4;
        if (i & 0xff00ff00U) c |= 8;
        if (i & 0xffff0000U) c |= 16;
        #endif
    } else c = 32;

    return c;
}

/* -------------------------------------------------------------------------- */

static inline unsigned int __ctzll(uint64_t i)
{
    if (likely(i)) {
        /* hardware implementation */
        #if (defined(__GNUC__) && \
             ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))) && \
            (defined(__i386__) || defined(__x86_64__)) || \
            (defined(__arm__) || defined(__aarch64__))

        return __builtin_ctzll(i);

        #elif (defined(_MSC_VER) && (_MSC_VER >= 1400)) && \
              (defined(_M_X64) || defined(_M_AMD64) || defined(_M_ARM))

        #pragma intrinsic(_BitScanForward64)

        unsigned long ret;
        _BitScanForward64(& ret, (unsigned __int64) i);
        return ret;

        #else
        /* portable software implementation (de Bruijn sequence) */
        static const uint8_t seq[64] = {
             0,  1,  2,  7,  3, 13,  8, 19,  4, 25, 14, 28,  9, 34, 20, 40,
             5, 17, 26, 38, 15, 46, 29, 48, 10, 31, 35, 54, 21, 50, 41, 57,
            63,  6, 12, 18, 24, 27, 33, 39, 16, 37, 45, 47, 30, 53, 49, 56,
            62, 11, 23, 32, 36, 44, 52, 55, 61, 22, 43, 51, 60, 42, 59, 58
        };

        return seq[((i & -i) * 0x0218a392cd3d5dbfULL) >> 58];

        #endif
    } else return 64;
}

/* -------------------------------------------------------------------------- */

static inline unsigned int __clz(uint32_t i)
{
    if (likely(i)) {
        /* hardware implementation */
        #if (defined(__GNUC__) && \
             ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))) && \
            (defined(__i386__) || defined(__x86_64__)) || \
            (defined(__arm__) || defined(__aarch64__))
        
        return __builtin_clz(i);
        
        #elif (defined(_MSC_VER) && (_MSC_VER >= 1400)) && \
              (defined(_M_IX86) || defined(_M_AMD64) || defined(_M_ARM))

        #pragma intrinsic(_BitScanReverse)

        unsigned long ret;

        _BitScanReverse(& ret, (unsigned long) i);

        return 31 - ret;

        #else
        /* portable software implementation (de Bruijn sequence) */
        static const char seq[32] = {
            0, 31, 9, 30, 3,  8, 13, 29,  2,  5,  7, 21, 12, 24, 28, 19,
            1, 10, 4, 14, 6, 22, 25, 20, 11, 15, 23, 26, 16, 27, 17, 18
        };
        
        i |= i >> 1;
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        i ++;

        return seq[i * 0x076be629 >> 27];

        #endif
    } else return 32;
}

/* -------------------------------------------------------------------------- */

static inline unsigned int __clzll(uint64_t i)
{
    if (likely(i)) {
        /* hardware implementation */
        #if (defined(__GNUC__) && \
             ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))) && \
            (defined(__i386__) || defined(__x86_64__)) || \
            (defined(__arm__) || defined(__aarch64__))
        
        return __builtin_clzll(i);
        
        #elif (defined(_MSC_VER) && (_MSC_VER >= 1400)) && \
              (defined(_M_X64) || defined(_M_AMD64) || defined(_M_ARM))

        #pragma intrinsic(_BitScanReverse64)

        unsigned long ret;

        _BitScanReverse64(& ret, (unsigned __int64) i);

        return 63 - ret;

        #else
        /* portable software implementation (de Bruijn sequence) */
        static const char seq[64] = {
             0, 47,  1, 56, 48, 27,  2, 60, 57, 49, 41, 37, 28, 16,  3, 61,
            54, 58, 35, 52, 50, 42, 21, 44, 38, 32, 29, 23, 17, 11,  4, 62,
            46, 55, 26, 59, 40, 36, 15, 53, 34, 51, 20, 43, 31, 22, 10, 45,
            25, 39, 14, 33, 19, 30,  9, 24, 13, 18,  8, 12,  7,  6,  5, 63
        };

        i |= i >> 1; 
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        i |= i >> 32;

        return 63 - seq[(i * 0x03f79d71b4cb0a89ULL) >> 58];

    #endif
    } else return 64;
}

/* -------------------------------------------------------------------------- */

static inline uint32_t __msb(uint32_t i)
{
    /* hardware implementation */
    #if (defined(__GNUC__) && \
         ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))) && \
        (defined(__i386__) || defined(__x86_64__)) || \
        (defined(__arm__) || defined(__aarch64__))

    return 1 << (__builtin_clz(i) ^ 31);

    #elif (defined(_MSC_VER) && (_MSC_VER >= 1400)) && \
          (defined(_M_IX86) || defined(_M_AMD64) || defined(_M_ARM))

    #pragma intrinsic(_BitScanReverse)

    unsigned long idx;

    _BitScanReverse(& idx, (unsigned long) i);

    return 1 << (idx ^ 31);

    #else
    /* portable software implementation (de Bruijn sequence) */
    static const uint8_t seq[] = { 0, 5, 1, 6, 4, 3, 2, 7 };

    i |= i >> 1; i |= i >> 2; i |= i >> 4;

    return 1 << seq[(uint8_t) (i * 0x1D) >> 5];

    #endif
}

/* -------------------------------------------------------------------------- */

static inline uint32_t __bswap32(uint32_t i)
{
    #if (defined(__GNUC__))

    return __builtin_bswap32(i);

    #elif (defined(_MSC_VER) && (_MSC_VER >= 1400))

    #pragma intrinsic(_byteswap_ulong)

    return _byteswap_ulong(i);

    #else

    /* portable software implementation */
    return (
        ((i >> 24) & 0x000000ff) |
        ((i >> 8)  & 0x0000ff00) |
        ((i << 8)  & 0x00ff0000) |
        ((i << 24) & 0xff000000)
    );

    #endif
}

/* -------------------------------------------------------------------------- */

static inline uint64_t __bswap64(uint64_t i)
{
    #if (defined(__GNUC__))

    return __builtin_bswap64(i);

    #elif (defined(_MSC_VER) && (_MSC_VER >= 1400))

    #pragma intrinsic(_byteswap_uint64)

    return _byteswap_uint64(i);

    #else

    /* portable software implementation */
    return (
        (i << 56) |
        ((i & 0xff00) << 40) |
        ((i & 0xff0000) << 24) |
        ((i & 0xff000000) << 8) |
        ((i & 0xff00000000) >> 8) |
        ((i & 0xff0000000000) >> 24) |
        ((i & 0xff000000000000) >> 40) |
        ((i & 0xff00000000000000) >> 56)
    );

    #endif
}

/* -------------------------------------------------------------------------- */

static inline unsigned int __zero_idx(uint32_t i)
{
    #if (defined(BIG_ENDIAN_HOST))
    i = __bswap32(i);
    #endif
    return __ctz(i) >> 3;
}

/* -------------------------------------------------------------------------- */

static inline unsigned int __zero_idx64(uint64_t i)
{
    #if (defined(BIG_ENDIAN_HOST))
    i = __bswap64(i);
    #endif
    return __ctzll(i) >> 3;
}

/* -------------------------------------------------------------------------- */

static inline unsigned int __msb_idx(uint32_t i)
{
    return 31 - __clz(i);
}

/* -------------------------------------------------------------------------- */

static inline unsigned int __msb_idx64(const uint64_t i)
{
    return 63 - __clzll(i);
}

/* -------------------------------------------------------------------------- */

static inline size_t __next_pow2(size_t size)
{
    size --;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    #if (SIZE_MAX > UINT32_MAX)
    size |= size >> 32;
    #endif
    size ++;
    size += (size == 0);

    return size;
}

/* -------------------------------------------------------------------------- */

static inline int __is_pow2_multiple(uint64_t value, uint32_t p)
{
    return (value & ((1ULL << p) - 1)) == 0;
}

/* -------------------------------------------------------------------------- */

static inline int __is_pow5_multiple(uint64_t value, const uint32_t p)
{
    /* returns true if value is divisible by 5^p */
    const uint64_t m_inv_5 = 14757395258967641293U;
    const uint64_t n_div_5 = 3689348814741910323U;
    uint32_t count = 0;

    while (1) {
        /* simulate a division by using the modular inverse of 5 */
        value *= m_inv_5;
        /* n_div_5 is the largest 64 bits multiple of 5 */
        if (value > n_div_5) break;
        count ++;
    }

    return (count >= p);
}

/* -------------------------------------------------------------------------- */
/* Other operations */
/* -------------------------------------------------------------------------- */

static inline uint64_t umul128(uint64_t a, uint64_t b, uint64_t *high)
{
    #if (defined(__SIZEOF_INT128__))
    __uint128_t result = (__uint128_t) a * (__uint128_t) b;
    *high = (uint64_t) (result >> 64);
    return (uint64_t) result;
    #elif (defined(_MSC_VER) && (_MSC_VER >= 1400)) && \
          (defined(_M_X64) || defined(_M_AMD64) || defined(_M_ARM))
    return __umul128(a, b, high);
    #else
    const uint32_t a_lo = (uint32_t) a;
    const uint32_t a_hi = (uint32_t) (a >> 32);
    const uint32_t b_lo = (uint32_t) b;
    const uint32_t b_hi = (uint32_t) (b >> 32);

    const uint64_t b00 = (uint64_t) a_lo * b_lo;
    const uint64_t b01 = (uint64_t) a_lo * b_hi;
    const uint64_t b10 = (uint64_t) a_hi * b_lo;
    const uint64_t b11 = (uint64_t) a_hi * b_hi;

    const uint32_t b00_lo = (uint32_t) b00;
    const uint32_t b00_hi = (uint32_t) (b00 >> 32);

    const uint64_t mid1 = b10 + b00_hi;
    const uint32_t mid1_lo = (uint32_t) (mid1);
    const uint32_t mid1_hi = (uint32_t) (mid1 >> 32);

    const uint64_t mid2 = b01 + mid1_lo;
    const uint32_t mid2_lo = (uint32_t) (mid2);
    const uint32_t mid2_hi = (uint32_t) (mid2 >> 32);

    const uint64_t r_hi = b11 + mid1_hi + mid2_hi;
    const uint64_t r_lo = ((uint64_t) mid2_lo << 32) | b00_lo;

    *high = r_hi;
    return r_lo;
    #endif
}

/* -------------------------------------------------------------------------- */

#define shr128(lo, hi, shift) (((hi) << (64 - (shift))) | ((lo) >> (shift)))

static inline uint64_t mul_shift64(uint64_t n, const uint64_t *mul, int32_t i)
{
    uint64_t high1;                                    // 128
    const uint64_t low1 = umul128(n, mul[1], & high1); // 64
    uint64_t high0;                                    // 64
    umul128(n, mul[0], & high0);                       // 0
    const uint64_t sum = high0 + low1;
    if (sum < high0) high1 ++; /* carry over into high1 */
    return shr128(sum, high1, i - 64);
}

/* -------------------------------------------------------------------------- */
/* CRC8 */
/* -------------------------------------------------------------------------- */

/* crc8 lookup table (Maxim/Dallas 1 wire) */
static const uint8_t _crc8_lut[256] = {
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
    0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
    0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
    0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
    0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
    0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
    0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
    0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
    0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f,
    0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
    0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
    0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1,
    0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
    0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
    0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a,
    0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
    0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};

/* -------------------------------------------------------------------------- */

static inline uint8_t _crc8(const char *string, size_t len)
{
    const uint8_t *p = (const uint8_t *) string;
    uint8_t crc = 0xff;
    while (len --) crc = _crc8_lut[crc ^ *p ++];
    return crc;
}

/* -------------------------------------------------------------------------- */
/* CRC7 */
/* -------------------------------------------------------------------------- */

/* crc7 lookup table (polynomial x^7 + x^3 + 1) */
static const uint8_t _crc7_lut[256] = {
    0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
    0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
    0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
    0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
    0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
    0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
    0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
    0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
    0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
    0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
    0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
    0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
    0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
    0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
    0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
    0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
    0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
    0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
    0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
    0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
    0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
    0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
    0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
    0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
    0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
    0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
    0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
    0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
    0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
    0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
    0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
    0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79
};

/* -------------------------------------------------------------------------- */

static inline uint8_t _crc7(const char *string, size_t len)
{
    const uint8_t *p = (const uint8_t *) string;
    uint8_t crc = 0;
    while (len --) crc = _crc7_lut[(crc << 1) ^ *p ++];
    return crc & 0x7f;
}

/* -------------------------------------------------------------------------- */
/* Atomics */
/* -------------------------------------------------------------------------- */

#if ((defined(_MSC_VER)) && ((_MSC_VER >= 1300) || (defined(_M_IX86)))) || \
    ((defined(__GNUC__)) && (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)) || \
     (__GNUC__ > 4) || defined(__i386__) || defined(__x86_64__))) || \
    ((defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)))
#define HAS_ATOMICS
#endif

#ifdef HAS_ATOMICS

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L))
#include <stdatomic.h>
#define _ATOMIC _Atomic
#else
#define _ATOMIC volatile
#endif

#if ((defined(_MSC_VER)) && (_MSC_VER == 1300))
    extern long __cdecl _InterlockedCompareExchange(
        long volatile *Destination, long Exchange, long Comparand);
    extern long __cdecl _InterlockedExchangeAdd(
        long volatile *Addend, long Value);
    void _ReadWriteBarrier(void);
    #pragma intrinsic(_InterlockedCompareExchange)
    #pragma intrinsic(_InterlockedExchangeAdd)
    #pragma intrinsic(_ReadWriteBarrier)
#endif

static inline int _atomic_cas(_ATOMIC int *ptr, int expected, int desired)
{
    #if (defined(_MSC_VER))
        #if (_MSC_VER >= 1300)
        return _InterlockedCompareExchange(
            (volatile long *) ptr,
            (long) desired,
            (long) expected
        ) == expected;
        #elif (defined(_M_IX86))
        {
            long result;
            __asm {
                mov eax, expected
                mov ecx, ptr
                mov edx, desired
                lock cmpxchg [ecx], edx
                mov result, eax
            }
            return result == expected;
        }
        #endif
    #elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L))
        return atomic_compare_exchange_weak_explicit(
            ptr,
            & expected,
            desired,
            memory_order_acq_rel,
            memory_order_relaxed
        );
    #elif (defined(__GNUC__))
        #if ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)))
        return __atomic_compare_exchange_n(
            ptr,
            & expected,
            desired,
            1,
            __ATOMIC_ACQ_REL,
            __ATOMIC_RELAXED
        );
        #elif ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))
        return __sync_bool_compare_and_swap(ptr, expected, desired);
        #elif defined(__i386__) || defined(__x86_64__)
        int prev;
        __asm__ __volatile__(
            "lock; cmpxchgl %2, %1"
            : "=a"(prev), "+m"(*ptr)
            : "r"(desired), "0"(expected)
            : "memory", "cc"
        );
        return prev == expected;
        #endif
    #endif
}

/* -------------------------------------------------------------------------- */

static inline int _atomic_ldr(_ATOMIC int *ptr)
{
    #if (defined(_MSC_VER))
        int ret = *ptr;
        #if _MSC_VER >= 1300
        _ReadWriteBarrier();
        #elif (defined(_M_IX86))
        __asm { /* empty, acts as barrier */ }
        #endif
        return ret;
    #elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L))
        return atomic_load_explicit(ptr, memory_order_relaxed);
    #elif (defined(__GNUC__))
        #if ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)))
        return __atomic_load_n(ptr, __ATOMIC_RELAXED);
        #elif ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))
        return __sync_fetch_and_add(ptr, 0);
        #elif defined(__i386__) || defined(__x86_64__)
        int value;
        __asm__ __volatile__(
            "movl %1, %0"
            : "=r"(value)
            : "m"(*ptr)
            : "memory"
        );
        return value;
        #endif
    #endif
}

/* -------------------------------------------------------------------------- */

static inline void _atomic_str(_ATOMIC int *ptr, int value)
{
    #if (defined(_MSC_VER))
        #if _MSC_VER >= 1300
        _ReadWriteBarrier();
        #endif
        *ptr = value;
    #elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L))
        atomic_store_explicit(ptr, value, memory_order_release);
    #elif (defined(__GNUC__))
        #if ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)))
        __atomic_store_n(ptr, value, __ATOMIC_RELEASE);
        #elif ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))
        __sync_lock_test_and_set(ptr, value);
        #elif defined(__i386__) || defined(__x86_64__)
        __asm__ __volatile__(
            "movl %1, %0"
            : "=m"(*ptr)
            : "r"(value)
            : "memory"
        );
        #endif
    #endif
}

/* -------------------------------------------------------------------------- */

static inline int _atomic_add(_ATOMIC int *ptr, int val)
{
    #if (defined(_MSC_VER))
        #if _MSC_VER >= 1300
        return _InterlockedExchangeAdd((long volatile *) ptr, (long) val);
        #elif (defined(_M_IX86))
        long result;
        __asm {
            mov ecx, ptr
            mov eax, val
            lock xadd [ecx], eax
            mov result, eax
        }
        return result;
        #endif
    #elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L))
        return atomic_fetch_add_explicit(ptr, val, memory_order_acq_rel);
    #elif (defined(__GNUC__))
        #if ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)))
        return __atomic_fetch_add(ptr, val, __ATOMIC_ACQ_REL);
        #elif ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))
        return __sync_fetch_and_add(ptr, val);
        #elif defined(__i386__) || defined(__x86_64__)
        int result;
        __asm__ __volatile__(
            "lock; xaddl %0, %1"
            : "=r"(result), "+m"(*ptr)
            : "0"(val)
            : "memory"
        );
        return result;
        #endif
    #endif
}

/* -------------------------------------------------------------------------- */

static inline int _atomic_sub(_ATOMIC int *ptr, int val)
{
    #if (defined(_MSC_VER))
        #if _MSC_VER >= 1300
        return _InterlockedExchangeAdd((long volatile *) ptr, - (long) val);
        #elif (defined(_M_IX86))
        long result;
        long neg_val = -val;
        __asm {
            mov ecx, ptr
            mov eax, neg_val
            lock xadd [ecx], eax
            mov result, eax
        }
        return result;
        #endif
    #elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L))
        return atomic_fetch_sub_explicit(ptr, val, memory_order_release);
    #elif (defined(__GNUC__))
        #if ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)))
        return __atomic_fetch_sub(ptr, val, __ATOMIC_RELEASE);
        #elif ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))
        return __sync_fetch_and_sub(ptr, val);
        #elif defined(__i386__) || defined(__x86_64__)
        int result;
        int neg_val = -val;
        __asm__ __volatile__(
            "lock; xaddl %0, %1"
            : "=r"(result), "+m"(*ptr)
            : "0"(neg_val)
            : "memory"
        );
        return result;
        #endif
    #endif
}

/* -------------------------------------------------------------------------- */

#endif

#endif
