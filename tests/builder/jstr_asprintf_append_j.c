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

	assert(jstr_assign_len(&j.data, &j.size, &j.capacity, "start", 5) == JSTR_RET_SUCC);
	assert(jstr_asprintf_append_j(&j, "|%d|", 42) == JSTR_RET_SUCC);
	assert(j.size == 9);
	assert(memcmp(j.data, "start|42|", 10) == 0);

	jstr_free_j(&j);
	SUCCESS();
	return EXIT_SUCCESS;
}
