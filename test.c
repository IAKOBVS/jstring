/* #include "jstr.h" */
/* #include "string-optype.h" */

#include "jstr_string.h"

JSTR_INLINE
JSTR_MAYBE_UNUSED
static char *
jstr_lltoa(char *JSTR_RST _s,
	unsigned long long _num) JSTR_NOEXCEPT
{
	enum { _base = 10 };
	if (_num <= 1) {
		_s[0] = _num % _base + '0';
		_s += 1;
	} else if (_num <= 10) {
		_s[0] = _num / _base % _base + '0';
		_s[1] = _num % _base + '0';
		_s += 2;
	} else if (_num <= 100) {
		_s[0] = _num / _base / _base % _base + '0';
		_s[1] = _num / _base % _base + '0';
		_s[2] = _num % _base + '0';
		_s += 3;
	} else if (_num <= 1000) {
		_s[0] = _num / _base / _base / _base % _base + '0';
		_s[1] = _num / _base / _base % _base + '0';
		_s[2] = _num / _base % _base + '0';
		_s[3] = _num % _base + '0';
		_s += 4;
	} else if (_num <= 10000) {
		_s[0] = _num / _base / _base / _base / _base % _base + '0';
		_s[1] = _num / _base / _base / _base % _base + '0';
		_s[2] = _num / _base / _base % _base + '0';
		_s[3] = _num / _base % _base + '0';
		_s[4] = _num % _base + '0';
		_s += 5;
	} else if (_num <= 100000) {
		_s[0] = _num / _base / _base / _base / _base / _base % _base + '0';
		_s[1] = _num / _base / _base / _base / _base % _base + '0';
		_s[2] = _num / _base / _base / _base % _base + '0';
		_s[3] = _num / _base / _base % _base + '0';
		_s[4] = _num / _base % _base + '0';
		_s[5] = _num % _base + '0';
		_s += 6;
	} else if (_num <= 1000000) {
		_s[0] = _num / _base / _base / _base / _base / _base / _base % _base + '0';
		_s[1] = _num / _base / _base / _base / _base / _base % _base + '0';
		_s[2] = _num / _base / _base / _base / _base % _base + '0';
		_s[3] = _num / _base / _base / _base % _base + '0';
		_s[4] = _num / _base / _base % _base + '0';
		_s[5] = _num / _base % _base + '0';
		_s[6] = _num % _base + '0';
		_s += 7;
	} else if (_num <= 10000000) {
		_s[0] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';
		_s[1] = _num / _base / _base / _base / _base / _base / _base % _base + '0';
		_s[2] = _num / _base / _base / _base / _base / _base % _base + '0';
		_s[3] = _num / _base / _base / _base / _base % _base + '0';
		_s[4] = _num / _base / _base / _base % _base + '0';
		_s[5] = _num / _base / _base % _base + '0';
		_s[6] = _num / _base % _base + '0';
		_s[7] = _num % _base + '0';
		_s += 8;
	} else if (_num <= 100000000) {
		_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';
		_s[1] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';
		_s[2] = _num / _base / _base / _base / _base / _base / _base % _base + '0';
		_s[3] = _num / _base / _base / _base / _base / _base % _base + '0';
		_s[4] = _num / _base / _base / _base / _base % _base + '0';
		_s[5] = _num / _base / _base / _base % _base + '0';
		_s[6] = _num / _base / _base % _base + '0';
		_s[7] = _num / _base % _base + '0';
		_s[8] = _num % _base + '0';
		_s += 9;
	} else /* if (_num <= 1000000000) */ {
		_s[0] = _num / _base / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';
		_s[1] = _num / _base / _base / _base / _base / _base / _base / _base / _base % _base + '0';
		_s[2] = _num / _base / _base / _base / _base / _base / _base / _base % _base + '0';
		_s[3] = _num / _base / _base / _base / _base / _base / _base % _base + '0';
		_s[4] = _num / _base / _base / _base / _base / _base % _base + '0';
		_s[5] = _num / _base / _base / _base / _base % _base + '0';
		_s[6] = _num / _base / _base / _base % _base + '0';
		_s[7] = _num / _base / _base % _base + '0';
		_s[8] = _num / _base % _base + '0';
		_s[9] = _num % _base + '0';
		_s += 10;
	}
	*_s = '\0';
	return _s;
}

int
main(int argc, char **argv)
{
	/* printf("%c\n", 403 % 10 + '0'); */
	/* printf("%c\n", 403 / 10 % 10 + '0'); */
	/* printf("%c\n", 403 / 10 / 10 % 10 + '0'); */

	char s[1024] = { 0 };
	_ltostr(s, 12345);
	puts(s);
	/* putchar('\n'); */
	/* fwrite(s, 1, 1024, stdout); */
	/* putchar('\n'); */

	/* jarr(int, x); */
	/* jarr_alloc_cat(&x, 1, 2, 3, 4); */

	/* 	jstr_ty s; */
	/* #define S  "12345678" */
	/* #define SL strlen(S) */
	/* #define N  "hello" */
	/* 	jstr_alloc_append_mem(&s.data, &s.size, &s.cap, S, SL); */
	/* 	void *old = s.data; */
	/* 	char *p = (char *)jstr_memrchr(s.data, '1', s.size); */
	/* 	printf("%p\n", old); */
	/* 	if (p) { */
	/* 		printf("%p\n", p); */
	/* 		printf("%s\n", p); */
	/* 	} */

	/* const char *s = "0123456789"; */
	/* size_t i = jstr_strrspn(s, "01234567890"); */
	/* printf("%zu\n", i); */

	/* jstr_ty s; */
	/* char *str = "hello-world"; */
	/* jstr_alloc_append_mem(&s.data, &s.size, &s.cap, str, strlen(str)); */
	/* if (s.data == NULL) */
	/* 	return 1; */
	/* char *searc = "\\w*"; */
	/* char *rplc = "01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; */
	/* jstr_debug(&s); */
	/* regex_t r; */
	/* jreg_errcode_ty err = jreg_rplcall_now_mem(&s.data, &s.size, &s.cap, searc, rplc, strlen(rplc), &r, 0, 0); */
	/* jstr_rplcall_mem(&s.data, &s.size, &s.cap, searc, rplc, strlen(searc), strlen(rplc)); */
	/* jreg_rmall_now(s.data, &s.size, searc, &r, 0, 0); */
	/* jstr_debug(&s); */
	/* jstr_free(s.data); */
	/* return err; */

	/* jarr(int, x); */
	/* jarr_alloc_cat(&x, 1); */
}
