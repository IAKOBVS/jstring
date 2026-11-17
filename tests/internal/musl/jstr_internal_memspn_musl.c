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

	assert(jstr_internal_memspn_musl("aaab", "a", 4) == 3);
	assert(jstr_internal_memspn_musl("baaa", "a", 4) == 0);
	assert(jstr_internal_memspn_musl("abcd", "ab", 4) == 2);
	/* Regression: single-byte accept sets must honour n. */
	assert(jstr_internal_memspn_musl("aaaaX", "a", 4) == 4);
	assert(jstr_internal_memspn_musl("aaaaX", "a", 3) == 3);
	assert(jstr_internal_memspn_musl("aaaaX", "a", 2) == 2);
	assert(jstr_internal_memspn_musl("aaaaX", "a", 0) == 0);
	assert(jstr_internal_memspn_musl("aaaaX", "", 4) == 0);

	SUCCESS();
	return EXIT_SUCCESS;
}
