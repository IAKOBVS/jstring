#include "jstr.h"

int main(int argc, char **argv)
{

	/* jarr(int, x); */
	/* jarr_alloc_cat(&x, 1, 2, 3, 4); */
	
	jstr_ty s;
#define S "hello world hello"
#define SL strlen(S)
#define N "hello"
	jstr_alloc_append_mem(&s.data, &s.size, &s.cap, S, SL);
	void *p = jstr_strrstr(s.data, N);
	if (p)
		puts(p);
	
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
