#include <assert.h>
#include "jstr.h"

#include <ctype.h>
#include <string.h>

int
main(void)
{
	const char *s = JSTR_ALPHA_VOWEL_UPPER_STR;
	assert(strlen(s) == 5);
	for (size_t i = 0; i < strlen(s); ++i) {
		unsigned char ch = (unsigned char)s[i];
			if (ch == '\0')
				continue;
		assert(strchr("AIUEO", ch) != NULL);
		assert(strchr(s, ch) == strchr(s, ch));
	}
	for (size_t i = 0; i < strlen(s); ++i)
		for (size_t j = i + 1; j < strlen(s); ++j)
			assert(s[i] != s[j]);
	return 0;
}
