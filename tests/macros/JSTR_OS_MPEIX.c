#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined mpeix || defined __mpexl
	assert(JSTR_OS_MPEIX == 1);
#else
	assert(JSTR_OS_MPEIX == 0);
#endif
	return 0;
}
