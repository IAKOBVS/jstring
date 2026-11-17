#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __SYMBIAN32__
	assert(JSTR_OS_SYMBIAN == 1);
#else
	assert(JSTR_OS_SYMBIAN == 0);
#endif
	return 0;
}
