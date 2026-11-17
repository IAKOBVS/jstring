#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if JSTR_HAVE_GENERIC
	const char *s = "x";
	int r = _Generic(s, JSTR_GENERIC_CASE_STR(1), default: 0);
	assert(r == 1);
#endif
	return 0;
}
