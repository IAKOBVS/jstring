#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined apollo
	assert(JSTR_OS_APOLLODOMAIN == 1);
#else
	assert(JSTR_OS_APOLLODOMAIN == 0);
#endif
	return 0;
}
