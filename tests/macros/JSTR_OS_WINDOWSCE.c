#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined _WIN32_WCE
	assert(JSTR_OS_WINDOWSCE == 1);
#else
	assert(JSTR_OS_WINDOWSCE == 0);
#endif
	return 0;
}
