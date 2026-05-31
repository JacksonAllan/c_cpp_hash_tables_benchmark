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

#ifndef ASKL_VARIANT_H

#define ASKL_VARIANT_H

#include "askl.h"
#include "askl_string.h"

/** @defgroup variant ASKL::variant */

typedef struct Variant {
    union {
        void *pointer;
        uint64_t integer;
        double decimal;     /* assume IEEE754 64-bits double precision format */
    } value;
    union {
        struct {
            uint8_t  type;  /* reserved unless type & _VALUE_OBJECT */
            uint8_t  byte;
            uint16_t word;
            uint32_t dword;
        } fields;
        struct {
            uint8_t type;
            uint8_t bytes[7];
        } raw;
    } metadata;             /* 56 bits of reclaimed padding to store metadata */
} Variant;

/**
 * @ingroup variant
 * @struct Variant
 * 
 * The Variant struct provides a small tagged value type used by ASKL containers
 * (e.g. Map and Trie) to store and retrieve values with lightweight runtime
 * type identification.
 *
 * A @ref Variant is passed by value. The value is stored either as:
 *  - a pointer
 *  - a 64-bit integer
 *  - a 64-bit IEEE754 double
 *
 * A 1-byte type tag is stored in the metadata area.
 *
 * @note pointer Variants are opaque: the API does not manage the pointed memory
 *
 */


#define VALUE_NULL        0
#define VALUE_STRING      1
#define VALUE_INTEGER     2
#define VALUE_BOOLEAN     3
#define VALUE_DECIMAL     4
#define VALUE_POINTER     5
#define _VALUE_OBJECT  0x80

#define is_null(v) ((v).metadata.fields.type == VALUE_NULL)
#define is_string(v) ((v).metadata.fields.type == VALUE_STRING)
#define is_integer(v) ((v).metadata.fields.type == VALUE_INTEGER)
#define is_boolean(v) ((v).metadata.fields.type == VALUE_BOOLEAN)
#define is_decimal(v) ((v).metadata.fields.type == VALUE_DECIMAL)
#define is_pointer(v) ((v).metadata.fields.type == VALUE_POINTER)
#define _is_object(v) ((v).metadata.fields.type & _VALUE_OBJECT)

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_from_pointer(void *ptr);

/**
 * @ingroup variant
 * @fn Variant variant_from_pointer(void *ptr)
 * @param ptr pointer value to store
 * @return a pointer-typed variant
 *
 * Build a @ref VALUE_POINTER variant storing @p ptr.
 *
 * This function does not take ownership of @p ptr.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_from_integer(uint64_t i);

/**
 * @ingroup variant
 * @fn Variant variant_from_integer(uint64_t i)
 * @param i integer value to store
 * @return an integer-typed variant
 *
 * Build a @ref VALUE_INTEGER variant storing @p i.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_from_decimal(double d);

/**
 * @ingroup variant
 * @fn Variant variant_from_decimal(double d)
 * @param d double value to store
 * @return a decimal-typed variant
 *
 * Build a @ref VALUE_DECIMAL variant storing @p d.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_from_boolean(int b);

/**
 * @ingroup variant
 * @fn Variant variant_from_boolean(int b)
 * @param b boolean value (0 is false, non-zero is true)
 * @return a boolean-typed variant
 *
 * Build a @ref VALUE_BOOLEAN variant.
 *
 * The stored value is normalized to 0 or 1.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_from_string(String *s);

/**
 * @ingroup variant
 * @fn Variant variant_from_string(m_string *s)
 * @param s pointer to an @ref m_string
 * @return a string-typed variant
 *
 * Build a @ref VALUE_STRING variant storing @p s.
 *
 * This function stores the pointer as-is. It does not duplicate the string.
 * Ownership is not modified; if the string must outlive its original owner,
 * the caller must duplicate it prior to storing it.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_null(void);

/**
 * @ingroup variant
 * @fn Variant variant_null(void)
 * @return a null variant
 *
 * Return the canonical null value variant (@ref VALUE_NULL).
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_true(void);

/**
 * @ingroup variant
 * @fn Variant variant_true(void)
 * @return a TRUE boolean variant
 *
 * Return the canonical TRUE value variant.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_false(void);

/**
 * @ingroup variant
 * @fn Variant variant_false(void)
 * @return a FALSE boolean variant
 *
 * Return the canonical FALSE value variant.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API void *variant_to_pointer(Variant v);

/**
 * @ingroup variant
 * @fn void *variant_to_pointer(Variant v)
 * @param v a variant
 * @return the contained pointer
 *
 * Extract the pointer from @p v.
 *
 * If @p v is not a pointer variant, the program will terminate with an error.
 * Callers should test with @ref is_pointer() first.
 * 
 * @note VALUE_NULL or _VALUE_OBJECT are tolerated.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API uint64_t variant_to_integer(Variant v);

/**
 * @ingroup variant
 * @fn uint64_t variant_to_integer(Variant v)
 * @param v a variant
 * @return the contained integer value
 *
 * Extract the integer from @p v.
 *
 * If @p v is not an integer variant, the program will terminate with an error.
 * Callers should test with @ref is_integer() first.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API double variant_to_decimal(Variant v);

/**
 * @ingroup variant
 * @fn double variant_to_decimal(variant v)
 * @param v a variant
 * @return the contained double value
 *
 * Extract the double from @p v.
 *
 * If @p v is not a decimal variant, the program will terminate with an error.
 * Callers should test with @ref is_decimal() first.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int variant_to_boolean(Variant v);

/**
 * @ingroup variant
 * @fn int variant_to_boolean(variant v)
 * @param v a variant
 * @return 0 for false, non-zero for true
 *
 * Extract the boolean value from @p v.
 *
 * If @p v is not a boolean variant, the program will terminate with an error.
 * Callers should test with @ref is_boolean() first.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API String *variant_to_string(Variant v);

/**
 * @ingroup variant
 * @fn String *variant_to_string(Variant v)
 * @param v a variant
 * @return the contained @ref m_string pointer
 *
 * Extract the string pointer from @p v.
 *
 * If @p v is not a string variant, the program will terminate with an error.
 * Callers should test with @ref is_string() first.
 *
 * The returned pointer is not duplicated; ownership is unchanged.
 *
 */

/* -------------------------------------------------------------------------- */

ASKL_API int variant_equal(Variant a, Variant b);

/**
 * @ingroup variant
 * @fn int variant_equal(variant a, variant b)
 * @param a first variant
 * @param b second variant
 * @return non-zero if the variants are equal, zero otherwise
 *
 * This function compares two variants for equality.
 *
 * Two variants are considered equal if they have the same runtime type and
 * carry the same value for that type.
 *
 * @note For pointer-like types (VALUE_STRING, VALUE_POINTER, _VALUE_OBJECT),
 *       equality means the pointers are equal; it does not compare the
 *       pointed-to contents.
 *
 * @note For VALUE_DECIMAL, this function uses exact representation equality.
 *       This means that values that compare equal numerically may still be
 *       considered different if their bit patterns differ (e.g. +0.0 vs -0.0),
 *       and NaN payloads will only compare equal if their representations match
 */

#endif
