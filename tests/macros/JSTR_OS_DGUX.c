#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined DGUX || defined __DGUX__ || defined __dgux__
	assert(JSTR_OS_DGUX == 1);
#else
	assert(JSTR_OS_DGUX == 0);
#endif
	return 0;
}
