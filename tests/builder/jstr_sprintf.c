#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	char *s = NULL;
	size_t sz = 0, cap = 0;

	assert(jstr_reserve(&s, &sz, &cap, 100) == JSTR_RET_SUCC);
	assert(jstr_sprintf(&s, &sz, &cap, "val=%d", 777) == JSTR_RET_SUCC);
	assert(sz == 7);
	assert(memcmp(s, "val=777", 8) == 0);

	jstr_free(&s, &sz, &cap);
	return EXIT_SUCCESS;
}
