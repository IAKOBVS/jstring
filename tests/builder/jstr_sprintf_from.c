#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	char *s = NULL;
	size_t sz = 0, cap = 0;

	assert(jstr_reserve(&s, &sz, &cap, 100) == JSTR_RET_SUCC);
	assert(jstr_sprintf_from(&s, &sz, &cap, 3, "x%dy", 5) == JSTR_RET_SUCC);
	assert(sz == 3 + 3);
	assert(memcmp(s + 3, "x5y", 4) == 0);

	jstr_free(&s, &sz, &cap);
	SUCCESS();
	return EXIT_SUCCESS;
}
