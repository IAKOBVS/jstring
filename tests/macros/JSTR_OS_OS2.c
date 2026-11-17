#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined OS2 || defined _OS2 || defined __OS2__ || defined __TOS_OS2__
	assert(JSTR_OS_OS2 == 1);
#else
	assert(JSTR_OS_OS2 == 0);
#endif
	return 0;
}
