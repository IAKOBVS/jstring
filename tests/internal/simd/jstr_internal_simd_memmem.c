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

#ifndef JSTR_HAVENT_MEMMEM_SIMD
	char hs[] = "hello world";
	assert(jstr_internal_simd_memmem(hs, 11, "world", 5) == hs + 6);
	assert(jstr_internal_simd_memmem(hs, 11, "xyz", 3) == NULL);
	assert(jstr_internal_simd_memmem(hs, 11, "o", 1) == hs + 4);
	assert(jstr_internal_simd_memmem(hs, 11, "", 0) == hs);
	assert(jstr_internal_simd_memmem(hs, 5, "world", 5) == NULL);
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
