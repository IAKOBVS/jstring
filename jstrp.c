#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex.h>
#include "macros.h"
#include "/home/james/c/pp_macros/global_macros.h"

int private_jstrp_rgxcmp(const char *RESTRICT s, const char *RESTRICT ptrn)
{
	regex_t rgx;
	regcomp(&rgx, ptrn, REG_EXTENDED);
	int ret = regexec(&rgx, s, 0, NULL, 0);
	regfree(&rgx);
	return ret;
}

static ALWAYS_INLINE void jstrp_pop(char **RESTRICT s, const int n)
{
	char *RESTRICT start = *s;
	const char *RESTRICT end = strchr(start, '\0');
	for ( ; start + n <= end; ++start)
		*start = *(start + n);
}

#define CONSONANT 0x01
#define VOWEL 0x10

int jstrp_cmp_greedy(const char *RESTRICT s1, const char *RESTRICT s2)
{
	for (int dif = 0; dif < 0x11; ++s1, ++s2) {
		switch (*s1) {
		CASE_CONSONANT
			dif += (*s1 != *s2) ? CONSONANT : 0;
			continue;
		CASE_VOWEL
			dif += (*s1 != *s2) ? VOWEL : 0;
			continue;
		case '\0':;
		}
		return dif;
	}
	return -1;
}

void jstrp_capitalizer(char **s)
{
	int need_capital = 0;
	for ( ;; ++s) {
		switch (**s) {
		CASE_CONSONANT_LOWER
		CASE_VOWEL_LOWER
			if (need_capital)
				**s = toupper(**s);
			continue;
		case '.':
			need_capital = 1;
			continue;
		case '\0':;
		}
		break;
	}
}

void jstrp_spacer(char **s)
{
	int spc = 0;
	for ( ;; ++s) {
		switch (**s) {
		CASE_ALPHANUM
			if (spc > 1) {
				jstrp_pop(s, spc);
				spc = 0;
			}
			continue;
		case ' ':
			++spc;
			continue;
		case '\0':;
		}
		break;
	}
}

int main()
{
	char s[] = "Hallo";
	char *sp = s;
	if (!jstrp_cmp_greedy(s, "hello"))
		puts("greedy_match");
	else
		puts("not match");
	/* jstrp_pop(&sp, 2); */
	/* puts(sp); */
}
