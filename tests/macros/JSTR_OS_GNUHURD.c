#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __GNU__ || defined __gnu_hurd__
	assert(JSTR_OS_GNUHURD == 1);
#else
	assert(JSTR_OS_GNUHURD == 0);
#endif
	return 0;
}
