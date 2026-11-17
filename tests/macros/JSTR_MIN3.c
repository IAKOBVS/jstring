#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(JSTR_MIN3(3, 1, 2) == 1);
	assert(JSTR_MIN3(1, 2, 3) == 1);
	assert(JSTR_MIN3(3, 2, 1) == 1);
	assert(JSTR_MIN3(-1, 0, 1) == -1);
	return 0;
}
