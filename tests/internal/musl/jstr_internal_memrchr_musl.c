#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../test.h"
#include "internal/musl/stdstring.h"

int
main(void)
{

	char buf[128];
	strcpy(buf, "abcab");
	assert(jstr_internal_memrchr_musl(buf, 'b', 5) == buf + 4);
	assert(jstr_internal_memrchr_musl(buf, 'z', 5) == NULL);
	assert(jstr_internal_memrchr_musl(buf, 'a', 0) == NULL);
	assert(jstr_internal_memrchr_musl(buf, 'a', 3) == buf);

	return EXIT_SUCCESS;
}
