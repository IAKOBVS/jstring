#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/simd.h"

int
main(void)
{

	char buf[128];
	strcpy(buf, "abc123");
	char *p = jstr_internal_simd_toupperstr_p(buf);
	assert(p == buf + 6);
	assert(memcmp(buf, "ABC123", 7) == 0);

	char buf2[128];
	strcpy(buf2, "");
	p = jstr_internal_simd_toupperstr_p(buf2);
	assert(p == buf2);

	return EXIT_SUCCESS;
}
