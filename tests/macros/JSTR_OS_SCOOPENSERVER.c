#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined M_I386 || defined M_XENIX || defined _SCO_DS
	assert(JSTR_OS_SCOOPENSERVER == 1);
#else
	assert(JSTR_OS_SCOOPENSERVER == 0);
#endif
	return 0;
}
