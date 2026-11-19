#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/musl/stdstring.h"

int
main(void)
{

	char buf[128];
	strcpy(buf, "abcABC");
	assert(jstr_internal_memcasechr_musl(buf, 'd', 6) == NULL);
	assert(jstr_internal_memcasechr_musl(buf, 'a', 0) == NULL);
	assert(jstr_internal_memcasechr_musl(buf, 'C', 6) == buf + 2);
	assert(jstr_internal_memcasechr_musl(buf, 'a', 3) == buf);

	return EXIT_SUCCESS;
}
