#define JSTR_DEBUG 1

#include "../src/jstr-regex.h"
#include "test.h"

#define T_APPEND(ret, fn, ...)                                                                         \
	do {                                                                                           \
		TESTING(fn);                                                                           \
			jstr_debug(&j);                                                                \
		if (ret != fn(__VA_ARGS__)) {                                                          \
			fprintf(stderr, "%s", JSTR_STRING(fn) "()");                                   \
			jstr_err_exit("");                                                             \
		}                                                                                      \
		ASSERT(strcmp(j.data, expected) == 0, j.data, expected);                               \
		ASSERT(j.size == strlen(expected), j.data, expected);                                  \
		ASSERT(memcmp(j.data, expected, j.size) == 0, j.data, expected);                       \
		ASSERT(strlen(expected) != 0 && j.size != 0 && j.capacity > j.size, j.data, expected); \
		jstr_free_j(&j);                                                                       \
	} while (0)

jstr_ty j = { 0 };

char buf[4096] = { 0 };

int
main(int argc, char **argv)
{
	const char *expected;
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_cat, &j.data, &j.size, &j.capacity, "hello", " ", "world", NULL);
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_append_len, &j.data, &j.size, &j.capacity, expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_prepend_len, &j.data, &j.size, &j.capacity, expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_assign_len, &j.data, &j.size, &j.capacity, expected, strlen(expected));

	const char *hello4 = "hello hello hello hello";
	assert(JSTR_RET_SUCC == jstr_assign_len(&j.data, &j.size, &j.capacity, hello4, strlen(hello4)));
	expected = "world hello hello hello";
	T_APPEND(JSTR_RET_SUCC, jstr_rplc_len, &j.data, &j.size, &j.capacity, "hello", strlen("hello"), "world", strlen("world"));

	assert(JSTR_RET_SUCC == jstr_assign_len(&j.data, &j.size, &j.capacity, hello4, strlen(hello4)));
	expected = "world world world world";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, &j.data, &j.size, &j.capacity, "hello", strlen("hello"), "world", strlen("world"));

	expected = "   ";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, &j.data, &j.size, &j.capacity, "world", strlen("world"), "", strlen(""));

	/* regex_t r; */
	/* expected = "   "; */
	/* assert(!jstrre_comp_chk(jstrre_comp(&r, "[0-9A-Za-z]*", 0))); */
	/* T_APPEND(JSTRRE_RET_NOERROR, jstrre_rplcall_len, &r, &j.data, &j.size, &j.capacity, "", strlen(""), 0); */

	SUCCESS();
	return 0;
}
