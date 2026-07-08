/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"

int
main(int argc, char **argv)
{
	START();

	jstr_ty result = JSTR_INIT;

	/* === jstr_insert_unsafe, jstr_insert_len === */
	TESTING(jstr_insert_unsafe);
	{
		char buf[64] = "hello";
		size_t sz = strlen(buf);
		jstr_insert_unsafe(buf, sz, 0, "x", 1);
		assert(!strcmp(buf, "xhello"));

		memcpy(buf, "hello", 6);
		sz = 5;
		jstr_insert_unsafe(buf, sz, 5, "y", 1);
		assert(!strcmp(buf, "helloy"));

		memcpy(buf, "hello", 6);
		sz = 5;
		jstr_insert_unsafe(buf, sz, 2, "XYZ", 3);
		assert(!strcmp(buf, "heXYZllo"));
	}
	TESTING(jstr_insert_len);
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "hello", 5)));
		assert(!jstr_chk(jstr_insert_len(&s, &sz, &cap, 0, "X", 1)));
		assert(sz == 6);
		assert(!memcmp(s, "Xhello", 6));

		assert(!jstr_chk(jstr_insert_len(&s, &sz, &cap, sz, "!", 1)));
		assert(sz == 7);
		assert(!memcmp(s, "Xhello!", 7));
		free(s);
	}

	/* === jstr_insertafterchr_len === */
	TESTING(jstr_insertafterchr_len);
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "a,b,c", 5)));
		assert(!jstr_chk(jstr_insertafterchr_len(&s, &sz, &cap, ',', "X", 1)));
		assert(sz == 6);

		assert(!jstr_chk(jstr_insertafterchr_len(&s, &sz, &cap, 'z', "X", 1)));
		assert(sz == 6);
		free(s);
	}

	/* === jstr_insertafterallchr_len === */
	TESTING(jstr_insertafterallchr_len);
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "a,b,c", 5)));
		assert(!jstr_chk(jstr_insertafterallchr_len(&s, &sz, &cap, ',', "!", 1)));
		assert(sz == 7);

		jstr_empty(s, &sz);
		cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "abc", 3)));
		assert(!jstr_chk(jstr_insertafterallchr_len(&s, &sz, &cap, ',', "!", 1)));
		assert(!memcmp(s, "abc", 3));
		free(s);
	}

	/* === jstr_insertafter_len === */
	TESTING(jstr_insertafter_len);
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "abABab", 6)));
		assert(!jstr_chk(jstr_insertafter_len(&s, &sz, &cap, "AB", 2, "!", 1)));
		assert(sz == 7);

		jstr_empty(s, &sz);
		cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "a,b,c", 5)));
		assert(!jstr_chk(jstr_insertafter_len(&s, &sz, &cap, ",", 1, "!", 1)));
		assert(sz == 6);
		assert(!memcmp(s, "!a,b,c", 6));

		assert(!jstr_chk(jstr_insertafter_len(&s, &sz, &cap, "", 0, "X", 1)));

		assert(!jstr_chk(jstr_insertafter_len(&s, &sz, &cap, "ZZ", 2, "Y", 1)));
		assert(!memcmp(s, "!a,b,c", 6));
		free(s);
	}

	/* === jstr_insertafterall_len === */
	TESTING(jstr_insertafterall_len);
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "abABabAB", 8)));
		assert(!jstr_chk(jstr_insertafterall_len(&s, &sz, &cap, "AB", 2, "!", 1)));
		assert(sz == 10);

		assert(!jstr_chk(jstr_insertafterall_len(&s, &sz, &cap, "", 0, "X", 1)));
		assert(sz == 10);

		free(s);
	}

	/* === jstr_rmspn_len === */
	TESTING(jstr_rmspn_len);
	{
		char buf[] = "hello---world";
		size_t sz = strlen(buf);
		assert(jstr_rmspn_len(buf, &sz, "-") == 3);
		assert(sz == strlen("helloworld"));

		sz = strlen("abc");
		assert(jstr_rmspn_len(buf, &sz, "-") == 0);
	}
	TESTING(jstr_rmspn_len_from);
	{
		char buf[] = "hello---world---!";
		size_t sz = strlen(buf);
		assert(jstr_rmspn_len_from(buf, &sz, 5, "-") == 3);
		assert(sz == strlen("helloworld---!"));

		sz = 3;
		assert(jstr_rmspn_len_from(buf, &sz, 5, "-") == 0);
	}

	/* === jstr_rmallchr_from, jstr_rmallchr === */
	TESTING(jstr_rmallchr_from);
	{
		char buf[] = "hello world";
		size_t sz = strlen(buf);
		assert(jstr_rmallchr_from(buf, &sz, 3, 'l') == 2);
		assert(sz == 9);
	}
	TESTING(jstr_rmallchr);
	{
		char buf[] = "hello world";
		size_t sz = strlen(buf);
		assert(jstr_rmallchr(buf, &sz, 'l') == 3);
		assert(sz == strlen("heo word"));
	}

	/* === jstr_rm_len === */
	TESTING(jstr_rm_len);
	{
		char buf[] = "hello world";
		size_t sz = strlen(buf);
		assert(jstr_rm_len(buf, &sz, "world", 5) == 1);
		assert(sz == strlen("hello "));

		sz = strlen("abc");
		assert(jstr_rm_len(buf, &sz, "notfound", 8) == 0);
		assert(sz == 3);
	}
	TESTING(jstr_rm_len_exec);
	{
		char buf[] = "hello world";
		size_t sz = strlen(buf);
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "world", 5);
		assert(jstr_rm_len_exec(&t, buf, &sz, "world", 5) == 1);
		assert(sz == strlen("hello "));
	}
	TESTING(jstr_rmn_len_exec);
	{
		char buf[] = "ababab";
		size_t sz = strlen(buf);
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "ab", 2);
		assert(jstr_rmn_len_exec(&t, buf, &sz, "ab", 2, 2) == 2);
		assert(sz == strlen("ab"));
	}
	TESTING(jstr_rmall_len_exec);
	{
		char buf[] = "ababab";
		size_t sz = strlen(buf);
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "ab", 2);
		assert(jstr_rmall_len_exec(&t, buf, &sz, "ab", 2) == 3);
		assert(sz == 0);
	}
	TESTING(jstr_rmall_len);
	{
		char buf[] = "ababab";
		size_t sz = strlen(buf);
		assert(jstr_rmall_len(buf, &sz, "ab", 2) == 3);
		assert(sz == 0);
	}

	/* === jstr_rmlast_len, jstr_rmlast_len_from === */
	TESTING(jstr_rmlast_len);
	{
		char buf[] = "abcabcabc";
		size_t sz = strlen(buf);
		assert(jstr_rmlast_len(buf, &sz, "abc", 3) == 1);
		assert(sz == strlen("abcabc"));
	}
	TESTING(jstr_rmlast_len_from);
	{
		char buf[] = "abcabcabc";
		size_t sz = strlen(buf);
		assert(jstr_rmlast_len_from(buf, &sz, 3, "abc", 3) == 1);
		assert(sz == strlen("abcabc"));

		sz = 3;
		assert(jstr_rmlast_len_from(buf, &sz, 5, "abc", 3) == 0);
	}

	/* === jstr_rplclast_len === */
	TESTING(jstr_rplclast_len);
	{
		jstr_empty(result.data, &result.size);
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "abcabc", 6)));
		assert(jstr_rplclast_len(jstr_struct(&result), "abc", 3, "XY", 2) == 1);
		assert(result.size == 5);

		result.size = 0;
		result.data[0] = '\0';
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "abc", 3)));
		assert(jstr_rplclast_len(jstr_struct(&result), "ZZZ", 3, "XY", 2) == 0);
		assert(result.size == 3);
	}

	/* === jstr_rplcallchr, jstr_rplcnchr, jstr_rplcchr === */
	TESTING(jstr_rplcallchr);
	{
		char buf[] = "hello";
		jstr_rplcallchr(buf, 'l', 'x');
		assert(!strcmp(buf, "hexxo"));
	}
	TESTING(jstr_rplcnchr);
	{
		char buf[] = "hello";
		size_t n = jstr_rplcnchr(buf, 'l', 'x', 1);
		assert(n == 1);
		assert(!strcmp(buf, "hexlo"));

		memcpy(buf, "hello", 6);
		n = jstr_rplcnchr(buf, 'l', 'x', 2);
		assert(n == 2);
		assert(!strcmp(buf, "hexxo"));
	}
	TESTING(jstr_rplcchr);
	{
		char buf[] = "hello";
		assert(jstr_rplcchr(buf, 'l', 'x') == 1);
		assert(!strcmp(buf, "hexlo"));

		memcpy(buf, "hello", 6);
		assert(jstr_rplcchr(buf, 'z', 'x') == 0);
		assert(!strcmp(buf, "hello"));
	}

	/* === jstr_rplc_len_exec, jstr_rplcn_len_exec,
	       jstr_rplcall_len_exec === */
	TESTING(jstr_rplc_len_exec);
	{
		jstr_empty(result.data, &result.size);
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result),
			"hello world", 11)));
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "world", 5);
		assert(jstr_rplc_len_exec(&t, jstr_struct(&result),
			"world", 5, "there", 5) == 1);
	}
	TESTING(jstr_rplcn_len_exec);
	{
		jstr_empty(result.data, &result.size);
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "ababab", 6)));
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "ab", 2);
		assert(jstr_rplcn_len_exec(&t, jstr_struct(&result),
			"ab", 2, "X", 1, 2) == 2);
	}
	TESTING(jstr_rplcall_len_exec);
	{
		jstr_empty(result.data, &result.size);
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "ababab", 6)));
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "ab", 2);
		assert(jstr_rplcall_len_exec(&t, jstr_struct(&result),
			"ab", 2, "X", 1) == 3);
	}

	/* === jstr_place_len, jstr_place_len_unsafe,
	       jstr_placeafterchr_len, jstr_placeafter_len === */
	TESTING(jstr_place_len);
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "hello", 5)));
		assert(!jstr_chk(jstr_place_len(&s, &sz, &cap, 0, "HE", 2)));
		assert(!memcmp(s, "HEllo", 5));

		jstr_empty(s, &sz);
		cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "hi", 2)));
		assert(!jstr_chk(jstr_place_len(&s, &sz, &cap, 5, "hello", 5)));
		assert(sz == 10);
		assert(!memcmp(s, "hi\0\0\0hello", 10));
		free(s);
	}
	TESTING(jstr_place_len_unsafe);
	{
		char buf[] = "hello";
		jstr_place_len_unsafe(buf, 1, "xyz", 3);
		assert(!memcmp(buf, "hxyzo", 5));
	}
	TESTING(jstr_placeafterchr_len);
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "a,b", 3)));
		assert(!jstr_chk(jstr_placeafterchr_len(&s, &sz, &cap, ',', "XYZ", 3)));
		assert(sz == 3);

		jstr_empty(s, &sz);
		cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "abc", 3)));
		assert(!jstr_chk(jstr_placeafterchr_len(&s, &sz, &cap, 'z', "X", 1)));
		assert(!memcmp(s, "abc", 3));
		free(s);
	}
	TESTING(jstr_placeafter_len);
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "abABab", 6)));
		assert(!jstr_chk(jstr_placeafter_len(&s, &sz, &cap, "AB", 2, "!", 1)));
		assert(sz == 6);

		jstr_empty(s, &sz);
		cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "a,b,c", 5)));
		assert(!jstr_chk(jstr_placeafter_len(&s, &sz, &cap, ",", 1, "!", 1)));
		assert(sz == 5);

		assert(!jstr_chk(jstr_placeafter_len(&s, &sz, &cap, "", 0, "X", 1)));
		assert(sz == 5);

		assert(!jstr_chk(jstr_placeafter_len(&s, &sz, &cap, "ZZ", 2, "Y", 1)));
		assert(sz == 5);
		free(s);
	}

	/* === jstr_repeat_len === */
	TESTING(jstr_repeat_len);
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "ab", 2)));
		assert(!jstr_chk(jstr_repeat_len(&s, &sz, &cap, 3)));
		assert(!memcmp(s, "ababab", sz));

		jstr_empty(s, &sz);
		cap = 0;
		assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "x", 1)));
		assert(!jstr_chk(jstr_repeat_len(&s, &sz, &cap, 0)));
		assert(!memcmp(s, "x", 1));
		assert(!jstr_chk(jstr_repeat_len(&s, &sz, &cap, 1)));
		assert(!memcmp(s, "x", 1));
		free(s);
	}

	/* === edge cases for partially covered functions === */
	TESTING(jstr_rplcchr_len_notfound);
	{
		char buf[] = "hello";
		assert(jstr_rplcchr_len(buf, 5, 'z', 'x') == 0);
		assert(!strcmp(buf, "hello"));
	}
	TESTING(jstr_rplc_len_from_exec_edge);
	{
		jstr_empty(result.data, &result.size);
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "hi", 2)));
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "hi", 2);
		assert(jstr_rplc_len_from_exec(&t, jstr_struct(&result),
			5, "hi", 2, "x", 1) == 0);
	}
	TESTING(jstr_rplcallspn_from_edge);
	{
		char buf[] = "hello";
		size_t sz = strlen(buf);
		assert(jstr_rplcallspn_from(buf, &sz, 10, "aeiou", '_') == 0);
	}
	TESTING(jstr_rmspn_from_edge);
	{
		char buf[] = "hello";
		size_t sz = strlen(buf);
		assert(jstr_rmspn_from(buf, &sz, 10, "aeiou") == 0);
	}
	TESTING(jstr_rmallspn_from_edge);
	{
		char buf[] = "hello";
		size_t sz = strlen(buf);
		assert(jstr_rmallspn_from(buf, &sz, 10, "aeiou") == 0);
	}

	jstr_free_j(&result);
	SUCCESS();
	return EXIT_SUCCESS;
}
