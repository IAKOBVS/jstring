#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	jstr_ty j = JSTR_INIT;

	assert(jstr_asprintf_j(&j, "num=%d", 99) == JSTR_RET_SUCC);
	assert(j.size == 6);
	assert(memcmp(j.data, "num=99", 7) == 0);

	jstr_free_j(&j);
	return EXIT_SUCCESS;
}
