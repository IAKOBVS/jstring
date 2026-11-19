#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	jstr_ty j = JSTR_INIT;

	assert(jstr_reserve(&j.data, &j.size, &j.capacity, 100) == JSTR_RET_SUCC);
	assert(jstr_sprintf_j(&j, "val=%d", 777) == JSTR_RET_SUCC);
	assert(j.size == 7);
	assert(memcmp(j.data, "val=777", 8) == 0);

	jstr_free_j(&j);
	return EXIT_SUCCESS;
}
