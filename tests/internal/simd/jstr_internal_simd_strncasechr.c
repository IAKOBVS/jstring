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
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
