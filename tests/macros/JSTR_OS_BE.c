#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __BEOS__
	assert(JSTR_OS_BE == 1);
#else
	assert(JSTR_OS_BE == 0);
#endif
	return 0;
}
