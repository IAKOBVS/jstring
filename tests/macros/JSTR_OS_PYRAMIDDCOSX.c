#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined pyr
	assert(JSTR_OS_PYRAMIDDCOSX == 1);
#else
	assert(JSTR_OS_PYRAMIDDCOSX == 0);
#endif
	return 0;
}
