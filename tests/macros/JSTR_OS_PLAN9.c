#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined EPLAN9
	assert(JSTR_OS_PLAN9 == 1);
#else
	assert(JSTR_OS_PLAN9 == 0);
#endif
	return 0;
}
