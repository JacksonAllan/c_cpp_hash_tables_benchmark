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

#ifndef ASKL_STRING_H

#define ASKL_STRING_H

#include "askl.h"

#ifdef HAS_PCRE
#include <pcre.h>
#endif

#ifdef HAS_ICONV
#include <iconv.h>
#endif

/** @defgroup string ASKL::string */
typedef struct String {
    char *data;
    struct String *tokens;
    struct String *parent;
    uint32_t len;
    uint32_t count;
    struct {
        uint32_t capacity;
        uint16_t tokens_capacity;
        uint16_t flags;
    } internal;
} String;

typedef struct _String_Pattern String_Pattern;

#define _STRING_FIXED_LENGTH 0x0001 /* disable string resizing */
#define _STRING_READ_ONLY    0x0002 /* disable string writing */
#define _STRING_IMMUTABLE    0x0003 /* disable writing and resizing */
#define _STRING_DISABLE_FREE 0x0004 /* disable free() on string content */
#define _STRING_ENCAPSULATED 0x0005 /* disable all dynamic allocation */
#define _STRING_STATIC_ALLOC 0x0008 /* static, stack allocated string */
#define _STRING_EXTENSION    0x000f /* mask extension flags */
#define _STRING_VALIDATED    0x0010 /* this string has been validated */
#define _STRING_HAS_ERROR    0x0020 /* this string contains errors */
#define HAS_ERROR(x) ((x)->internal.flags & _STRING_HAS_ERROR)
#define _STRING_BUFFERING    0x0040 /* this string is used for buffering */
#define IS_BUFFER(x) ((x)->internal.flags & _STRING_BUFFERING)
/*                           0x0080    reserved */
#ifdef _ENABLE_HTTP
#define _STRING_HTTP_REQUEST 0x0100 /* HTTP request */
#define IS_HTTP(x) ((x)->_flags & _STRING_HTTP_REQUEST)
#endif
#define _STRING_PARTIAL_DATA 0x0200 /* used by the server for streaming */
#define IS_FRAGMENT(x) ((x)->internal.flags & _STRING_PARTIAL_DATA)
#define _STRING_LARGE_BUFFER 0x0400 /* large request */
#define IS_LARGE(x) ((x)->_flags & _STRING_LARGE_BUFFER)
#ifdef _ENABLE_HTTP
#define _STRING_HTTP_CHUNKED 0x0800 /* HTTP 1.1 Chunked encoding */
#define IS_CHUNK(x) ((x)->_flags & _STRING_HTTP_CHUNKED)
#endif

#define STRING_STATIC_INITIALIZER(s, l) { \
    (char *)(s), NULL, NULL, (l), 0, \
    { 0, 0, _STRING_IMMUTABLE | _STRING_ENCAPSULATED | _STRING_STATIC_ALLOC } \
}

/**
 * @ingroup string
 * @struct String
 *
 * This structure is designed to hold arbitrary sequences of bytes.
 *
 * @ref data is a pointer to the content of the String.
 *
 * @ref len is the size, in bytes, of that content.
 *
 * @ref count holds the number of tokens in which the string has been sliced,
 * with @ref string_split or any other function.
 *
 * @ref tokens is an array of String structures, which are set to point
 * to particular subsections of the current string. Tokens can be resized or
 * altered with the various functions defined in this API, without restrictions.
 * However, some functions performing destructive transformations on their
 * input may destroy all tokens associated to a string; in this case, this
 * behaviour will be specified in the function documentation.
 *
 * @ref parent is a pointer to the parent string of a token (which may be
 * a token itself). You can test if a string is a token or not by checking
 * the parent field value; if it is NULL, then the string is not a token.
 *
 * You can clean up the tokens of a string at any time by calling
 * @ref string_free_token().
 *
 * All other fields are private, using or altering their values is likely
 * to result in unexpected behaviours.
 *
 * @b private @ref flags is a bitfield holding several internal flags, like
 * write protection, or resizing protection.
 *
 * @b private @ref capacity is the actual size of the inner data buffer of the
 * String structure. It should only be used in the resizing functions.
 *
 * @b private @ref tokens_capacity is the number of available slots for new
 * tokens. This structure cannot hold more than 65535 tokens.
 *
 */

#define EMPTY(s)    (! (s)->len || ((s)->len == 1 && ! *(s)->data))

