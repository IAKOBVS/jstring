#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	jstr_ty j = JSTR_INIT;

	assert(jstr_asprintf_j(&j, "ab") == JSTR_RET_SUCC);
	assert(jstr_asprintf_from_j(&j, 2, "test%d", 123) == JSTR_RET_SUCC);
	assert(j.size == 2 + 7);
	assert(memcmp(j.data + 2, "test123", 8) == 0);

	jstr_free_j(&j);
	return EXIT_SUCCESS;
}
