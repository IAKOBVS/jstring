#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/simd.h"

int
main(void)
{

#ifndef JSTR_HAVENT_STRNCHR_SIMD
	char buf[128];
	strcpy(buf, "abac");
	assert(jstr_internal_simd_strnchr(buf, 'a', 4) == buf);
	assert(jstr_internal_simd_strnchr(buf, 'c', 3) == NULL);
	assert(jstr_internal_simd_strnchr(buf, 'c', 4) == buf + 3);
	assert(jstr_internal_simd_strnchr(buf, 'a', 0) == NULL);

	/* Regression: the zero-byte mask stops the scan, but must not be
	 * reported as a match. With n > strlen the old code returned the
	 * terminating NUL pointer instead of NULL. */
	assert(jstr_internal_simd_strnchr(buf, 'z', 5) == NULL);
	assert(jstr_internal_simd_strnchr(buf, 'z', 99) == NULL);
	assert(jstr_internal_simd_strnchr(buf, 0, 5) == NULL);
	assert(jstr_internal_simd_strnchr(buf, 0, 99) == NULL);
	/* Real matches before the terminator still work for the same spans. */
	assert(jstr_internal_simd_strnchr(buf, 'a', 5) == buf);
	assert(jstr_internal_simd_strnchr(buf, 'c', 5) == buf + 3);
	assert(jstr_internal_simd_strnchr(buf, 'a', 99) == buf);
	assert(jstr_internal_simd_strnchr(buf, 'c', 99) == buf + 3);
#endif

	return EXIT_SUCCESS;
}
