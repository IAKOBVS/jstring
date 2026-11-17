#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(JSTR_MIN(3, 5) == 3);
	assert(JSTR_MIN(-1, 1) == -1);
	assert(JSTR_MIN(2, 2) == 2);
	return 0;
}
