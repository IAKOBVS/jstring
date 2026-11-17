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

	jstr_ty j = JSTR_INIT;

	assert(jstr_reserve(&j.data, &j.size, &j.capacity, 100) == JSTR_RET_SUCC);
	assert(jstr_sprintf_from_j(&j, 3, "x%dy", 5) == JSTR_RET_SUCC);
	assert(j.size == 3 + 3);
	assert(memcmp(j.data + 3, "x5y", 4) == 0);

	jstr_free_j(&j);
	SUCCESS();
	return EXIT_SUCCESS;
}
