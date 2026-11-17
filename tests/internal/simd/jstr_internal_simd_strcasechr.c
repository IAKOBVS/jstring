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

#ifndef JSTR_HAVENT_STRCASECHR_SIMD
	char buf[128];
	strcpy(buf, "abcABC");
	assert(jstr_internal_simd_strcasechr(buf, 'a') == buf);
	assert(jstr_internal_simd_strcasechr(buf, 'D') == NULL);
	assert(jstr_internal_simd_strcasechr(buf, 'c') == buf + 2);
#endif

	SUCCESS();
	return EXIT_SUCCESS;
}
