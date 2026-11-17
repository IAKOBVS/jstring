#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined MSDOS || defined __MSDOS__ || defined _MSDOS || defined __DOS__
	assert(JSTR_OS_MSDOS == 1);
#else
	assert(JSTR_OS_MSDOS == 0);
#endif
	return 0;
}
