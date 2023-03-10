#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

int jstrp_cmp_greedy(char *s1, char *s2)
{
	int cns1 = 0, vw1 = 0, cns2 = 0, vw2 = 0;
	for ( ; cns1 == cns2 & vw1 == vw2; ++s1, ++s2) {
		switch (*s1) {
		CASE_CONSONANT:
			++cns1;
			break;
		CASE_VOWEL:
			++vw1;
			break;
		}
		switch (*s2) {
		CASE_CONSONANT:
			++cns2;
			continue;
		CASE_VOWEL:
			++vw2;
			continue;
		case '\0':;
		}
		if (cns1 == cns2 & vw1 == vw2)
			return 0;
	}
	return -1;
}

int jstrp_cmp_greedy_v2(char *s1, char *s2)
{
	for (int cns_dif = 0, vw_dif = 0 ; cns_dif != 2 & vw_dif != 2; ++s1, ++s2) {
		switch (*s1) {
		CASE_CONSONANT:
			if (*s1 != *s2)
				++cns_dif;
			continue;
		CASE_VOWEL:
			if (*s1 != *s2)
				++vw_dif;
			continue;
		case '\0':;
		}
		return 0;
	}
	return -1;
}

int main()
{
	char *s1 = "hillo";
	char *s2 = "hello";

	if (!jstrp_cmp_greedy_v2(s1, s2))
		puts("greedy_equal");
	else
		puts("is not equal");
}
