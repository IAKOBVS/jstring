#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <string.h>
#include "jstr.h"

int main(void) {
	/* jstr_rmallchr_len_from */
	{
		char s[] = "hello world";
		size_t sz = 11;
		size_t n = jstr_rmallchr_len_from(s, &sz, 'o');
		assert(n == 2);
		assert(sz == 9);
		assert(!strcmp(s, "hell wrld"));
	}
	/* jstr_rmallchr_len */
	{
		char s[] = "hello world";
		size_t sz = 11;
		size_t n = jstr_rmallchr_len(s, &sz, 'l');
		assert(n == 3);
		assert(sz == 8);
		assert(!strcmp(s, "heo word"));
	}
	/* jstr_rmallchr_from */
	{
		char s[] = "abaaba";
		size_t sz = 6;
		size_t n = jstr_rmallchr_from(s, &sz, 3, 'a');
		assert(n == 2);
		assert(sz == 4);
		assert(!strcmp(s, "abab"));
	}
	/* jstr_rmallchr */
	{
		char s[] = "ababa";
		size_t sz = 5;
		size_t n = jstr_rmallchr(s, &sz, 'a');
		assert(n == 3);
		assert(sz == 2);
		assert(!strcmp(s, "bb"));
	}
	/* jstr_rmnchr */
	{
		char s[] = "abacada";
		size_t sz = 7;
		size_t n = jstr_rmnchr(s, &sz, 'a', 2);
		assert(n == 2);
		assert(sz == 5);
		assert(!strcmp(s, "bcada"));
	}
	/* jstr_rmchr_len_from */
	{
		char s[] = "abacada";
		size_t sz = 7;
		size_t n = jstr_rmchr_len_from(s, &sz, 1, 'a');
		assert(n == 1);
		assert(sz == 6);
	}
	/* jstr_rmchr_len */
	{
		char s[] = "abacada";
		size_t sz = 7;
		size_t n = jstr_rmchr_len(s, &sz, 'a');
		assert(n == 1);
		assert(sz == 6);
		assert(!strcmp(s, "bacada"));
	}
	/* jstr_rmchr_from */
	{
		char s[] = "abacada";
		size_t sz = 7;
		size_t n = jstr_rmchr_from(s, &sz, 3, 'a');
		assert(n == 1);
		assert(sz == 6);
	}
	/* jstr_rmchr */
	{
		char s[] = "abacada";
		size_t sz = 7;
		size_t n = jstr_rmchr(s, &sz, 'a');
		assert(n == 1);
		assert(sz == 6);
	}
	/* jstr_rmnchr_from: start_idx >= *sz */
	{
		char s[] = "hello";
		size_t sz = 5;
		size_t n = jstr_rmnchr_from(s, &sz, 10, 'l', 1);
		assert(n == 0);
		assert(sz == 5);
	}
	/* jstr_rmnchr_from: n == 0 */
	{
		char s[] = "hello";
		size_t sz = 5;
		size_t n = jstr_rmnchr_from(s, &sz, 0, 'l', 0);
		assert(n == 0);
		assert(sz == 5);
	}
	/* jstr_rmnchr_from: char not found */
	{
		char s[] = "hello";
		size_t sz = 5;
		size_t n = jstr_rmnchr_from(s, &sz, 0, 'z', 1);
		assert(n == 0);
		assert(sz == 5);
	}
	/* jstr_rmnchr_from: no match after start position */
	{
		char s[] = "aaa";
		size_t sz = 3;
		size_t n = jstr_rmnchr_from(s, &sz, 3, 'a', 1);
		assert(n == 0);
	}
	/* jstr_insertafterall_len: find_len == 1 (single-char shortcut) */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "aXbXc", 5)));
		int r = jstr_insertafterall_len(&s, &sz, &cap, "X", 1, "Y", 1);
		assert(r == 0);
		assert(sz == 7);
		assert(!strcmp(s, "aYXbYXc"));
		free(s);
	}
	/* jstr_rm_len_from_exec: find_len == 0 */
	{
		jstr_twoway_ty t = JSTR_INIT;
		jstr_memmem_comp(&t, "", 0);
		char s[] = "hello";
		size_t sz = 5;
		size_t n = jstr_rm_len_from_exec(&t, s, &sz, 0, "", 0);
		assert(n == 0);
	}
	/* jstr_rmlast_len_from: not found */
	{
		char s[] = "hello";
		size_t sz = 5;
		int r = jstr_rmlast_len_from(s, &sz, 0, "z", 1);
		assert(r == 0);
		assert(sz == 5);
	}
	/* jstr_rplc_len_from_exec: find_len == 0 */
	{
		jstr_twoway_ty t = JSTR_INIT;
		jstr_memmem_comp(&t, "", 0);
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "hello", 5)));
		int r = jstr_rplc_len_from_exec(&t, &s, &sz, &cap, 0, "", 0, "x", 1);
		assert(r == 0);
		free(s);
	}
	/* jstr_rplcn_len_from_exec: find_len == 0 */
	{
		jstr_twoway_ty t = JSTR_INIT;
		jstr_memmem_comp(&t, "", 0);
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "hello", 5)));
		size_t r = jstr_rplcn_len_from_exec(&t, &s, &sz, &cap, 0, "", 0, "x", 1, 1);
		assert(r == 0);
		free(s);
	}
	/* jstr_rplcall_len_from_exec */
	{
		jstr_twoway_ty t = JSTR_INIT;
		jstr_memmem_comp(&t, "x", 1);
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "axbxc", 5)));
		int r = jstr_rplcall_len_from_exec(&t, &s, &sz, &cap, 0, "x", 1, "yy", 2);
		assert(r == 2);
		assert(sz == 7);
		assert(!strcmp(s, "ayybyyc"));
		free(s);
	}
	/* jstr_rplcn_len */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "axbxc", 5)));
		size_t r = jstr_rplcn_len(&s, &sz, &cap, "x", 1, "yy", 2, 1);
		assert(r == 1);
		free(s);
	}
	/* jstr_rplcn_len: find_len == 0 (rplcn_len_from_exec skip) */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "hello", 5)));
		size_t r = jstr_rplcn_len(&s, &sz, &cap, "", 0, "x", 1, 1);
		assert(r == 0);
		free(s);
	}
	return 0;
}
