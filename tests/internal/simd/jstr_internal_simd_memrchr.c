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

#ifndef JSTR_HAVENT_MEMRCHR_SIMD
	char buf[128];
	strcpy(buf, "abcab");
	assert(jstr_internal_simd_memrchr(buf, 'b', 5) == buf + 4);
	assert(jstr_internal_simd_memrchr(buf, 'z', 5) == NULL);
	assert(jstr_internal_simd_memrchr(buf, 'a', 0) == NULL);
	assert(jstr_internal_simd_memrchr(buf, 'a', 3) == buf);
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
