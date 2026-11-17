#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_ENDIAN_UNKNOWN == 0) || (JSTR_ENDIAN_UNKNOWN == 1));
	assert((JSTR_ENDIAN_BIG + JSTR_ENDIAN_LITTLE + JSTR_ENDIAN_UNKNOWN) == 1);
	return 0;
}
