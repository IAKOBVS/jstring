/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../src/jstr-regex.h"
#include "test-array.h"

#define T(func, expected)                                                                 \
	do {                                                                              \
		assert(!jstr_chk(func));                                                  \
		ASSERT_ERRFUNC(func, (result).capacity > strlen(expected));               \
		ASSERT_ERRFUNC(func, (result).data);                                      \
		ASSERT_ERRFUNC(func, (result).size == strlen(expected));                  \
		ASSERT_ERRFUNC(func, !memcmp((result).data, expected, strlen(expected))); \
	} while (0)

#define T_P(func, expected)                                                               \
	do {                                                                              \
		func;                                                                     \
		ASSERT_ERRFUNC(func, (result).data);                                      \
		ASSERT_ERRFUNC(func, (result).size == strlen(expected));                  \
		ASSERT_ERRFUNC(func, !memcmp((result).data, expected, strlen(expected))); \
	} while (0)

#define T_ASSERT(func, result, expected)                                                                           \
	do {                                                                                                       \
		ASSERT_RESULT(func, (result).size == strlen(expected), (result).data, expected);                   \
		ASSERT_RESULT(func, memcmp((result).data, expected, (result).size) == 0, (result).data, expected); \
	} while (0)

#define T_APPEND(ret, func, ...)                  \
	do {                                      \
		TESTING(func);                    \
		if (ret != func(__VA_ARGS__)) {   \
			jstr_debug(&(result));    \
			jstr_errdie("");          \
		}                                 \
		T_ASSERT(func, result, expected); \
		(result).size = 0;                \
		*(result).data = '\0';            \
	} while (0)

#define T_APPEND_NORET(func, ...)                 \
	do {                                      \
		TESTING(func);                    \
		func(__VA_ARGS__);                \
		T_ASSERT(func, result, expected); \
		(result).size = 0;                \
		*(result).data = '\0';            \
	} while (0)

#define FILL(result, str) assert(!jstr_chk(jstr_assign_len(jstr_struct(&(result)), str, strlen(str))))

#define T_RPLC_INIT(buf, str, str_len)                                               \
	do {                                                                         \
		jstr_empty(buf.data, &buf.size);                                     \
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&buf), str, str_len))); \
		assert(buf.size == str_len);                                         \
		assert(!memcmp(buf.data, str, str_len));                             \
	} while (0)

