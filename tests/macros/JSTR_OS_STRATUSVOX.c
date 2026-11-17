#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __VOS__
	assert(JSTR_OS_STRATUSVOX == 1);
#else
	assert(JSTR_OS_STRATUSVOX == 0);
#endif
	return 0;
}
