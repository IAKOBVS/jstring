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

#ifndef JSTR_HAVENT_COUNTCHR_SIMD
	assert(jstr_internal_simd_countchr("abacaba", 'a') == 4);
	assert(jstr_internal_simd_countchr("", 'a') == 0);
	assert(jstr_internal_simd_countchr("bbb", 'a') == 0);
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
