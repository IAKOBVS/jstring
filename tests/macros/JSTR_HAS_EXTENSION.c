#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined(__has_extension) || defined(__glibc_has_extension)
	assert(JSTR_HAS_EXTENSION(c_static_assert) == 0 || JSTR_HAS_EXTENSION(c_static_assert) == 1);
#endif
	return 0;
}
