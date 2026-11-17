#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if (defined sun || defined __sun) && (!defined __SVR4 && !defined __svr4__)
	assert(JSTR_OS_SUNOS == 1);
#else
	assert(JSTR_OS_SUNOS == 0);
#endif
	return 0;
}
