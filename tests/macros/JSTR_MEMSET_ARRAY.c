#include <assert.h>
#include "jstr.h"

#include <string.h>

int
main(void)
{
	unsigned char a[256];
	unsigned char b[10];
	JSTR_MEMSET_ARRAY(a, 0x5a);
	JSTR_MEMSET_ARRAY(b, 0xa5);
	for (size_t i = 0; i < sizeof a; ++i)
		assert(a[i] == 0x5a);
	for (size_t i = 0; i < sizeof b; ++i)
		assert(b[i] == 0xa5);
	return 0;
}
