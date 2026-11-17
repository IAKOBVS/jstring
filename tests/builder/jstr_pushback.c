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

	assert(jstr_pushback(&s, &sz, &cap, 'a') == JSTR_RET_SUCC);
	assert(sz == 1);
	assert(memcmp(s, "a", 2) == 0);

	assert(jstr_pushback(&s, &sz, &cap, 'b') == JSTR_RET_SUCC);
	assert(sz == 2);
	assert(memcmp(s, "ab", 3) == 0);

	assert(jstr_assign_len(&s, &sz, &cap, "xy", 2) == JSTR_RET_SUCC);
	assert(jstr_pushback(&s, &sz, &cap, 'z') == JSTR_RET_SUCC);
	assert(sz == 3);
	assert(memcmp(s, "xyz", 4) == 0);

	jstr_free(&s, &sz, &cap);
	SUCCESS();
	return EXIT_SUCCESS;
}
