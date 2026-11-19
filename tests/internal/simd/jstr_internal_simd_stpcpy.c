#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/simd.h"

int
main(void)
{

	char dst[128];
	char *p = jstr_internal_simd_stpcpy(dst, "hello");
	assert(p == dst + 5);
	assert(memcmp(dst, "hello", 6) == 0);

	p = jstr_internal_simd_stpcpy(dst, "");
	assert(p == dst);
	assert(dst[0] == '\0');

	return EXIT_SUCCESS;
}
