#include <assert.h>
#include "jstr.h"

int
main(void)
{
	if (JSTR_HAVE_TYPEOF && JSTR_HAVE_GENERIC) {
		assert(JSTR_SAME_TYPE(1, 2));
		assert(!JSTR_SAME_TYPE(1, 2.0));
	} else {
		assert(JSTR_SAME_TYPE(1, 2));
	}
	return 0;
}
