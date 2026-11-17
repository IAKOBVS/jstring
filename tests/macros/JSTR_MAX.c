#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(JSTR_MAX(3, 5) == 5);
	assert(JSTR_MAX(-1, 1) == 1);
	assert(JSTR_MAX(2, 2) == 2);
	return 0;
}
