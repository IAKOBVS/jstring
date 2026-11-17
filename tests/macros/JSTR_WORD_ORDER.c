#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined(JSTR_WORD_ORDER)
# if defined(JSTR_BIG_ENDIAN) && defined(JSTR_LITTLE_ENDIAN)
	assert(JSTR_WORD_ORDER == JSTR_BIG_ENDIAN || JSTR_WORD_ORDER == JSTR_LITTLE_ENDIAN);
# endif
#endif
	return 0;
}
