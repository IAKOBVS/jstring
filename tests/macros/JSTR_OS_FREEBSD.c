#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __FreeBSD__
	assert(JSTR_OS_FREEBSD == 1);
#else
	assert(JSTR_OS_FREEBSD == 0);
#endif
	return 0;
}
