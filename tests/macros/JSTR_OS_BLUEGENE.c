#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __bg__ || defined __THW_BLUEGEN__
	assert(JSTR_OS_BLUEGENE == 1);
#else
	assert(JSTR_OS_BLUEGENE == 0);
#endif
	return 0;
}
