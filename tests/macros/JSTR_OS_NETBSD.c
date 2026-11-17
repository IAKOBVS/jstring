#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __NetBSD__
	assert(JSTR_OS_NETBSD == 1);
#else
	assert(JSTR_OS_NETBSD == 0);
#endif
	return 0;
}
