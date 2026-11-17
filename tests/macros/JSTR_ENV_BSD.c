#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__ || defined __bsdi__ || defined __DragonFly__ || defined _SYSTYPE_BSD
	assert(JSTR_ENV_BSD == 1);
#else
	assert(JSTR_ENV_BSD == 0);
#endif
	return 0;
}
