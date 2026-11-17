#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if JSTR_HAVE_GENERIC
	char c = 'x';
	assert(JSTR_INTERNAL_IS_CHAR(c));
	assert(!JSTR_INTERNAL_IS_CHAR(1));
#endif
	return 0;
}
