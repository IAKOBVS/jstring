#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/musl/stdstring.h"

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	assert(jstr_internal_memrspn_musl("aaabbb", "b", 6) == 3);
	assert(jstr_internal_memrspn_musl("aaaa", "b", 4) == 4);
	assert(jstr_internal_memrspn_musl("aaab", "b", 4) == 3);
	/* Regression: n == 0 must not form s - 1. */
	assert(jstr_internal_memrspn_musl("abc", "a", 0) == 0);
	assert(jstr_internal_memrspn_musl("abc", "", 3) == 0);

	SUCCESS();
	return EXIT_SUCCESS;
}
