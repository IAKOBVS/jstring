#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined _WINDU_SOURCE
	assert(JSTR_OS_WINDU == 1);
#else
	assert(JSTR_OS_WINDU == 0);
#endif
	return 0;
}
