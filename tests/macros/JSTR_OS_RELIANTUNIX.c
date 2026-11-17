#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined sinux
	assert(JSTR_OS_RELIANTUNIX == 1);
#else
	assert(JSTR_OS_RELIANTUNIX == 0);
#endif
	return 0;
}
