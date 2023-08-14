#ifndef JSTR_PP_VA_ARGS_MACROS_DEF_H
#define JSTR_PP_VA_ARGS_MACROS_DEF_H

#include "_jstr_pp_st_assert.h"
#include "_jstr_pp_strcpy_va_args.h"
#include "_jstr_pp_strlen2_va_args.h"
#include "_jstr_pp_strlen_va_args.h"
#include "_jstr_pp_va_args_tools.h"

#ifndef JSTR_PP_CASE_VOWEL_UPPER
#	define JSTR_PP_CASE_VOWEL_LOWER \
	case 'a':                        \
	case 'i':                        \
	case 'u':                        \
	case 'e':                        \
	case 'o':
#	define JSTR_PP_CASE_VOWEL_UPPER \
	case 'A':                        \
	case 'I':                        \
	case 'U':                        \
	case 'E':                        \
	case 'O':
#	define JSTR_PP_CASE_VOWEL \
JSTR_PP_CASE_VOWEL_UPPER:          \
		JSTR_PP_CASE_VOWEL_LOWER

#	define JSTR_PP_CASE_CONSONANT_LOWER \
	case 'b':                            \
	case 'c':                            \
	case 'd':                            \
	case 'f':                            \
	case 'g':                            \
	case 'h':                            \
	case 'j':                            \
	case 'k':                            \
	case 'l':                            \
	case 'm':                            \
	case 'n':                            \
	case 'p':                            \
	case 'q':                            \
	case 'r':                            \
	case 's':                            \
	case 't':                            \
	case 'v':                            \
	case 'w':                            \
	case 'x':                            \
	case 'y':                            \
	case 'z':
#	define JSTR_PP_CASE_CONSONANT_UPPER \
	case 'B':                            \
	case 'C':                            \
	case 'D':                            \
	case 'F':                            \
	case 'G':                            \
	case 'H':                            \
	case 'J':                            \
	case 'K':                            \
	case 'L':                            \
	case 'M':                            \
	case 'N':                            \
	case 'P':                            \
	case 'Q':                            \
	case 'R':                            \
	case 'S':                            \
	case 'T':                            \
	case 'V':                            \
	case 'W':                            \
	case 'X':                            \
	case 'Y':                            \
	case 'Z':
#	define JSTR_PP_CASE_CONSONANT \
JSTR_PP_CASE_CONSONANT_UPPER:          \
		JSTR_PP_CASE_CONSONANT_LOWER

#	define JSTR_PP_CASE_DIGIT \
	case '0':                  \
	case '1':                  \
	case '2':                  \
	case '3':                  \
	case '4':                  \
	case '5':                  \
	case '6':                  \
	case '7':                  \
	case '8':                  \
	case '9':
#	define JSTR_PP_CASE_LOWER \
	case 'a':                  \
	case 'b':                  \
	case 'c':                  \
	case 'd':                  \
	case 'e':                  \
	case 'f':                  \
	case 'g':                  \
	case 'h':                  \
	case 'i':                  \
	case 'j':                  \
	case 'k':                  \
	case 'l':                  \
	case 'm':                  \
	case 'n':                  \
	case 'o':                  \
	case 'p':                  \
	case 'q':                  \
	case 'r':                  \
	case 's':                  \
	case 't':                  \
	case 'u':                  \
	case 'v':                  \
	case 'w':                  \
	case 'x':                  \
	case 'y':                  \
	case 'z':
#	define JSTR_PP_CASE_UPPER \
	case 'A':                  \
	case 'B':                  \
	case 'C':                  \
	case 'D':                  \
	case 'E':                  \
	case 'F':                  \
	case 'G':                  \
	case 'H':                  \
	case 'I':                  \
	case 'J':                  \
	case 'K':                  \
	case 'L':                  \
	case 'M':                  \
	case 'N':                  \
	case 'O':                  \
	case 'P':                  \
	case 'Q':                  \
	case 'R':                  \
	case 'S':                  \
	case 'T':                  \
	case 'U':                  \
	case 'V':                  \
	case 'W':                  \
	case 'X':                  \
	case 'Y':                  \
	case 'Z':
#endif

#define JSTR_PP_ISDIGIT(c) ((c) >= 48 && (c) <= 57)

#define JSTR_PP_CONCATENATE(A, B) \
	JSTR_PP_CONCATENATE_(A, B)

#define JSTR_PP_CONCATENATE_(A, B) \
	A##B

#ifndef __cplusplus
#	define JSTR_PP_PRINTF_FORMAT(x) _Generic((x),                       \
						  unsigned char              \
						  : "%hhu\n",                \
						    unsigned char *          \
						  : "%s\n",                  \
						    char                     \
						  : "%c\n",                  \
						    char *                   \
						  : "%s\n",                  \
						    short                    \
						  : "%hd\n",                 \
						    unsigned short           \
						  : "%hu\n",                 \
						    unsigned int             \
						  : "%u\n",                  \
						    int                      \
						  : "%d\n",                  \
						    double                   \
						  : "%f\n",                  \
						    long double              \
						  : "%Lf\n",                 \
						    float                    \
						  : "%f\n",                  \
						    size_t                   \
						  : "%zu\n",                 \
						    long                     \
						  : "%ld\n",                 \
						    long long                \
						  : "%lld\n",                \
						    unsigned long long       \
						  : "%llu\n",                \
                                                                             \
						    const unsigned char      \
						  : "%hhu\n",                \
						    const unsigned char *    \
						  : "%s\n",                  \
						    const char               \
						  : "%c\n",                  \
						    const char *             \
						  : "%s\n",                  \
						    const short              \
						  : "%hd\n",                 \
						    const unsigned short     \
						  : "%hu\n",                 \
						    const unsigned int       \
						  : "%u\n",                  \
						    const int                \
						  : "%d\n",                  \
						    const double             \
						  : "%f\n",                  \
						    const long double        \
						  : "%Lf\n",                 \
						    const float              \
						  : "%f\n",                  \
						    const size_t             \
						  : "%zu\n",                 \
						    const long               \
						  : "%ld\n",                 \
						    const long long          \
						  : "%lld\n",                \
						    const unsigned long long \
						  : "%llu\n")
#	define jstr_pp_cout(x) printf(JSTR_PP_PRINTF_FORMAT(x), x)
#else
#	include <iostream>
#	define jstr_pp_cout(x) std::cout << x << '\n'
#endif /* __cplusplus */

#endif /* JSTR_PP_VA_ARGS_MACROS_DEF_H */
