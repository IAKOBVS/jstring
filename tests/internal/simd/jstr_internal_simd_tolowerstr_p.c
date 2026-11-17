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

	char buf[128];
	strcpy(buf, "ABC123");
	char *p = jstr_internal_simd_tolowerstr_p(buf);
	assert(p == buf + 6);
	assert(memcmp(buf, "abc123", 7) == 0);

	char buf2[128];
	strcpy(buf2, "");
	p = jstr_internal_simd_tolowerstr_p(buf2);
	assert(p == buf2);

	SUCCESS();
	return EXIT_SUCCESS;
}
