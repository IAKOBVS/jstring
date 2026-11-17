#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __linux__ || defined linux || defined __linux
	assert(JSTR_OS_LINUX == 1);
#else
	assert(JSTR_OS_LINUX == 0);
#endif
	return 0;
}
