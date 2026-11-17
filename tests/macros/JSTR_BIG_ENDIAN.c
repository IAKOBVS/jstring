#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined(JSTR_BIG_ENDIAN)
# if defined(JSTR_BIG_ENDIAN) && defined(JSTR_LITTLE_ENDIAN)
	assert(JSTR_BIG_ENDIAN == JSTR_BIG_ENDIAN || JSTR_BIG_ENDIAN == JSTR_LITTLE_ENDIAN);
# endif
#endif
	return 0;
}
