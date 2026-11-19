#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/musl/stdstring.h"

int
main(void)
{

	char buf[128];
	strcpy(buf, "abac");
	assert(jstr_internal_strnchr_musl(buf, 'a', 4) == buf);
	assert(jstr_internal_strnchr_musl(buf, 'c', 3) == NULL);
	assert(jstr_internal_strnchr_musl(buf, 'c', 4) == buf + 3);
	assert(jstr_internal_strnchr_musl(buf, 'a', 0) == NULL);
	/* Regression: n past the terminator must not return the NUL pointer. */
	assert(jstr_internal_strnchr_musl(buf, 'z', 5) == NULL);
	assert(jstr_internal_strnchr_musl(buf, 'z', 99) == NULL);
	assert(jstr_internal_strnchr_musl(buf, 0, 5) == NULL);
	assert(jstr_internal_strnchr_musl(buf, 0, 99) == NULL);

	return EXIT_SUCCESS;
}
