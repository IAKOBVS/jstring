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

#ifndef JSTR_HAVENT_COUNTCHR_LEN_SIMD
	char buf[] = "abac\0x";
	assert(jstr_internal_simd_countchr_len(buf, 'a', 7) == 2);
	assert(jstr_internal_simd_countchr_len(buf, 'a', 0) == 0);
	assert(jstr_internal_simd_countchr_len(buf, 'x', 7) == 1);
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
