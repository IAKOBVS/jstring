#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	char buf[32];
	strcpy(buf, "abc");
	char *p = jstr_popfront_p(buf, 3);
	assert(p == buf + 2);
	assert(buf[0] == 'b');
	assert(buf[1] == 'c');
	assert(buf[2] == '\0');

	char buf2[32];
	strcpy(buf2, "a");
	p = jstr_popfront_p(buf2, 0);
	assert(p == buf2);

	return EXIT_SUCCESS;
}
