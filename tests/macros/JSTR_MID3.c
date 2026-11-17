#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(JSTR_MID3(3, 1, 2) == 2);
	assert(JSTR_MID3(1, 2, 3) == 2);
	assert(JSTR_MID3(3, 2, 1) == 2);
	assert(JSTR_MID3(-1, 0, 1) == 0);
	return 0;
}
