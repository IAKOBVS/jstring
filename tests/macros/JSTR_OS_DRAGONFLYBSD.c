#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __DragonFly__
	assert(JSTR_OS_DRAGONFLYBSD == 1);
#else
	assert(JSTR_OS_DRAGONFLYBSD == 0);
#endif
	return 0;
}
