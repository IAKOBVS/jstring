#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if JSTR_HAVE_GENERIC
	char buf[sizeof(char *)];
	assert(JSTR_INTERNAL_IS_STR_STACK(&buf) == 1);
#endif
	return 0;
}
