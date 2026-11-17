#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __VXWORKS__ || defined __vxworks
	assert(JSTR_OS_VXWORKS == 1);
#else
	assert(JSTR_OS_VXWORKS == 0);
#endif
	return 0;
}