#define ASSERT_RESULT_RMCHR(func, str, rplc, expr)                       \
	do {                                                             \
		if (jstr_unlikely(!(expr))) {                            \
			PRINTERR("\nAssertion failure: %s().\n", #func); \
			PRINTERR("String:%s.\n"                          \
			         "find  :%c.\n"                          \
			         "rplc  :%c.\n",                         \
			         str,                                    \
			         find,                                   \
			         rplc);                                  \
			PRINTERR("Result:\n");                           \
			jstr_debug(&(result));                           \
			PRINTERR("Expected:\n");                         \
			jstr_debug(&(expected));                         \
			T_DEBUG(func, expr);                             \
			assert(expr);                                    \
		}                                                        \
	} while (0)

#define ASSERT_RESULT_RPLCCHR(func, str, find, rplc, expr)               \
	do {                                                             \
		if (jstr_unlikely(!(expr))) {                            \
			PRINTERR("\nAssertion failure: %s().\n", #func); \
			PRINTERR("String  :%s.\n"                        \
			         "find    :%c.\n"                        \
			         "rplc    :%c.\n",                       \
			         str,                                    \
			         find,                                   \
			         rplc);                                  \
			PRINTERR("Result  :\n");                         \
			jstr_debug(&(result));                           \
			PRINTERR("Expected:\n");                         \
			jstr_debug(&(expected));                         \
			T_DEBUG(func, expr);                             \
			assert(expr);                                    \
		}                                                        \
	} while (0)

#define ASSERT_RESULT_RPLC(func, str, find, rplc, n, expr)               \
	do {                                                             \
		if (jstr_unlikely(!(expr))) {                            \
			PRINTERR("\nAssertion failure: %s().\n", #func); \
			PRINTERR("string:%s.\n"                          \
			         "find  :%s.\n"                          \
			         "rplc  :%s.\n",                         \
			         str,                                    \
			         find,                                   \
			         rplc);                                  \
			PRINTERR("string len:%zu.\n", strlen(str));      \
			PRINTERR("find len:%zu.\n", strlen(find));       \
			PRINTERR("rplc len:%zu.\n", strlen(rplc));       \
			PRINTERR("Result:\n");                           \
			PRINTERR("N     :%zu\n", n);                     \
			jstr_debug(&(result));                           \
			PRINTERR("Expected:\n");                         \
			jstr_debug(&(expected));                         \
			T_DEBUG(func, expr);                             \
			assert(expr);                                    \
		}                                                        \
	} while (0)

#define T_RMCHR(func, simple_func)                                                                                                             \
	do {                                                                                                                                   \
		TESTING(func);                                                                                                                 \
		T_FOREACHI(test_array_memcmp, i)                                                                                               \
		{                                                                                                                              \
			const char *str = test_array_memcmp[i].s1;                                                                             \
			const char find = *test_array_memcmp[i].s2;                                                                            \
			size_t str_len = strlen(str);                                                                                          \
			T_RPLC_INIT((result), str, str_len);                                                                                   \
			T_RPLC_INIT((expected), str, str_len);                                                                                 \
			t.n = i;                                                                                                               \
			t.result_n = func((result).data, &((result).size), 0, find, t.n);                                                      \
			t.expected_n = simple_func((expected).data, &((expected).size), 0, find, t.n);                                         \
			ASSERT_RESULT_RMCHR(func, str, find, t.expected_n == t.result_n);                                                      \
			ASSERT_RESULT_RMCHR(func, str, find, strlen((result).data) == strlen((expected).data));                                \
			ASSERT_RESULT_RMCHR(func, str, find, (result).size == (expected).size);                                                \
			ASSERT_RESULT_RMCHR(func, str, find, !memcmp((result).data, (expected).data, (result).size * sizeof(*(result).data))); \
		}                                                                                                                              \
	} while (0)

#define T_RPLCCHR(func, simple_func)                                                                                                                   \
	do {                                                                                                                                           \
		TESTING(func);                                                                                                                         \
		T_FOREACHI(test_array_memcmp, i)                                                                                                       \
		{                                                                                                                                      \
			const char *str = test_array_memcmp[i].s1;                                                                                     \
			const char find = *test_array_memcmp[i].s2;                                                                                    \
			const char rplc = *test_array_memcmp[i].s1;                                                                                    \
			size_t str_len = strlen(str);                                                                                                  \
			T_RPLC_INIT((result), str, str_len);                                                                                           \
			T_RPLC_INIT((expected), str, str_len);                                                                                         \
			t.n = i;                                                                                                                       \
			t.result_n = func((result).data, ((result).size), find, rplc, t.n);                                                            \
			t.expected_n = simple_func((expected).data, ((expected).size), find, rplc, t.n);                                               \
			ASSERT_RESULT_RPLCCHR(func, str, find, rplc, t.result_n == t.expected_n);                                                      \
			ASSERT_RESULT_RPLCCHR(func, str, find, rplc, !memcmp((result).data, (expected).data, (result).size * sizeof(*(result).data))); \
		}                                                                                                                                      \
	} while (0)

#define T_RM(func, simple_func)                                                                                                                      \
	do {                                                                                                                                         \
		TESTING(func);                                                                                                                       \
		T_FOREACHI(test_array_memmem, i)                                                                                                     \
		{                                                                                                                                    \
			const char *str = test_array_memmem[i].hs;                                                                                   \
			const char *find = test_array_memmem[i].ne;                                                                                  \
			size_t str_len = strlen(str);                                                                                                \
			size_t find_len = strlen(find);                                                                                              \
			T_RPLC_INIT((result), str, str_len);                                                                                         \
			T_RPLC_INIT((expected), str, str_len);                                                                                       \
			t.n = i;                                                                                                                     \
			t.result_n = func((result).data, &((result).size), find, find_len, t.n);                                                     \
			t.expected_n = simple_func((expected).data, &((expected).size), find, find_len, t.n);                                        \
			ASSERT_RESULT_RPLC(func, str, find, "", i, t.expected_n == t.result_n);                                                      \
			ASSERT_RESULT_RPLC(func, str, find, "", i, strlen((result).data) == strlen((expected).data));                                \
			ASSERT_RESULT_RPLC(func, str, find, "", i, (result).size == (expected).size);                                                \
			ASSERT_RESULT_RPLC(func, str, find, "", i, !memcmp((result).data, (expected).data, (result).size * sizeof(*(result).data))); \
		}                                                                                                                                    \
	} while (0)

#define T_RPLC(func, simple_func)                                                                                                                                               \
	do {                                                                                                                                                                    \
		TESTING(func);                                                                                                                                                  \
		T_FOREACHI(test_array_memmem, i)                                                                                                                                \
		{                                                                                                                                                               \
			int loop = 1;                                                                                                                                           \
			const char *str = test_array_memmem[i].hs;                                                                                                              \
			const char *find = test_array_memmem[i].ne;                                                                                                             \
			size_t find_len = strlen(find);                                                                                                                         \
			const char *rplc = find + find_len / 2;                                                                                                                 \
			size_t str_len = strlen(str);                                                                                                                           \
			size_t rplc_len = strlen(rplc);                                                                                                                         \
			for (;;) {                                                                                                                                              \
				t.n = i;                                                                                                                                        \
				T_RPLC_INIT((result), str, str_len);                                                                                                            \
				T_RPLC_INIT((expected), str, str_len);                                                                                                          \
				t.expected_n = func(&(result).data, &(result).size, &(result).capacity, (i < str_len) ? i : 0, find, find_len, rplc, rplc_len, t.n);            \
				t.result_n = simple_func(&(expected).data, &(expected).size, &(expected).capacity, (i < str_len) ? i : 0, find, find_len, rplc, rplc_len, t.n); \
				ASSERT_RESULT_RPLC(func, str, find, rplc, i, t.expected_n == t.result_n);                                                                       \
				ASSERT_RESULT_RPLC(func, str, find, rplc, i, strlen((result).data) == strlen((expected).data));                                                 \
				ASSERT_RESULT_RPLC(func, str, find, rplc, i, (result).size == (expected).size);                                                                 \
				ASSERT_RESULT_RPLC(func, str, find, rplc, i, !memcmp((result).data, (expected).data, (result).size * sizeof(*(result).data)));                  \
				if (!loop)                                                                                                                                      \
					break;                                                                                                                                  \
				loop = 0;                                                                                                                                       \
				JSTR_SWAP(const char *, find, rplc);                                                                                                            \
				JSTR_SWAP(size_t, find_len, rplc_len);                                                                                                          \
			}                                                                                                                                                       \
		}                                                                                                                                                               \
	} while (0)

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rmnchr_len_from(char *s, size_t *sz, size_t start_idx, char remove, size_t n)
{
	if (n == 0)
		return 0;
	size_t changed = 0;
	const char *src = s + start_idx;
	char *dst = s + start_idx;
	size_t k = *sz - start_idx;
	for (; k--;) {
		if (*src == remove && n) {
			--n;
			++src;
			++changed;
		} else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';
	*sz = JSTR_PTR_DIFF(dst, s);
	return changed;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rplcnchr_len(char *s, size_t sz, char remove, char replace, size_t n)
{
	if (n == 0)
		return 0;
	size_t k = sz;
	size_t changed = 0;
	for (; k--; ++s)
		if (*s == remove) {
			if (n-- == 0)
				break;
			*s = replace;
			++changed;
		}
	return changed;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rplcn_len_from(char **s, size_t *sz, size_t *cap, size_t start_idx, const char *find, size_t find_len, const char *rplc, size_t rplc_len, size_t n)
{
	if (n == 0 || *sz == 0 || find_len == 0)
		return 0;
	char *p = *s + start_idx;
	size_t changed = 0;
	for (; n-- && (p = simple_memmem(p, JSTR_PTR_DIFF(*s + *sz, p), find, find_len)); ++changed) {
		p = jstr_rplcat_len(s, sz, cap, JSTR_PTR_DIFF(p, *s), rplc, rplc_len, find_len);
		assert(p != NULL);
	}
	return changed;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rmn_len(char *s, size_t *sz, const char *find, size_t find_len, size_t n)
{
	size_t cap = (size_t)-1;
	return simple_rplcn_len_from(&s, sz, &cap, 0, find, find_len, "", 0, n);
}

int
main(int argc, char **argv)
{
	START();
	regex_t preg;

#if 0

	/* jstr-replace tests. */
	jstr_ty result = JSTR_INIT;
	{
		jstr_ty expected = JSTR_INIT;
		T_RMCHR(jstr_rmnchr_len_from, simple_rmnchr_len_from);
		T_RPLCCHR(jstr_rplcnchr_len, simple_rplcnchr_len);
		T_RM(jstr_rmn_len, simple_rmn_len);
		T_RPLC(jstr_rplcn_len_from, simple_rplcn_len_from);
		jstr_free_j(&expected);
		jstr_empty(result.data, &result.size);
	}

	const char *expected, *find, *rplc;

	/* TODO: more tests. */

	/* jstr-builder tests. */
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_cat, jstr_struct(&result), "hello", " ", "world", NULL);
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_append_len, jstr_struct(&result), expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_prepend_len, jstr_struct(&result), expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_assign_len, jstr_struct(&result), expected, strlen(expected));

	/* jstr-replace tests. */
	FILL(result, "hello hello hello hello");
	find = "hello";
	rplc = "world";
	expected = "world hello hello hello";
	T_APPEND_NORET(jstr_rplc_len, jstr_struct(&result), find, strlen(find), rplc, strlen(rplc));
	FILL(result, "hello hello hello hello");
	find = "hello";
	rplc = "world";
	expected = "world world world world";
	T_APPEND_NORET(jstr_rplcall_len, jstr_struct(&result), find, strlen(find), rplc, strlen(rplc));
	FILL(result, "hello hello hello hello");
	find = "hello";
	rplc = "";
	expected = "   ";
	T_APPEND_NORET(jstr_rplcall_len, jstr_struct(&result), find, strlen(find), rplc, strlen(rplc));
	FILL(result, "hello hello hello hello");
	find = "hello";
	rplc = "";
	expected = "   ";
	T_APPEND_NORET(jstr_rplcall_len, jstr_struct(&result), find, strlen(find), rplc, strlen(rplc));

	/* jstr-builder tests. */
	jstr_empty(result.data, &result.size);
	T(jstr_cat(jstr_struct(&result), "hello", " world", NULL), "hello world");
	T(jstr_cat(jstr_struct(&result), "a", "b", NULL), "hello worldab");
	T(jstr_asprintf(jstr_struct(&result), "%s", "c"), "c");
	T(jstr_asprintf_append(jstr_struct(&result), "%s", "z"), "cz");
	T(jstr_assign_len(jstr_struct(&result), "hello", strlen("hello")), "hello");
	T(jstr_append_len(jstr_struct(&result), " world", strlen(" world")), "hello world");
	jstr_empty(result.data, &result.size);
	T_P(result.size = JSTR_PTR_DIFF(jstr_pushback_unsafe_p(result.data, result.size, 'k'), result.data), "k");
	T_P(result.size = JSTR_PTR_DIFF(jstr_pushfront_unsafe_p(result.data, result.size, 'l'), result.data), "lk");
	T_P(result.size = JSTR_PTR_DIFF(jstr_popfront_p(result.data, result.size), result.data), "k");
	T_P(result.size = JSTR_PTR_DIFF(jstr_popfront_p(result.data, result.size), result.data), "");
	T_P(result.size = JSTR_PTR_DIFF(jstr_popfront_p(result.data, result.size), result.data), "");
	T_P(result.size = JSTR_PTR_DIFF(jstr_popback_p(result.data, result.size), result.data), "");
	T_P(result.size = JSTR_PTR_DIFF(jstr_popback_p(result.data, result.size), result.data), "");
	T(jstr_assign_len(jstr_struct(&result), "hello", strlen("hello")), "hello");
	T_P(result.size = JSTR_PTR_DIFF(jstr_popback_p(result.data, result.size), result.data), "hell");
	T_P(result.size = JSTR_PTR_DIFF(jstr_popback_p(result.data, result.size), result.data), "hel");
	T_P(result.size = JSTR_PTR_DIFF(jstr_popfront_p(result.data, result.size), result.data), "el");
	T_P(result.size = JSTR_PTR_DIFF(jstr_popfront_p(result.data, result.size), result.data), "l");

#else
	jstr_ty result = JSTR_INIT;
	const char *expected, *find, *rplc;
#endif

#define T_RE(_string, _find, _rplc, _expected, _n)                                                           \
	do {                                                                                                 \
		FILL(result, _string);                                                                       \
		find = _find;                                                                                \
		rplc = _rplc;                                                                                \
		expected = _expected;                                                                        \
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, _find, 0)));                                     \
		T_APPEND_NORET(jstr_re_rplcn_len, &preg, jstr_struct(&result), _rplc, strlen(_rplc), 0, _n); \
		jstr_re_free(&preg);                                                                         \
	} while (0)

#define T_RE_BREF(_string, _find, _rplc, _expected, _nmatch, _n)                                                           \
	do {                                                                                                               \
		FILL(result, _string);                                                                                     \
		find = _find;                                                                                              \
		rplc = _rplc;                                                                                              \
		expected = _expected;                                                                                      \
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, _find, 0)));                                                   \
		T_APPEND_NORET(jstr_re_rplcn_bref_len, &preg, jstr_struct(&result), _rplc, strlen(_rplc), 0, _nmatch, _n); \
		jstr_re_free(&preg);                                                                                       \
	} while (0)

	/* jstr-regex tests. */
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", (size_t)-1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", 4);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___hello", 3);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "__hello_hello", 2);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "_hello_hello_hello", 1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "hello_hello_hello_hello", 0);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", (size_t)-1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", 4);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_hello", 3);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_hello_hello", 2);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_hello_hello_hello", 1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "hello_hello_hello_hello", 0);

	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", 1, (size_t)-1);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", 1, 4);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___hello", 1, 3);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "__hello_hello", 1, 2);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "_hello_hello_hello", 1, 1);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "hello_hello_hello_hello", 1, 0);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", 1, (size_t)-1);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", 1, 4);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_hello", 1, 3);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_hello_hello", 1, 2);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_hello_hello_hello", 1, 1);
	T_RE_BREF("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "hello_hello_hello_hello", 1, 0);
	
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hellohello_worldworld", 2, (size_t)-1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hellohello_worldworld", 2, 4);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hellohello_world", 2, 3);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hello_world", 2, 2);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_world_hello_world", 2, 1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hello_world_hello_world", 2, 0);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "helloworld_helloworld", 3, (size_t)-1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "helloworld_helloworld", 3, 2);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "helloworld_hello_world", 3, 1);
	T_RE_BREF("hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "hello_world_hello_world", 3, 0);

	jstr_free_j(&result);

	SUCCESS();
	return EXIT_SUCCESS;
}
