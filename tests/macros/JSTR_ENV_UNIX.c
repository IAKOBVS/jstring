#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __unix__ || defined __unix
	assert(JSTR_ENV_UNIX == 1);
#else
	assert(JSTR_ENV_UNIX == 0);
#endif
	return 0;
}
