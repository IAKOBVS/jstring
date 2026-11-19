#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	char *s = NULL;
	size_t sz = 0, cap = 0;

	assert(jstr_asprintf(&s, &sz, &cap, "ab") == JSTR_RET_SUCC);
	assert(jstr_asprintf_from(&s, &sz, &cap, 2, "test%d", 123) == JSTR_RET_SUCC);
	assert(sz == 2 + 7);
	assert(memcmp(s + 2, "test123", 7) == 0);

	jstr_free(&s, &sz, &cap);
	return EXIT_SUCCESS;
}
