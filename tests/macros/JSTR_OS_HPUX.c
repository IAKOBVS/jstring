#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined _hpux || defined hpux || defined __hpux
	assert(JSTR_OS_HPUX == 1);
#else
	assert(JSTR_OS_HPUX == 0);
#endif
	return 0;
}
