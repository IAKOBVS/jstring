#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/simd.h"

int
main(void)
{

#ifndef JSTR_HAVENT_COUNTCHR_SIMD
	assert(jstr_internal_simd_countchr("abacaba", 'a') == 4);
	assert(jstr_internal_simd_countchr("", 'a') == 0);
	assert(jstr_internal_simd_countchr("bbb", 'a') == 0);
#endif

	return EXIT_SUCCESS;
}
