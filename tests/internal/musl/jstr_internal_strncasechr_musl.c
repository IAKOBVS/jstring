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
	assert(jstr_internal_strncasechr_musl(buf, 'd', 6) == NULL);
	assert(jstr_internal_strncasechr_musl(buf, 'B', 1) == NULL);
	assert(jstr_internal_strncasechr_musl(buf, 'B', 6) == buf + 1);
	assert(jstr_internal_strncasechr_musl(buf, 'a', 0) == NULL);
	/* Regression: n past the terminator must not return the NUL pointer. */
	assert(jstr_internal_strncasechr_musl(buf, 'z', 7) == NULL);
	assert(jstr_internal_strncasechr_musl(buf, 'Z', 99) == NULL);
	assert(jstr_internal_strncasechr_musl(buf, 0, 7) == NULL);

	return EXIT_SUCCESS;
}
