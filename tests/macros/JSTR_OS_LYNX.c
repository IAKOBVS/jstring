#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __Lynx__
	assert(JSTR_OS_LYNX == 1);
#else
	assert(JSTR_OS_LYNX == 0);
#endif
	return 0;
}
