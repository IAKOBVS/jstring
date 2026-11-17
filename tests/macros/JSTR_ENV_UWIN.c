#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined _UWIN
	assert(JSTR_ENV_UWIN == 1);
#else
	assert(JSTR_ENV_UWIN == 0);
#endif
	return 0;
}
