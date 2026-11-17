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

#ifndef JSTR_HAVENT_STRCHRNUL_SIMD
	char buf[128];
	strcpy(buf, "abcabc");
	assert(jstr_internal_simd_strchr(buf, 'b') == buf + 1);
	assert(jstr_internal_simd_strchr(buf, 'z') == NULL);
	assert(jstr_internal_simd_strchr(buf, 'a') == buf);

	char empty[] = "";
	assert(jstr_internal_simd_strchr(empty, 'a') == NULL);
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
