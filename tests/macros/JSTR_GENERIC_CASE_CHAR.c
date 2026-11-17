#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if JSTR_HAVE_GENERIC
	char c = 'x';
	int r = _Generic(c, JSTR_GENERIC_CASE_CHAR(1), default: 0);
	assert(r == 1);
#endif
	return 0;
}
