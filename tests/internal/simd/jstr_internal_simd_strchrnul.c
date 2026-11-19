#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/simd.h"

int
main(void)
{

#ifndef JSTR_HAVENT_STRCHRNUL_SIMD
	char buf[128];
	strcpy(buf, "abcabc");
	assert(jstr_internal_simd_strchrnul(buf, 'b') == buf + 1);
	assert(jstr_internal_simd_strchrnul(buf, 'z') == buf + strlen(buf));
	assert(jstr_internal_simd_strchrnul(buf, '\0') == buf + strlen(buf));
#endif

	return EXIT_SUCCESS;
}
