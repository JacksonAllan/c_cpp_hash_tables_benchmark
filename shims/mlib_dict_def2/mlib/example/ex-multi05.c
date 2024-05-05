#include <stdio.h>

#include "m-array.h"
#include "m-string.h"

/* This example show ARRAY_DEF usage with string,
   M_LET macro usage with complex initialization condition:
   how to initialize a dynamic array and initialize it with the given arguments.
   It show also the M_EACH keywork
*/

// Let's create a dynamic array of string_t and register it globaly
// a so called std::vector<std::string> in C++
ARRAY_DEF(vector_string, string_t)
#define M_OPL_vector_string_t() ARRAY_OPLIST(vector_string, STRING_OPLIST)

int main(void)
{
    // Let's define a new string named str
    M_LET(str, string_t)
    // Let's define msg as a vector_array_t
    M_LET( (msg, STRING_CTE("Hello"), STRING_CTE("C"), STRING_CTE("World!"), STRING_CTE("Let's"), STRING_CTE("discover"), STRING_CTE("M*LIB!")), vector_string_t) {
        // Iterate over all words of the array msg
        for M_EACH(word, msg, vector_string_t) {
            // Cat the current word into the target string
            string_cat(str, *word);
            // Also add a space character
            string_push_back(str, ' ');
        }
        // Let's define another string and set it to a formatted value.
        M_LET( (count, "\nTo display this it uses \"vector\" of %zu strings!", vector_string_size(msg)), string_t) {
            string_cat(str, count);
        }
        // Print the result
        printf("%s\n", string_get_cstr(str) );
    }
    return 0;
}
