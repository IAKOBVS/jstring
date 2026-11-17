#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __QNX__ || defined __QNXNTO__
	assert(JSTR_OS_QNSX == 1);
#else
	assert(JSTR_OS_QNSX == 0);
#endif
	return 0;
}
