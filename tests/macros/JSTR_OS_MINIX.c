#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __minix
	assert(JSTR_OS_MINIX == 1);
#else
	assert(JSTR_OS_MINIX == 0);
#endif
	return 0;
}
