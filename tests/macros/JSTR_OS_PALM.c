#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __palmos__
	assert(JSTR_OS_PALM == 1);
#else
	assert(JSTR_OS_PALM == 0);
#endif
	return 0;
}
