/*
    impl_guard_test.c - regression for duplicate COREX_IMPLEMENTATION emission
    -------------------------------------------------------------------------
    When a dependent single-header library includes corex.h while
    COREX_IMPLEMENTATION is already defined, the implementation must be
    emitted at most once per translation unit.

    This file includes corex.h multiple times with COREX_IMPLEMENTATION set.
    A successful build proves the COREX_IMPLEMENTATION_INCLUDED guard works.

    Build:
        gcc -std=c99 -Wall -Wextra -Werror -pedantic -I. examples/impl_guard_test.c -o impl_guard_test -lm
*/

#define COREX_IMPLEMENTATION
#include "../corex.h"

#include "../corex.h" /* declarations only; implementation already included */

/* Simulate a dependent header that also defines COREX_IMPLEMENTATION. */
#define COREX_IMPLEMENTATION
#include "../corex.h"

#include <stdio.h>

int main(void)
{
    vec2 a = vec2_new((corex_real)3, (corex_real)4);
    corex_real len = vec2_len(a);

    if (len < (corex_real)4.999 || len > (corex_real)5.001)
        return 1;

    printf("impl_guard_test: ok (vec2_len(3,4) = %g)\n", (double)len);
    return 0;
}
