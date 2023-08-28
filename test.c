#include "_jstr.h"

int main(int argc, char **argv)
{
	char s[] = "          hello world113a9        ";
	puts(s);
	printf("stlren:%zu\n", strlen(s));
	char *p = jstr_trim_mem_p(s, strlen(s));
	puts(s);
	printf("%zu\n", p - s);
	printf("stlren:%zu\n", strlen(s));
	/* if (p) */
	/* 	puts(p); */
	return 0;
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
	/* jreg_rmall_now_mem(s.data, &s.size, searc, &r, 0, 0); */
	/* jstr_debug(&s); */
	/* jstr_free(s.data); */
	/* return err; */
}
