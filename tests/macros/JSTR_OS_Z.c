#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __MVS__ || defined __HOS_MVS__ || defined __TOS_MVS__
	assert(JSTR_OS_Z == 1);
#else
	assert(JSTR_OS_Z == 0);
#endif
	return 0;
}
