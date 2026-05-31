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

#include "askl_variant.h"

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_from_pointer(void *ptr)
{
    Variant v = { { 0 } };
    v.metadata.fields.type = VALUE_POINTER;
    v.value.pointer = ptr;
    return v;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_from_integer(uint64_t i)
{
    Variant v = { { 0 } };
    v.metadata.fields.type = VALUE_INTEGER;
    v.value.integer = i;
    return v;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_from_decimal(double d)
{
    Variant v = { { 0 } };
    v.metadata.fields.type = VALUE_DECIMAL;
    v.value.decimal = d;
    return v;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_from_boolean(int b)
{
    Variant v = { { 0 } };
    v.metadata.fields.type = VALUE_BOOLEAN;
    v.value.integer = !! b;
    return v;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_from_string(String *s)
{
    Variant v = { { 0 } };
    v.metadata.fields.type = VALUE_STRING;
    v.value.pointer = s;
    return v;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_null(void)
{
    Variant v = { { 0 } };
    v.metadata.fields.type = VALUE_NULL;
    v.value.pointer = NULL;
    return v;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_true(void)
{
    Variant v = { { 0 } };
    v.metadata.fields.type = VALUE_BOOLEAN;
    v.value.integer = 1;
    return v;
}

/* -------------------------------------------------------------------------- */

ASKL_API Variant variant_false(void)
{
    Variant v = { { 0 } };
    v.metadata.fields.type = VALUE_BOOLEAN;
    return v;
}

/* -------------------------------------------------------------------------- */

ASKL_API void *variant_to_pointer(Variant v)
{
    if (! is_pointer(v)) {
        /* tolerate VALUE_NULL and _VALUE_OBJECT */
        if (! _is_object(v) && v.metadata.fields.type != VALUE_NULL)
            die("type error");
    }
    return v.value.pointer;
}

/* -------------------------------------------------------------------------- */

ASKL_API uint64_t variant_to_integer(Variant v)
{
    if (! is_integer(v)) die("type error");

    return v.value.integer;
}

/* -------------------------------------------------------------------------- */

ASKL_API double variant_to_decimal(Variant v)
{
    if (! is_decimal(v)) die("type error");
    return v.value.decimal;
}

/* -------------------------------------------------------------------------- */

ASKL_API int variant_to_boolean(Variant v)
{
    if (! is_boolean(v)) die("type error");
    return (v.value.integer);
}

/* -------------------------------------------------------------------------- */

ASKL_API String *variant_to_string(Variant v)
{
    if (! is_string(v)) die("type error");
    return (String *) v.value.pointer;
}

/* -------------------------------------------------------------------------- */

ASKL_API int variant_equal(Variant a, Variant b)
{
    if (a.metadata.fields.type != b.metadata.fields.type) return 0;

    switch (a.metadata.fields.type) {
    case VALUE_NULL: return 1;
    case VALUE_STRING: return (a.value.pointer == b.value.pointer);
    case VALUE_INTEGER:
    case VALUE_BOOLEAN: return (a.value.integer == b.value.integer);
    case VALUE_DECIMAL: {
        return memcmp(
            & a.value.decimal,
            & b.value.decimal,
            sizeof(a.value.decimal)
        ) == 0;
    }
    case VALUE_POINTER:
    case _VALUE_OBJECT: return (a.value.pointer == b.value.pointer);
    default: return 0;
    }
}

/* -------------------------------------------------------------------------- */
