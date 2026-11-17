#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __sysv__ || defined __SVR4 || defined __svr4__ || defined _SYSTYPE_SRV4
	assert(JSTR_ENV_SVR4 == 1);
#else
	assert(JSTR_ENV_SVR4 == 0);
#endif
	return 0;
}
