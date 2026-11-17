#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __OS400__
	assert(JSTR_OS_IBMOS400 == 1);
#else
	assert(JSTR_OS_IBMOS400 == 0);
#endif
	return 0;
}