#define tokens(...) EXPAND_TOKENS(__VA_ARGS__)

#define EXPAND_TOKENS_1(a)         tokens[(a)]
#define EXPAND_TOKENS_2(a,b)       tokens[(a)].tokens[(b)]
#define EXPAND_TOKENS_3(a,b,c)     tokens[(a)].tokens[(b)].tokens[(c)]

#define COUNT_ARGS_IMPL(_1,_2,_3,N,...) N
#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 3, 2, 1)

#define SELECT_EXPAND(N) EXPAND_TOKENS_##N
#define EXPAND_TOKENS_EVAL(N) SELECT_EXPAND(N)
#define EXPAND_TOKENS(...) \
EXPAND_TOKENS_EVAL(COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

/**
 * @name Token navigation helpers
 * @ingroup string
 * @def tokens(...)
 *
 * Convenience macro for navigating nested tokens from a parent String.
 *
 *   Example:
 *   @code
 *   String *s = ...;
 *   // First-level token
 *   String *t  = & s->tokens(x);
 *   // Second-level token
 *   String *u  = & s->tokens(x, y);
 *   // Third-level token
 *   String *v  = & s->tokens(x, y, z);
 *   @endcode
 *
 * This expands to:
 *
 * - s->tokens[x]
 * - s->tokens[x].tokens[y]
 * - s->tokens[x].tokens[y].tokens[z]
 *
 * The macro does not perform bounds checking.
 */


/* -------------------------------------------------------------------------- */

ASKL_API int string_api_setup(void);

/* -------------------------------------------------------------------------- */

ASKL_API String *string_reserve(const char *string, size_t len, size_t extra);

/**
 * @ingroup string
 * @fn String *string_reserve(const char *string, size_t len, size_t extra)
 * @param string Optional pointer to initial data to copy into the new string.
 * @param len    Number of bytes to copy from @p string.
 * @param extra  Additional bytes to reserve in the internal buffer.
 * @return A pointer to a new String, or NULL on error.
 *
 * Allocates a new String and its internal buffer, with enough space for
 * len + extra bytes.
 *
 * - If @p string is non-NULL, up to @p len bytes are copied into the new
 *   buffer; the buffer is then padded with a wchar_t-sized NUL terminator.
 * - If @p string is NULL and @p len is zero, the String is created with
 *   no buffer and len == 0.
 *
 * This is useful when you know a string will grow and want to avoid multiple
 * reallocations.
 *
 * The returned String must be destroyed with @ref string_free().
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_alloc(const char *string, size_t len);

/**
 * @ingroup string
 * @fn String *string_alloc(const char *string, size_t len)
 * @param string Optional pointer to initial data to copy.
 * @param len    Number of bytes to allocate and (optionally) copy.
 * @return A pointer to a new String, or NULL on error.
 *
 * string_alloc() is a convenience wrapper over @ref string_reserve() with
 * extra == 0.
 *
 * Typical patterns:
 *
 * @code
 * // Allocate an empty string with no buffer.
 * String *s = string_alloc(NULL, 0);
 *
 * // Allocate a 256-byte buffer, uninitialized.
 * String *buf = string_alloc(NULL, 256);
 *
 * // Copy an existing C string (NUL byte is treated as data like any other).
 * String *copy = string_alloc(src, len);
 * @endcode
 *
 * The returned String must be freed with @ref string_free().
 */


/* -------------------------------------------------------------------------- */

static inline const char *string_end(const String *s)
{
    assert(s && s->data);
    return s->data + s->len;
}

/* -------------------------------------------------------------------------- */

static inline String *last_token(const String *s) {
    assert(s && s->count);
    return & s->tokens[s->count - 1];
}

/* -------------------------------------------------------------------------- */

ASKL_API String *string_encaps(const char *string, size_t len);

/**
 * @ingroup string
 * @fn String *string_encaps(const char *string, size_t len)
 * @param string the buffer to encapsulate in a String structure
 * @param len the length of the buffer
 * @return NULL if an error occurred, a pointer to a new String otherwise
 *
 * This functions wraps an existing static or dynamically allocated buffer
 * in a new read-only, fixed length String structure.
 *
 * This "static" String structure can be used with all string functions
 * accepting read-only Strings, and should be deleted after use with
 * the @ref string_free() function. This will not free the initial buffer,
 * though. Handling of the initial buffer is left to the programmer.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API uint8_t string_fetch_uint8(String *string);

/* -------------------------------------------------------------------------- */

ASKL_API uint16_t string_fetch_uint16(String *string);

/* -------------------------------------------------------------------------- */

ASKL_API uint32_t string_fetch_uint32(String *string);

/* -------------------------------------------------------------------------- */

ASKL_API uint64_t string_fetch_uint64(String *string);

/* -------------------------------------------------------------------------- */

ASKL_API int string_fetch_buffer(String *string, char *out, size_t len);

/* -------------------------------------------------------------------------- */

ASKL_API void string_flush(String *string);

/* -------------------------------------------------------------------------- */

ASKL_API int string_wchar(String *string);

/**
 * @ingroup string
 * @fn int string_wchar(String *string)
 * @param string the string to be converted.
 * @return -1 if an error occurred, 0 otherwise.
 *
 * This function converts the data stored in the internal buffer of the given
 * @b string to wide characters from multibyte characters.
 *
 * If the conversion is successful, the inner buffer is replaced by its
 * multibyte equivalent.
 *
 * If the function fails to convert the data, it will returns -1 and leave the
 * buffer unchanged.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_mbyte(String *string);

/**
 * @ingroup string
 * @fn int string_mbyte(String *string)
 * @param string the string to be converted.
 * @return -1 if an error occurred, 0 otherwise.
 *
 * This function converts the data stored in the internal buffer of the given
 * @b string to multibyte characters from wide characters.
 *
 * If the conversion is successful, the inner buffer is replaced by its
 * wide character equivalent.
 *
 * If the function fails to convert the data, it will returns -1 and leave the
 * buffer unchanged.
 *
 */

/* -------------------------------------------------------------------------- */
#ifdef HAS_ICONV
/* -------------------------------------------------------------------------- */

ASKL_API size_t string_convs(const char *src, size_t srclen, const char *src_enc,
                           char *dst, size_t dstlen, const char *dst_enc);

/**
 * @ingroup string
 * @fn size_t string_convs(const char *src, size_t srclen, const char *src_enc,
 *                         char *dst, size_t dstlen, const char *dst_enc)
 * @param src the string to be converted.
 * @param srclen the length of the string to be converted.
 * @param src_enc the encoding of the string to be converted.
 * @param dst the output buffer.
 * @param dstlen length of the output buffer.
 * @param dst_enc encoding to use for the conversion.
 * @return -1 if an error occurred, 0 otherwise.
 *
 * This function uses the Iconv library to convert the encoding of the given
 * string. If the output is NULL, the function will return the length the
 * output buffer should have to fit the converted string.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_conv(String *s, const char *src_enc, const char *dst_enc);

/**
 * @ingroup string
 * @fn int string_conv(String *s, const char *src_enc, const char *dst_enc)
 * @param s the string to be converted.
 * @param src_enc the encoding of the original string
 * @param dst_enc the encoding to use for the conversion
 * @return -1 if an error occurred, 0 otherwise.
 *
 * This function uses the Iconv library to convert the encoding of the given
 * string.
 *
 */

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

ASKL_API String *string_clone(const String *string);

/**
 * @ingroup string
 * @fn String *string_clone(const String *string)
 * @param string the string to be duplicated.
 * @return a pointer to a new string, or NULL.
 *
 * This function simply makes an exact copy of the String given in parameter,
 * and returns it.
 *
 * If the source string has subtokens, they are copied along with the data.
 *
 * If for some reason copying the string was not possible, the function
 * will return NULL.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_clone_reserve(const String *string, size_t extra);

/**
 * @ingroup string
 * @fn String *string_clone_reserve(const String *string)
 * @param string the string to be duplicated.
 * @param extra additional space to allocate.
 * @return a pointer to a new string, or NULL.
 *
 * This function simply makes an exact copy of the String given in parameter,
 * and returns it. The new string will be extended to be @b extra bytes longer
 * than the original.
 *
 * If the source string has subtokens, they are copied along with the data.
 *
 * If for some reason copying the string was not possible, the function
 * will return NULL.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_free(String *string);

/**
 * @ingroup string
 * @fn String *string_free(String *string)
 * @param string the string to be destroyed.
 * @return always NULL.
 *
 * This function will properly clean up and destroy a String structure,
 * including its tokens if there are any.
 *
 * This function always returns NULL, so it can be used to clean a pointer:
 * str = string_free(str);
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API size_t string_len(const String *string);

/**
 * @ingroup string
 * @fn size_t string_len(const String *string)
 * @param string the string the size has to be read.
 * @return the size of the string, or (size_t) -1.
 *
 * This function simply returns the size of the given string. It may be
 * preferable to the SIZE, CLEN or WLEN macros since it performs a NULL check.
 *
 * If the size can not be read, the function returns (size_t) -1.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API size_t string_capacity(const String *string);

/**
 * @ingroup string
 * @fn size_t string_capacity(const String *string)
 * @param string the string the buffer space has to be read.
 * @return the allocation size of the string, or (size_t) -1.
 *
 * This function simply returns the allocation space consumed by the buffer
 * of the given string.
 *
 * If the buffer space can not be read, the function returns (size_t) -1.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API size_t string_available(const String *string);

/* -------------------------------------------------------------------------- */

ASKL_API int string_resize(String *string, size_t size);

/**
 * @ingroup string
 * @fn int string_resize(String *string, size_t size)
 * @param string the string to resize.
 * @param size the new size to give to the string.
 * @return -1 if the string could not be resized, 0 otherwise.
 *
 * This function forces the resizing of a string. The buffer will be truncated
 * or extended to the given @b size, without care for the inner data.
 *
 * If the buffer can not be resized (wrong size, not enough memory or if the
 * string is marked as not resizable), the string is left untouched and the
 * function returns -1.
 *
 * If the buffer size matches the given @b size, no changes are done to the
 * string and the function returns 0.
 *
 * If this function is called on a token, it will extend or shrink the
 * main string and alter the token size accordingly.
 *
 * Subtokens are kept and updated after resizing.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_extend(String *string, size_t size);

/**
 * @ingroup string
 * @fn int string_extend(String *string, size_t size)
 * @param string the string to be extended.
 * @param size the new size of the string.
 * @return -1 if an error occurs (see @ref string_resize), 0 otherwise.
 *
 * This function is simply a wrapper around @ref string_resize(), which ensures
 * the buffer will only be resized if @b size is greater than its current size.
 *
 */
 

/* -------------------------------------------------------------------------- */

ASKL_API int string_shrink(String *string, size_t size);

/**
 * @ingroup string
 * @fn int string_shrink(String *string, size_t size)
 * @param string the string to be shrunk.
 * @param size the new size of the string.
 * @return -1 if an error occurs (see @ref string_resize), 0 otherwise.
 *
 * This function is simply a wrapper around @ref string_resize(), which ensures
 * the buffer will only be resized if @b size is less than its current size.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_splice(String *to, off_t o, const char *from, size_t l);

/**
 * @ingroup string
 * @fn String *string_splice(String *to, off_t o, const char *from, size_t l)
 * @param to   Destination string (may be NULL).
 * @param o    Offset in @p to at which to write, within current bounds.
 * @param from Source buffer to copy from.
 * @param l    Number of bytes to copy.
 * @return NULL on error, or a pointer to the destination string.
 *
 * Low-level primitive for moving bytes into a String.
 *
 * - If @p to is NULL, a new String is allocated large enough to hold
 *   @p l bytes.
 * - The destination is resized as needed to make room for o + l bytes.
 * - Overlapping source/destination is handled correctly.
 *
 * On success, @p to->len is updated as needed and the internal buffer is
 * NUL-terminated if @p to is a top-level string.
 *
 * This function may drop or partially adjust existing tokens when they would
 * become invalid.
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_cut(String *string, off_t o, size_t l, char *out);

/**
 * @ingroup string
 * @fn int string_cut(String *string, off_t o, size_t l, char *out)
 * @param string Target string.
 * @param o      Offset of the substring to cut (relative to @p string).
 * @param l      Length of the substring to cut.
 * @param out    Optional destination buffer; if non-NULL, the removed bytes
 *               are copied there.
 * @return 0 on success, -1 on error.
 *
 * Removes a substring [o, o + l) from @p string and closes the gap by
 * shifting subsequent data to the left.
 *
 * - If @p out is non-NULL, the removed bytes are copied into @p out.
 * - The underlying buffer is reused and the string is resized in place.
 * - Token metadata are updated to keep existing tokens valid:
 *   tokens fully inside the removed region are dropped; tokens after the
 *   removed region are shifted; straddling tokens are truncated.
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_append_buffer(String *to, const char *from, size_t len);

/**
 * @ingroup string
 * @fn String *string_append_buffer(String *to, const char *from, size_t len)
 * @param to   Destination string (may be NULL).
 * @param from Source buffer.
 * @param len  Length of @p from in bytes.
 * @return NULL on error, otherwise a pointer to the resulting string.
 *
 * Appends @p from to the end of @p to, resizing it as needed. If @p to is
 * NULL, a new String is allocated and initialized with @p from.
 *
 * If appending to a token whose end matches the current end of the string,
 * the append is performed relative to that token to preserve logical token
 * structure. Existing tokens are preserved where possible.
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_prepend_buffer(String *to, const char *from, size_t len);

/**
 * @ingroup string
 * @fn String *string_prepend_buffer(String *to, const char *from, size_t len)
 * @param to the destination string.
 * @param from the source C string.
 * @param len the source C string length.
 * @return NULL if an error occurred, 0 otherwise.
 *
 * This function prepends the source string to the destination string,
 * using @ref string_movs() as backend.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_compare_buffer(const String *a, const char *b, size_t len);

/**
 * @ingroup string
 * @fn int string_compare_buffer(const String *a, const char *b, size_t len)
 * @param a   String to compare.
 * @param b   Raw buffer to compare to.
 * @param len Number of bytes in @p b.
 * @return memcmp()-style result, or INT_MAX on parameter error.
 *
 * Compares up to min(a->len, len) bytes of @p a->data against @p b using
 * memcmp(). Returns:
 *
 * - < 0     if @p a is lexicographically less than @p b,
 * - > 0     if @p a is greater than @p b,
 * - 0       if the prefixes compared are equal,
 * - INT_MAX if parameters are invalid.
 */


/* -------------------------------------------------------------------------- */

ASKL_API String *string_append(String *to, const String *from);

/**
 * @ingroup string
 * @fn String *string_append(String *to, const String *from)
 * @param to the destination string.
 * @param from the source string.
 * @return NULL if an error occurred, 0 otherwise.
 *
 * This function is simply a wrapper around @ref string_append_buffer().
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_prepend(String *to, const String *from);

/**
 * @ingroup string
 * @fn String *string_prepend(String *to, const String *from)
 * @param to the destination string.
 * @param from the source string.
 * @return NULL if an error occurred, 0 otherwise.
 *
 * This function is simply a wrapper around @ref string_prepend_buffer().
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_compare(const String *a, const String *b);

/**
 * @ingroup string
 * @fn string_compare(const String *a, const String *b)
 * @param a the destination string.
 * @param b the source string.
 * @return NULL if an error occurred, 0 otherwise.
 *
 * This function is simply a wrapper around @ref string_cmps(), please see
 * the documentation of @ref string_cmps().
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_upper(String *string);

/**
 * @ingroup string
 * @fn int string_upper(String *string)
 * @param string
 * @return -1 if an error occurred, 0 otherwise
 *
 * This function simply converts the internal buffer of the given string to
 * upper case.
 *
 * Since the conversion is done in place, no resizing is done and thus
 * existing tokens are preserved.
 *
 * If an error occurs, the function returns -1.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_lower(String *string);

/**
 * @ingroup string
 * @fn int string_lower(String *string)
 * @param string
 * @return -1 if an error occurred, 0 otherwise
 *
 * This function simply converts the internal buffer of the given string to
 * lower case.
 *
 * Since the conversion is done in place, no resizing is done and thus
 * existing tokens are preserved.
 *
 * If an error occurs, the function returns -1.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_pattern_compile(String_Pattern *p, const char *s, size_t len);

/**
 * @ingroup string
 * @fn int string_pattern_compile(String_Pattern *p, const char *s, size_t len)
 * @param p   Pattern object to initialize.
 * @param s   Pointer to the pattern bytes ("needle").
 * @param len Length of the pattern in bytes.
 * @return 0 on success, -1 on error.
 *
 * Pre-computes lookup data for a search pattern to be used with
 * @ref string_find_pattern(). The pattern data itself is not copied; only
 * metadata (length and Boyer–Moore style skip table) are stored in @p p.
 *
 * For patterns longer than 4 bytes, a tuned Boyer–Moore algorithm is used.
 * For patterns of length 1–4, @ref string_find_pattern() falls back to a
 * simple, faster naive search and only the effective length is stored.
 *
 * @warning The pattern length must be in the range [1, 255]. Longer patterns
 *          are rejected and cause the function to return -1.
 *
 * @note The caller is responsible for ensuring that the pointer @p s remains
 *       valid for as long as it is used with @ref string_find_pattern(), or
 *       that an equivalent pattern buffer is passed as the @p sub parameter
 *       there.
 */

/* -------------------------------------------------------------------------- */

ASKL_API off_t string_find_pattern(
    const String *s,
    size_t o,
    const char *sub,
    String_Pattern *p
);

/**
 * @ingroup string
 * @fn off_t string_find_pattern(const String *s, size_t o, const char *sub,
 *                               String_Pattern *p)
 * @param s   The "haystack" string to search in.
 * @param o   Starting offset within @p s (in bytes).
 * @param sub Pointer to the pattern bytes ("needle").
 * @param p   A pattern object previously initialized with
 *            @ref string_pattern_compile() for the same pattern.
 * @return -1 on error or if the pattern is not found, otherwise the byte
 *         offset of the first match within @p s.
 *
 * Searches for a pre-compiled pattern inside @p s, starting at offset @p o.
 * The effective pattern length and lookup table are taken from @p p; the
 * @p sub pointer is only used to compare candidate matches.
 *
 * For patterns of length 1–4 (as encoded into @p p by
 * @ref string_pattern_compile()), a small naive search is used. For longer
 * patterns, an optimized Boyer–Moore style algorithm with a skip table
 * is used.
 *
 * @warning @p p must have been initialized by calling
 *          @ref string_pattern_compile() with the same pattern bytes and
 *          length that are referenced by @p sub here. Passing a different
 *          pattern buffer or length is undefined behaviour.
 *
 * @note If @p o plus the pattern length stored in @p p exceeds @p s->len,
 *       the function fails and returns -1.
 */

/* -------------------------------------------------------------------------- */

ASKL_API off_t string_find(
    const String *str,
    size_t o,
    const char *sub,
    size_t len
);

/**
 * @ingroup string
 * @fn off_t string_find(const String *str, off_t o, const char *sub,
                         size_t len                                  )
 * @param str The "haystack" string to search in.
 * @param o   Starting offset within @p str (in bytes).
 * @param sub The "needle" bytes to search for.
 * @param len Length of the needle in bytes.
 * @return -1 on error or if the substring is not found, otherwise the byte
 *         offset of the first match within @p str.
 *
 * Searches for the substring @p sub in @p str, starting at offset @p o.
 * Internally this function compiles a temporary search pattern using
 * @ref string_pattern_compile() and then calls @ref string_find_pattern().
 *
 * @warning The needle length @p len must not exceed 255 bytes. Longer
 *          needles are rejected and cause the function to return -1.
 *
 * @note This is a convenience function for one-off searches. If you need to
 *       search for the same needle multiple times (possibly in different
 *       strings), it is more efficient to call @ref string_pattern_compile()
 *       once and then use @ref string_find_pattern() repeatedly.
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_split(String *string, const char *pattern, size_t len);

/**
 * @ingroup string
 * @fn string_split(String *string, const char *pattern, size_t len)
 * @param string the string to be split.
 * @param pattern the token delimiter.
 * @param len the size of the delimiter.
 * @return -1 if an error occurred, 0 otherwise.
 *
 * This function use @ref string_finds() to split the given @b string, using
 * a delimiter passed in parameter.
 *
 * The parts of a split string are stored in the @ref tokens field of the
 * String structure, and their number in the @ref count field.
 *
 * @note Tokens are views; they do not copy data. However, they can be
 * transparently used as valid, resizeable and writeable String with the
 * great majority of the functions operating on String structures.
 *
 * @warning several string manipulation functions destroy tokens to avoid
 * corruption, mainly when the underlying data are removed.
 *
 * You can clean up tokens at any time by calling @ref string_free_token().
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_merge(String *string, const char *pattern, size_t len);

/**
 * @ingroup string
 * @fn int string_merge(String *string, const char *pattern, size_t len)
 * @param string the string to be merged
 * @param pattern the new token delimiter
 * @param len the size of the delimiter
 * @return -1 if an error occurred, 0 otherwise.
 *
 * This function replaces all the delimiters between tokens of the
 * target string with the new delimiter given in parameter.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_replace_all(
    String *string,
    const char *search,
    size_t slen,
    const char *rep,
    size_t rlen
);

/**
 * @ingroup string
 * @fn String *string_replace_all(String *string, const char *search,
 *                                size_t slen, const char *rep, size_t rlen)
 * @param string the string where an expression should be replaced.
 * @param search the expression to be replaced.
 * @param slen the length of this expression.
 * @param rep the replacement string.
 * @param rlen the length of the replacement string.
 * @return NULL if an error occurred, a pointer to the main string otherwise.
 *
 * This function searches for the given @b search string inside the main
 * @b string, and replaces each occurrence by the provided @b rep string.
 *
 * If a NULL @b rep parameter is given, the target substring is deleted
 * instead and the string resized accordingly.
 *
 * The original string is automatically resized to fit the modifications, but
 * since it is a destructive transformation, all its token will be deleted
 * in order to avoid corruption.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_remove_all(String *str, const char *rem, size_t len);

/**
 * @ingroup string
 * @fn String *string_remove_all(String *str, const char *rem, size_t len)
 * @param str the string to be processed
 * @param rem the substring which must be removed
 * @param len the length of the substring
 * @return NULL if an error occurred, a pointer to the main string otherwise.
 *
 * This function is simply a wrapper around @ref string_replace_all() with
 * a NULL replacement string. This way, all occurrences of the target substring
 * are dropped.
 *
 * @note Since it calls @ref string_replace_all(), this function cause the
 * destruction of all the tokens of the main string.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_select(String *string, unsigned int off, size_t len);

/**
 * @ingroup string
 * @fn String *string_select(String *string, unsigned int off, size_t len)
 * @param string Parent string.
 * @param off    Offset from which to create the token.
 * @param len    Length of the selected region.
 * @return Pointer to the new token, or NULL on error.
 *
 * Convenience function that clears any existing tokens on @p string and then
 * creates a single token covering [off, off + len).
 *
 * Equivalent to:
 * @code
 * string_free_token(string);
 * return string_add_token(string, off, off + len);
 * @endcode
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_add_token(String *s, off_t start, off_t end);

/**
 * @ingroup string
 * @fn String *string_add_token(String *s, off_t start, off_t end)
 * @param s     Parent string.
 * @param start Start offset (inclusive) relative to @p s->data.
 * @param end   End offset (exclusive) relative to @p s->data.
 * @return Pointer to the newly created token, or NULL on error.
 *
 * Creates a new token that views the range [start, end) of @p s.
 */

/* -------------------------------------------------------------------------- */

ASKL_API int string_push_token(String *string, const char *token, size_t len);

/**
 * @ingroup string
 * @fn int string_push_tokens(String *string, const char *token, size_t len)
 * @param string the string to which the token will be appended
 * @param token the data to append
 * @param len the length of the data
 * @return -1 if an error happens, 0 otherwise
 *
 * Appends @p token to the end of @p string and creates a token that points to
 * the newly appended region.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *string_suppr_token(String *s, unsigned int index);

/* -------------------------------------------------------------------------- */

ASKL_API String *string_pop_token(String *string);

/* -------------------------------------------------------------------------- */
#ifdef HAS_PCRE
/* -------------------------------------------------------------------------- */

ASKL_API int string_parse(String *string, const char *pattern, size_t len);

/**
 * @ingroup string
 * @fn int string_parse(String *string, const char *pattern, size_t len)
 * @param string the string to be processed
 * @param pattern the C string holding the regular expression to match
 * @param len the length of the pattern
 * @return -1 if an error happens, 0 otherwise
 *
 * This function will match the regular expression @ref pattern with the
 * given @ref string. If a match is found, it will be stored as a token.
 *
 * If you use the parenthesis to extract substrings, you can get the
 * substrings as subtokens of the matching token.
 *
 */

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */

ASKL_API void string_free_token(String *string);

/**
 * @ingroup string
 * @fn void string_free_token(String *string)
 * @param string the string to be cleaned.
 * @return void
 *
 * This function deletes all the tokens of the given string.
 * See @ref String or @ref string_split() for more informations about
 * the tokens.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API void string_api_cleanup(void);

/* -------------------------------------------------------------------------- */

#endif
