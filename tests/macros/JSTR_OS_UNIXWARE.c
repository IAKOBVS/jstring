#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined sco || defined _UNIXWARE7
	assert(JSTR_OS_UNIXWARE == 1);
#else
	assert(JSTR_OS_UNIXWARE == 0);
#endif
	return 0;
}
