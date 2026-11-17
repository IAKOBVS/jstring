#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
#if JSTR_HAVE_GENERIC
	size_t v = 0;
	int r = _Generic(v, JSTR_GENERIC_CASE_SIZE(1), default: 0);
	assert(r == 1);
#endif
	return 0;
}
