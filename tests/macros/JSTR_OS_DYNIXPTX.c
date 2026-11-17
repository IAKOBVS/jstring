#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined _SEQUENT_ || defined sequent
	assert(JSTR_OS_DYNIXPTX == 1);
#else
	assert(JSTR_OS_DYNIXPTX == 0);
#endif
	return 0;
}
