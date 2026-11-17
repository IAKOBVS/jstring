#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if JSTR_HAVE_GENERIC
	char buf[sizeof(char *)];
	int r = _Generic(&buf, JSTR_GENERIC_CASE_STR_STACK(1, &buf), default: 0);
	assert(r == 1);
#endif
	return 0;
}
