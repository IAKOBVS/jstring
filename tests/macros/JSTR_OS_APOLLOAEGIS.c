#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined aegis
	assert(JSTR_OS_APOLLOAEGIS == 1);
#else
	assert(JSTR_OS_APOLLOAEGIS == 0);
#endif
	return 0;
}
