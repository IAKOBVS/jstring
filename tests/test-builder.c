#define JSTR_DEBUG 1
#define JSTR_PANIC 1

#include "../src/jstr-regex.h"
#include "test.h"

#define T_APPEND(ret, fn, ...)                                                                         \
	do {                                                                                           \
		TESTING(fn);                                                                           \
		if (ret != fn(__VA_ARGS__)) {                                                          \
			fprintf(stderr, "%s\n", JSTR_STRING(fn) "()");                                 \
			jstr_debug(&j);                                                                \
			jstr_err_exit("");                                                             \
		}                                                                                      \
		ASSERT(strcmp(j.data, expected) == 0, j.data, expected);                               \
		ASSERT(j.size == strlen(expected), j.data, expected);                                  \
		ASSERT(memcmp(j.data, expected, j.size) == 0, j.data, expected);                       \
		ASSERT(strlen(expected) != 0 && j.size != 0 && j.capacity > j.size, j.data, expected); \
		jstr_free_j(&j);                                                                       \
	} while (0)

#define FILL(j, str) assert(JSTR_RET_SUCC == jstr_assign_len(JSTR_STRUCT(&(j)), str, strlen(str)))

jstr_ty j = JSTR_INIT;

char buf[4096] = { 0 };

int
main(int argc, char **argv)
{
	const char *expected;
	const char *find;
	const char *rplc;
	regex_t preg;
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_cat, JSTR_STRUCT(&j), "hello", " ", "world", NULL);
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_append_len, JSTR_STRUCT(&j), expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_prepend_len, JSTR_STRUCT(&j), expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_assign_len, JSTR_STRUCT(&j), expected, strlen(expected));
	FILL(j, "hello hello hello hello");
	find = "hello";
	rplc = "world";
	expected = "world hello hello hello";
	T_APPEND(JSTR_RET_SUCC, jstr_rplc_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));
	FILL(j, "hello hello hello hello");
	find = "hello";
	rplc = "world";
	expected = "world world world world";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));
	FILL(j, "hello hello hello hello");
	find = "hello";
	rplc = "";
	expected = "   ";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));
	FILL(j, "hello hello hello hello");
	find = "hello";
	rplc = "";
	expected = "   ";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));
	FILL(j, "hello hello hello hello");
	find = "[0-9A-Za-z]*";
	rplc = "";
	expected = "   ";
	assert(!jstrre_comp_chk(jstrre_comp(&preg, find, 0)));
	T_APPEND(JSTR_RET_SUCC, jstrre_rplcall_len, &preg, JSTR_STRUCT(&j), rplc, strlen(rplc), 0);
	FILL(j, "hello hello hello hello");
	find = "\\([0-9A-Za-z]*\\)";
	rplc = "\\1\\2";
	expected = "hellohello hellohello hellohello hellohello";
	assert(!jstrre_comp_chk(jstrre_comp(&preg, find, 0)));
	T_APPEND(JSTR_RET_SUCC, jstrre_rplcall_bref_len, &preg, JSTR_STRUCT(&j), rplc, strlen(rplc), 0, 3);
	SUCCESS();
	return 0;
}
