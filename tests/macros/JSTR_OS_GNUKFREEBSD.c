#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __FreeBSD_kernel__ && defined __GLIBC__
	assert(JSTR_OS_GNUKFREEBSD == 1);
#else
	assert(JSTR_OS_GNUKFREEBSD == 0);
#endif
	return 0;
}
