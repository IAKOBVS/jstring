#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined _WIN16 || defined _WIN32 || defined _WIN64 || defined __WIN32__ || defined __TOS_WIN__ || defined __WINDOWS__
	assert(JSTR_OS_WINDOWS == 1);
#else
	assert(JSTR_OS_WINDOWS == 0);
#endif
	return 0;
}
