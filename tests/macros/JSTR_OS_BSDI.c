#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __bsdi__
	assert(JSTR_OS_BSDI == 1);
#else
	assert(JSTR_OS_BSDI == 0);
#endif
	return 0;
}
