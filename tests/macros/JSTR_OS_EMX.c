#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __EMX__
	assert(JSTR_OS_EMX == 1);
#else
	assert(JSTR_OS_EMX == 0);
#endif
	return 0;
}
