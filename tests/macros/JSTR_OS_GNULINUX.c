#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __gnu_linux__
	assert(JSTR_OS_GNULINUX == 1);
#else
	assert(JSTR_OS_GNULINUX == 0);
#endif
	return 0;
}
