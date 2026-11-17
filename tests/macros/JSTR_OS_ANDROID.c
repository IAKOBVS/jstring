#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __ANDROID__
	assert(JSTR_OS_ANDROID == 1);
#else
	assert(JSTR_OS_ANDROID == 0);
#endif
	return 0;
}
