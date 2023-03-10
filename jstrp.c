#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "macros.h"

#define CASE_VOWEL_LOWER case 'a': case 'i': case 'u': case 'e': case 'o'
#define CASE_VOWEL_UPPER case 'A': case 'I': case 'U': case 'E': case 'O'
#define CASE_VOWEL CASE_VOWEL_UPPER: CASE_VOWEL_LOWER

#define CASE_CONSONANT_LOWER case 'b': case 'c': case 'd': case 'f': case 'g': case 'h': case 'j': case 'k': case 'l': case 'm': case 'n': case 'p': case 'q': case 'r': case 's': case 't': case 'v': case 'w': case 'x': case 'y': case 'z'
#define CASE_CONSONANT_UPPER case 'B': case 'C': case 'D': case 'F': case 'G': case 'H': case 'J': case 'K': case 'L': case 'M': case 'N': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'V': case 'W': case 'X': case 'Y': case 'Z'
#define CASE_CONSONANT CASE_CONSONANT_UPPER: CASE_CONSONANT_LOWER

#define CASE_DIGIT case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
#define CASE_LOWER case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z'
#define CASE_UPPER case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z'

#define CASE_ALPHA CASE_LOWER: CASE_UPPER
#define CASE_ALPHANUM CASE_DIGIT: CASE_ALPHA

#define CASE_WHITESPACE case '\n': case '\t': case '\r': case ' '

static ALWAYS_INLINE void jstrp_pop(char **RESTRICT s, const int shift_left)
{
	char *RESTRICT start = *s;
	char *RESTRICT end = start + strlen(*s);
	for ( ; start + shift_left <= end; ++start)
		*start = *(start + shift_left);
}

int jstrp_cmp_greedy(char *s1, char *s2)
{
	int cns_dif = 0;
	int vw_dif = 0;
	for ( ; cns_dif != 2 & vw_dif != 2; ++s1, ++s2) {
		switch (*s1) {
		CASE_CONSONANT:
			cns_dif += (*s1 != *s2) ? 1 : 0;
			continue;
		CASE_VOWEL:
			vw_dif += (*s1 != *s2) ? 1 : 0;
			continue;
		case '\0':;
		}
		return 0;
	}
	return -1;
}

void jstrp_capitalizer(char **s)
{
	int need_capital = 0;
	for ( ;; ++s) {
		switch (**s) {
		CASE_CONSONANT_LOWER:
		CASE_VOWEL_LOWER:
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
		CASE_ALPHANUM:
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
	char s[] = "Hello world. hww";
	char *sp = s;
	jstrp_capitalizer(&sp);
	puts(sp);
}
