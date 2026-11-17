#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined _UNICOS
	assert(JSTR_OS_UNIC == 1);
#else
	assert(JSTR_OS_UNIC == 0);
#endif
	return 0;
}
