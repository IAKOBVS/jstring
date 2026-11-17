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

	assert(jstr_internal_memcspn_musl("aaab", "b", 4) == 3);
	assert(jstr_internal_memcspn_musl("aaaa", "b", 4) == 4);
	assert(jstr_internal_memcspn_musl("baaa", "b", 4) == 0);
	assert(jstr_internal_memcspn_musl("abcd", "bc", 4) == 1);
	/* Regression: an empty reject set returns n over binary data. */
	{
		const char bin[4] = { 'a', 0, 'b', 'c' };
		assert(jstr_internal_memcspn_musl(bin, "", 3) == 3);
		assert(jstr_internal_memcspn_musl(bin, "", 0) == 0);
	}
	assert(jstr_internal_memcspn_musl("abc", "b", 0) == 0);

	SUCCESS();
	return EXIT_SUCCESS;
}
