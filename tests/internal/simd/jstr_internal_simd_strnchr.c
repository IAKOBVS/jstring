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

#ifndef JSTR_HAVENT_STRNCHR_SIMD
	char buf[128];
	strcpy(buf, "abac");
	assert(jstr_internal_simd_strnchr(buf, 'a', 4) == buf);
	assert(jstr_internal_simd_strnchr(buf, 'c', 3) == NULL);
	assert(jstr_internal_simd_strnchr(buf, 'c', 4) == buf + 3);
	assert(jstr_internal_simd_strnchr(buf, 'a', 0) == NULL);
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
