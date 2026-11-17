#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if JSTR_HAVE_GENERIC
	const char *s = "x";
	assert(JSTR_INTERNAL_IS_STR(s));
	assert(!JSTR_INTERNAL_IS_STR(1));
#endif
	return 0;
}
