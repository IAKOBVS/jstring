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

	assert(jstr_assign_len(&s, &sz, &cap, "start", 5) == JSTR_RET_SUCC);
	assert(jstr_asprintf_append(&s, &sz, &cap, "|%d|", 42) == JSTR_RET_SUCC);
	assert(sz == 9);
	assert(memcmp(s, "start|42|", 10) == 0);

	jstr_free(&s, &sz, &cap);
	SUCCESS();
	return EXIT_SUCCESS;
}
