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

#ifndef JSTR_HAVENT_MEMCASECHR_SIMD
	char buf[128];
	strcpy(buf, "abcABC");
	assert(jstr_internal_simd_memcasechr(buf, 'd', 6) == NULL);
	assert(jstr_internal_simd_memcasechr(buf, 'a', 0) == NULL);
	assert(jstr_internal_simd_memcasechr(buf, 'C', 6) == buf + 2);
	assert(jstr_internal_simd_memcasechr(buf, 'a', 3) == buf);
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
