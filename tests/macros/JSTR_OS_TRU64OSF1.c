#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __osf__ || defined __osf
	assert(JSTR_OS_TRU64OSF1 == 1);
#else
	assert(JSTR_OS_TRU64OSF1 == 0);
#endif
	return 0;
}
