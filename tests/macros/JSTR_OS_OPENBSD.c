#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __OpenBSD__
	assert(JSTR_OS_OPENBSD == 1);
#else
	assert(JSTR_OS_OPENBSD == 0);
#endif
	return 0;
}
