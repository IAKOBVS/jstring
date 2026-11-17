#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	char buf[32];
	char *p = jstr_pushback_unsafe_p(buf, 0, 'a');
	assert(p == buf + 1);
	assert(memcmp(buf, "a", 2) == 0);

	p = jstr_pushback_unsafe_p(buf, 1, 'b');
	assert(p == buf + 2);
	assert(memcmp(buf, "ab", 3) == 0);

	SUCCESS();
	return EXIT_SUCCESS;
}
