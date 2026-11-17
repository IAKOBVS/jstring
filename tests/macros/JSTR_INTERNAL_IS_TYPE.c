#include <assert.h>
#include "jstr.h"

int
main(void)
{
	if (JSTR_HAVE_TYPEOF && JSTR_HAVE_GENERIC) {
		assert(JSTR_INTERNAL_IS_TYPE(int, 0));
		assert(!JSTR_INTERNAL_IS_TYPE(double, 0));
	} else {
		assert(JSTR_INTERNAL_IS_TYPE(int, 0));
	}
	return 0;
}
