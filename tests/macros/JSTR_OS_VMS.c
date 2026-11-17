#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined VMS || defined __VMS
	assert(JSTR_OS_VMS == 1);
#else
	assert(JSTR_OS_VMS == 0);
#endif
	return 0;
}
