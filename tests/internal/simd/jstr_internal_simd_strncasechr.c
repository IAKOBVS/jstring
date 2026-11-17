#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/simd.h"

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

#ifndef JSTR_HAVENT_STRNCASECHR_SIMD
	char buf[128];
	strcpy(buf, "abcABC");
	assert(jstr_internal_simd_strncasechr(buf, 'd', 6) == NULL);
	assert(jstr_internal_simd_strncasechr(buf, 'B', 1) == NULL);
	assert(jstr_internal_simd_strncasechr(buf, 'B', 6) == buf + 1);
	assert(jstr_internal_simd_strncasechr(buf, 'a', 0) == NULL);

	/* Regression: the zero-byte mask stops the scan, but must not be
	 * reported as a match. With n > strlen the old code returned the
	 * terminating NUL pointer instead of NULL. */
	assert(jstr_internal_simd_strncasechr(buf, 'z', 7) == NULL);
	assert(jstr_internal_simd_strncasechr(buf, 'Z', 99) == NULL);
	assert(jstr_internal_simd_strncasechr(buf, 0, 7) == NULL);
	assert(jstr_internal_simd_strncasechr(buf, 0, 99) == NULL);
	/* Real case-insensitive matches still work for the same spans. */
	assert(jstr_internal_simd_strncasechr(buf, 'a', 7) == buf);
	assert(jstr_internal_simd_strncasechr(buf, 'A', 7) == buf);
	assert(jstr_internal_simd_strncasechr(buf, 'c', 7) == buf + 2);
	assert(jstr_internal_simd_strncasechr(buf, 'C', 7) == buf + 2);
	assert(jstr_internal_simd_strncasechr(buf, 'b', 99) == buf + 1);
	assert(jstr_internal_simd_strncasechr(buf, 'C', 99) == buf + 2);
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
