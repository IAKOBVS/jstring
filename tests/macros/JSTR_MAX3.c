#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(JSTR_MAX3(3, 1, 2) == 3);
	assert(JSTR_MAX3(1, 2, 3) == 3);
	assert(JSTR_MAX3(3, 2, 1) == 3);
	assert(JSTR_MAX3(-1, 0, 1) == 1);
	return 0;
}
