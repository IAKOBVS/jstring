#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined UTS
	assert(JSTR_OS_UTS == 1);
#else
	assert(JSTR_OS_UTS == 0);
#endif
	return 0;
}
