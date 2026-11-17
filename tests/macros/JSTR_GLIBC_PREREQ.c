#include <assert.h>
#include "jstr.h"

int
main(void)
{
	int v = JSTR_GLIBC_PREREQ(2, 0);
	assert(v == 0 || v == 1);
#ifdef __GLIBC__
	assert(JSTR_GLIBC_PREREQ(0, 0));
	assert(!JSTR_GLIBC_PREREQ(9999, 0));
#endif
	return 0;
}
