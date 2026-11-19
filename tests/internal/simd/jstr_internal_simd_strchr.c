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
	assert(jstr_internal_simd_strchr(buf, 'b') == buf + 1);
	assert(jstr_internal_simd_strchr(buf, 'z') == NULL);
	assert(jstr_internal_simd_strchr(buf, 'a') == buf);

	char empty[] = "";
	assert(jstr_internal_simd_strchr(empty, 'a') == NULL);
#endif

	return EXIT_SUCCESS;
}
