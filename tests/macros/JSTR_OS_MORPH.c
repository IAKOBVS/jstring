#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __MORPHOS__
	assert(JSTR_OS_MORPH == 1);
#else
	assert(JSTR_OS_MORPH == 0);
#endif
	return 0;
}
