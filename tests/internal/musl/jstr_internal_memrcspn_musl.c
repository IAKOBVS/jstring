#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/musl/stdstring.h"

int
main(void)
{

	assert(jstr_internal_memrcspn_musl("aaabaaa", "b", 7) == 4);
	assert(jstr_internal_memrcspn_musl("aaaa", "b", 4) == 0);
	assert(jstr_internal_memrcspn_musl("aaab", "b", 4) == 4);
	/* Regression: n == 0 must not form s - 1. */
	assert(jstr_internal_memrcspn_musl("abc", "a", 0) == 0);
	assert(jstr_internal_memrcspn_musl("abc", "", 3) == 3);

	return EXIT_SUCCESS;
}
