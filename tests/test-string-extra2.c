#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <string.h>
#include "jstr.h"

int main(void) {
	/* jstr_strcasestr_len: hs_len < ne_len */
	{
		const char *hs = "hello";
		const char *ne = "HELLO WORLD";
		char *r = jstr_strcasestr_len(hs, 5, ne, 11);
		assert(r == NULL);
	}
	/* jstr_strcasestr_len: ne_len == 0 */
	{
		const char *hs = "hello";
		char *r = jstr_strcasestr_len(hs, 5, "", 0);
		assert(r == hs);
	}
	/* jstr_strcasestr_len: ne_len > 8 (musl twoway path) */
	{
		const char *hs = "xxAbCdEfGhIjXx";
		const char *ne = "aBcDeFgHiJ";
		char *r = jstr_strcasestr_len(hs, 14, ne, 10);
		assert(r == hs + 2);
	}
	/* jstr_strcasestr_len_comp + exec: ne_len == 2 */
	{
		jstr_twoway_ty t = JSTR_INIT;
		jstr_strcasestr_len_comp(&t, "aB", 2);
		const char *hs = "xABy";
		char *r = jstr_strcasestr_len_exec(&t, hs, 4, "aB", 2);
		assert(r == hs + 1);
	}
	/* jstr_strcasestr_len_comp + exec: ne_len == 1 */
	{
		jstr_twoway_ty t = JSTR_INIT;
		jstr_strcasestr_len_comp(&t, "X", 1);
		const char *hs = "abcXyz";
		char *r = jstr_strcasestr_len_exec(&t, hs, 6, "X", 1);
		assert(r == hs + 3);
	}
	/* jstr_strcasestr_len_comp + exec: ne_len == 0 */
	{
		jstr_twoway_ty t = JSTR_INIT;
		jstr_strcasestr_len_comp(&t, "", 0);
		const char *hs = "anything";
		char *r = jstr_strcasestr_len_exec(&t, hs, 8, "", 0);
		assert(r == hs);
	}
	/* jstr_strtok: all delimiter chars */
	{
		const char *save = ".,/";
		const char *tok = jstr_strtok(&save, ".,/");
		assert(tok == NULL);
	}
	/* jstr_thousep_len_p: empty string */
	{
		char buf[32] = "";
		char *end = jstr_thousep_len_p(buf, 0, ',');
		assert(end == buf);
	}
	/* jstr_thousepcpy_len_p: negative number */
	{
		char buf[32];
		char *end = jstr_thousepcpy_len_p(buf, "-1234", 5, ',');
		assert(end == buf + 6);
		assert(!strcmp(buf, "-1,234"));
	}
	/* jstr_strcasestr_len: ne_len == 0 (covers line 708) */
	{
		const char *hs = "anything";
		char *r = jstr_strcasestr_len(hs, 8, "", 0);
		assert(r == hs);
	}
	/* jstr_strcasestr_len_exec: ne_len == 2, hs_len < 2 (covers line 746) */
	{
		jstr_twoway_ty t = JSTR_INIT;
		jstr_strcasestr_len_comp(&t, "ab", 2);
		const char *hs = "x";
		char *r = jstr_strcasestr_len_exec(&t, hs, 1, "ab", 2);
		assert(r == NULL);
	}
	return 0;
}
