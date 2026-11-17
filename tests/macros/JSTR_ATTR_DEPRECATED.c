#include <assert.h>
#include "jstr.h"

#if defined(__GNUC__) && !defined(__clang__)
JSTR_ATTR_DEPRECATED("old") static int helper(void) { return 7; }
#else
JSTR_ATTR_DEPRECATED("old", "new") static int helper(void) { return 7; }
#endif

int
main(void)
{
	(void)helper;
	return 0;
}
