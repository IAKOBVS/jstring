#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined(__GNUC__) || defined(__clang__)
	assert(JSTR_ATTR_BUILTIN_CONSTANT_P(1) != 0);
#endif
	return 0;
}
