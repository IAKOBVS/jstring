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

	assert(jstr_asprintf(&s, &sz, &cap, "num=%d", 99) == JSTR_RET_SUCC);
	assert(sz == 6);
	assert(memcmp(s, "num=99", 7) == 0);

	assert(jstr_asprintf(&s, &sz, &cap, "x") == JSTR_RET_SUCC);
	assert(sz == 1);
	assert(memcmp(s, "x", 2) == 0);

	jstr_free(&s, &sz, &cap);
	SUCCESS();
	return EXIT_SUCCESS;
}
