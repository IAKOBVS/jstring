#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(JSTR_BETWEEN(2, 1, 3));
	assert(JSTR_BETWEEN(1, 1, 3));
	assert(JSTR_BETWEEN(3, 1, 3));
	assert(!JSTR_BETWEEN(0, 1, 3));
	assert(!JSTR_BETWEEN(4, 1, 3));
	return 0;
}
