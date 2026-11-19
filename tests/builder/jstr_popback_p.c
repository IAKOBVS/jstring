#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	char buf[32];
	strcpy(buf, "abc");
	char *p = jstr_popback_p(buf, 3);
	assert(p == buf + 2);
	assert(buf[2] == '\0');
	assert(memcmp(buf, "ab", 3) == 0);

	char buf2[32];
	strcpy(buf2, "a");
	p = jstr_popback_p(buf2, 0);
	assert(p == buf2);
	assert(buf2[0] == 'a');

	return EXIT_SUCCESS;
}
