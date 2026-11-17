#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined _AIX || defined __TOS_AIX__
	assert(JSTR_OS_AIX == 1);
#else
	assert(JSTR_OS_AIX == 0);
#endif
	return 0;
}
