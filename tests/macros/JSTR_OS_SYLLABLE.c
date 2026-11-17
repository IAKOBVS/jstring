#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __SYLLABLE__
	assert(JSTR_OS_SYLLABLE == 1);
#else
	assert(JSTR_OS_SYLLABLE == 0);
#endif
	return 0;
}
