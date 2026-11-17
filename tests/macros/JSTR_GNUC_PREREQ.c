#include <assert.h>
#include "jstr.h"

int
main(void)
{
	int v = JSTR_GNUC_PREREQ(4, 0);
	assert(v == 0 || v == 1);
#if defined(__GNUC__) || defined(__clang__)
	assert(JSTR_GNUC_PREREQ(0, 0));
	assert(!JSTR_GNUC_PREREQ(9999, 0));
#endif
	return 0;
}
