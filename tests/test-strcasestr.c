#define _GNU_SOURCE

#include "test-array.h"
#include "test.h"

#include "/home/james/.local/src/glibc/sysdeps/x86_64/multiarch/strcasestr-avx2.c"

int
main(int argc, char **argv)
{
	START();
	/* T_STRSTR(jstr_strcasestr, simple_strcasestr, test_array_memmem); */
	T_STRSTR(FUNC_NAME, simple_strcasestr, test_array_memmem);
	SUCCESS();
	return EXIT_SUCCESS;
}
