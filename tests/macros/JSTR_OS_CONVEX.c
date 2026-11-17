#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __convex__
	assert(JSTR_OS_CONVEX == 1);
#else
	assert(JSTR_OS_CONVEX == 0);
#endif
	return 0;
}
