#ifndef JSTR_MACROS_DEF_H
#define JSTR_MACROS_DEF_H 1

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L && !defined __cplusplus
#	define JSTR_HAVE_GENERIC 1
#endif /* HAVE_GENERIC */

#if (defined __GNUC__ && (__GNUC__ >= 4)) || (defined __clang__ && (__clang_major__ >= 3))
#	define JSTR_HAVE_TYPEOF 1
#endif /* HAVE_TYPEOF */

#if defined JSTR_HAVE_TYPEOF && defined JSTR_HAVE_GENERIC
#	define JSTR_SAME_TYPE(x, y) _Generic((x), \
	__typeof__(y): 1,                          \
	default: 0)
#	define JSTR_IS_TYPE(T, x) _Generic((x), \
	T: 1,                                    \
	default: 0)
#endif /* HAVE_TYPEOF && JSTR_HAVE_GENERIC */

#if (defined __GNUC__ || defined __clang__) && JSTR_HAVE_GENERIC
#	define JSTR_GENERIC_CASE_SIZE(_expr)                           \
		int : _expr,                                            \
		      unsigned int : _expr,                             \
				     size_t : _expr,                    \
					      long : _expr,             \
						     long long : _expr, \
								 unsigned long long : _expr
#	define JSTR_GENERIC_CASE_STR(_bool) \
		char * : _bool,              \
			 const char * : _bool

#	define JSTR_GENERIC_CASE_STR_STACK(_bool, _s) \
		char(*)[sizeof(_s)] : 1,               \
		const char(*)[sizeof(_s)] : 1

#	define JSTR_GENERIC_CASE_CHAR(_bool) \
		char : _bool,                 \
		       const char : _bool

#	define JSTR_IS_SIZE(_expr) _Generic((_expr), \
	JSTR_GENERIC_CASE_SIZE(1),                    \
	default: 0)

#	define JSTR_IS_STR(_expr) _Generic((_expr), \
	JSTR_GENERIC_CASE_STR(1),                    \
	default: 0)

#	define JSTR_IS_STR_STACK(_expr) _Generic((_expr), \
	JSTR_GENERIC_CASE_STR_STACK(1, _expr),             \
	default: 0)

#	define JSTR_IS_CHAR(_expr) _Generic((_expr), \
	JSTR_GENERIC_CASE_CHAR(1),                    \
	default: 0)

#	define JSTR_ASSERT_IS_SIZE(_expr) \
		JSTR_ASSERT(JSTR_IS_SIZE(_expr), "Passing non-number as number argument!");
#	define JSTR_ASSERT_IS_STR(_expr) \
		JSTR_ASSERT(JSTR_IS_STR(_expr), "Passing non-string as string argument!");
#	define JSTR_ASSERT_IS_CHAR(_expr) \
		JSTR_ASSERT(JSTR_IS_CHAR(_expr), "Passing non-char as char argument!");
#	define JSTR_ASSERT_TYPECHECK(_expr_ty, _expr) \
		JSTR_ASSERT(JSTR_SAME_TYPE(_expr_ty, _expr), "Passing the wrong data type!");
#else
#	define JSTR_GENERIC_CASE_SIZE(_expr)
#	define JSTR_GENERIC_CASE_STR(_bool)
#	define JSTR_GENERIC_CASE_CHAR(_bool)
#	define JSTR_IS_STR(_expr)
#	define JSTR_IS_CHAR(_expr)
#	define JSTR_ASSERT_IS_SIZE(_expr)
#	define JSTR_ASSERT_IS_STR(_expr)
#	define JSTR_ASSERT_IS_CHAR(_expr)
#	define JSTR_ASSERT_TYPECHECK(_expr_ty, _expr)
#endif /* __GNUC__ || __clang__ */

#if __cplusplus > 199711L
#	define JSTR_NOEXCEPT noexcept
#else
#	define JSTR_NOEXCEPT
#endif

#ifdef static_assert
#	define JSTR_HAVE_STATIC_ASSERT		  1
#	define JSTR_ASSERT(_expr, msg)		  static_assert(_expr, msg)
#	define JSTR_ASSERT_SEMICOLON(_expr, msg) static_assert(_expr, msg);
#elif __STDC_VERSION__ >= 201112L
#	define JSTR_HAVE_STATIC_ASSERT 1
#	define JSTR_ASSERT(_expr, msg) _Static_assert(_expr, msg)
#else
#	define JSTR_ASSERT(_expr, msg)
#	define JSTR_ASSERT_SEMICOLON(_expr, msg)
#endif /* static_assert */

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#	define JSTR_RESTRICT restrict
#elif defined __GNUC__ || defined __clang__
#	define JSTR_RESTRICT __restrict__
#elif defined _MSC_VER
#	define JSTR_RESTRICT __restrict
#else
#	define JSTR_RESTRICT 1
#endif /* restrict */

#if (defined __GNUC__ && (__GNUC__ >= 3)) || (defined __clang__ && __has_builtin(__builtin_expect))
#	define jstr_likely(x)	 __builtin_expect((x), 1)
#	define jstr_unlikely(x) __builtin_expect((x), 0)
#else
#	define jstr_likely(x)	 (x)
#	define jstr_unlikely(x) (x)
#endif /* __has_builtin(__builtin_expect) */

#if defined __GNUC__ || defined __clang__

#	define JSTR_INLINE __attribute__((always_inline)) inline
#	if __has_attribute(pure)
#		define JSTR_PURE __attribute__((pure))
#	else
#		define JSTR_PURE
#	endif /* JSTR_PURE */
#	if __has_attribute(const)
#		define JSTR_CONST __attribute__((const))
#	else
#		define JSTR_CONST
#	endif /* JSTR_CONST */
#	if __has_attribute(flatten)
#		define JSTR_FLATTEN __attribute__((flatten))
#	else
#		define JSTR_FLATTEN
#	endif /* JSTR_FLATTEN */
#	if __has_attribute(cold)
#		define JSTR_COLD __attribute__((cold))
#	else
#		define JSTR_COLD
#	endif /* JSTR_COLD */
#	if __has_attribute(sentinel)
#		define JSTR_SENTINEL __attribute__((sentinel))
#	else
#		define JSTR_SENTINEL
#	endif /* JSTR_SENTINEL */
#	if __has_attribute(nonnull)
#		define JSTR_NONNULL_ALL   __attribute__((nonnull))
#		define JSTR_NONNULL(args) __attribute__((nonnull(args)))
#	else
#		define JSTR_NONNULL_ALL
#		define JSTR_NONNULL(args)
#	endif /* JSTR_NONNULL */
#	if __has_attribute(malloc)
#		define JSTR_MALLOC				      __attribute__((malloc))
#		define JSTR_MALLOC_DEALLOC(deallocator)	      __attribute__((malloc, deallocator))
#		define JSTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx) __attribute__((malloc, deallocator, ptr_idx))
#	endif /* JSTR_MALLOC */
#	if __has_attribute(returns_nonnull)
#		define JSTR_RETURNS_NONNULL __attribute__((returns_nonnull))
#	else
#		define JSTR_RETURNS_NONNULL
#	endif /* RETURNS_NONNULL */
#	if __has_attribute(warn_unused_result)
#		define JSTR_WARN_UNUSED __attribute__((warn_unused_result))
#	else
#		define JSTR_WARN_UNUSED
#	endif /* */
#	if __has_builtin(__builtin_constant_p)
#		define JSTR_CONSTANT_P(p) __builtin_constant_p(p)
#	else
#		define JSTR_CONSTANT_P(p) 0
#	endif /* __has_builtin(__builtin_constant_p) */
#	if __has_builtin(deprecated)
#		define JSTR_DEPRECATED(msg, replacement) __attribute__((deprecated(msg, replacement)))
#	else
#		define JSTR_DEPRECATED(msg, replacement)
#	endif /* JSTR_DEPRECATED */
#	if __has_attribute(nothrow)
#		define JSTR_NOTHROW __attribute__((nothrow))
#	else
#		define JSTR_NOTHROW
#	endif
#	if __has_attribute(__may_alias__)
#		define JSTR_MAY_ALIAS		 __attribute__((__may_alias__))
#		define JSTR_HAVE_ATTR_MAY_ALIAS 1
#	elif __has_attribute(may_alias)
#		define JSTR_MAY_ALIAS		 __attribute__((may_alias))
#		define JSTR_HAVE_ATTR_MAY_ALIAS 1
#	else
#		define JSTR_MAY_ALIAS
#	endif

#elif defined _MSC_VER

#	define JSTR_INLINE   __forceinline inline
#	define JSTR_NOINLINE __declspec(noinline)
#	define JSTR_PURE     __declspec(noalias)
#	define JSTR_CONST    __declspec(restrict)
#	define JSTR_FLATTEN
#	define JSTR_COLD
#	define JSTR_SENTINEL
#	define JSTR_NONNULL_ALL
#	define JSTR_NONNULL(args)
#	define JSTR_MALLOC
#	define JSTR_MALLOC_DEALLOC(deallocator)
#	define JSTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx)
#	define JSTR_RETURNS_NONNULL
#	define JSTR_CONSTANT_P(p) 0
#	define JSTR_WARN_UNUSED
#	define JSTR_DEPRECATED(msg, replacement)
#	define JSTR_NOTHROW __declspec(nothrow)
#	define JSTR_MAY_ALIAS

#else

#	define JSTR_INLINE inline
#	define JSTR_NOINLINE
#	define JSTR_PURE
#	define JSTR_CONST
#	define JSTR_FLATTEN
#	define JSTR_COLD
#	define JSTR_SENTINEL
#	define JSTR_NONNULL_ALL
#	define JSTR_NONNULL(args)
#	define JSTR_MALLOC
#	define JSTR_MALLOC_DEALLOC(deallocator)
#	define JSTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx)
#	define JSTR_CONSTANT_P(p) 0
#	define JSTR_WARN_UNUSED
#	define JSTR_DEPRECATED(msg, replacement)
#	define JSTR_NOTHROW
#	define JSTR_MAY_ALIAS

#endif /* Gnuc || clang || msvc */

#if defined __GNUC__ || defined __clang__
#	define JSTR_MAYBE_UNUSED __attribute__((unused))
#else
#	define JSTR_MAYBE_UNUSED
#endif /* maybe_unused */

#if defined __GNUC__ || defined __clang__
#	define JSTR_NOINLINE __attribute__((noinline))
#elif defined _MSC_VER
#	define JSTR_NOINLINE __declspec(noinline)
#else
#	define JSTR_NOINLINE
#endif /* noinline */

#ifndef PJSTR_MAX
#	define PJSTR_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif /* Max */

#ifndef JSTR_MIN
#	define JSTR_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif /* Min */

#define JSTR_CASE_VOWEL_LOWER \
case 'a':                     \
case 'i':                     \
case 'u':                     \
case 'e':                     \
case 'o':

#define JSTR_CASE_VOWEL_UPPER \
case 'A':                     \
case 'I':                     \
case 'U':                     \
case 'E':                     \
case 'O':

#define JSTR_CASE_CONSONANT_LOWER \
case 'b':                         \
case 'c':                         \
case 'd':                         \
case 'f':                         \
case 'g':                         \
case 'h':                         \
case 'j':                         \
case 'k':                         \
case 'l':                         \
case 'm':                         \
case 'n':                         \
case 'p':                         \
case 'q':                         \
case 'r':                         \
case 's':                         \
case 't':                         \
case 'v':                         \
case 'w':                         \
case 'x':                         \
case 'y':                         \
case 'z':
#define JSTR_CASE_CONSONANT_UPPER \
case 'B':                         \
case 'C':                         \
case 'D':                         \
case 'F':                         \
case 'G':                         \
case 'H':                         \
case 'J':                         \
case 'K':                         \
case 'L':                         \
case 'M':                         \
case 'N':                         \
case 'P':                         \
case 'Q':                         \
case 'R':                         \
case 'S':                         \
case 'T':                         \
case 'V':                         \
case 'W':                         \
case 'X':                         \
case 'Y':                         \
case 'Z':

#define JSTR_CASE_DIGIT \
case '0':               \
case '1':               \
case '2':               \
case '3':               \
case '4':               \
case '5':               \
case '6':               \
case '7':               \
case '8':               \
case '9':

#define JSTR_CASE_LOWER \
case 'a':               \
case 'b':               \
case 'c':               \
case 'd':               \
case 'e':               \
case 'f':               \
case 'g':               \
case 'h':               \
case 'i':               \
case 'j':               \
case 'k':               \
case 'l':               \
case 'm':               \
case 'n':               \
case 'o':               \
case 'p':               \
case 'q':               \
case 'r':               \
case 's':               \
case 't':               \
case 'u':               \
case 'v':               \
case 'w':               \
case 'x':               \
case 'y':               \
case 'z':

#define JSTR_CASE_UPPER \
case 'A':               \
case 'B':               \
case 'C':               \
case 'D':               \
case 'E':               \
case 'F':               \
case 'G':               \
case 'H':               \
case 'I':               \
case 'J':               \
case 'K':               \
case 'L':               \
case 'M':               \
case 'N':               \
case 'O':               \
case 'P':               \
case 'Q':               \
case 'R':               \
case 'S':               \
case 'T':               \
case 'U':               \
case 'V':               \
case 'W':               \
case 'X':               \
case 'Y':               \
case 'Z':

#define JSTR_CASE_XDIGIT\
JSTR_CASE_DIGIT \
case 'a':                 \
case 'b':                 \
case 'c':                 \
case 'd':                 \
case 'e':                 \
case 'f':                 \
case 'A':                 \
case 'B':                 \
case 'C':                 \
case 'D':                 \
case 'E':                 \
case 'F':

#define JSTR_CASE_BLANK \
case '\t':              \
case ' ':

#define JSTR_CASE_SPACE \
	JSTR_CASE_BLANK \
case '\n':              \
case '\v':              \
case '\f':              \
case '\r':

#define JSTR_CASE_GRAPH \
case 33:                \
case 34:                \
case 35:                \
case 36:                \
case 37:                \
case 38:                \
case 39:                \
case 40:                \
case 41:                \
case 42:                \
case 43:                \
case 44:                \
case 45:                \
case 46:                \
case 47:                \
case 48:                \
case 49:                \
case 50:                \
case 51:                \
case 52:                \
case 53:                \
case 54:                \
case 55:                \
case 56:                \
case 57:                \
case 58:                \
case 59:                \
case 60:                \
case 61:                \
case 62:                \
case 63:                \
case 64:                \
case 65:                \
case 66:                \
case 67:                \
case 68:                \
case 69:                \
case 70:                \
case 71:                \
case 72:                \
case 73:                \
case 74:                \
case 75:                \
case 76:                \
case 77:                \
case 78:                \
case 79:                \
case 80:                \
case 81:                \
case 82:                \
case 83:                \
case 84:                \
case 85:                \
case 86:                \
case 87:                \
case 88:                \
case 89:                \
case 90:                \
case 91:                \
case 92:                \
case 93:                \
case 94:                \
case 95:                \
case 96:                \
case 97:                \
case 98:                \
case 99:                \
case 100:               \
case 101:               \
case 102:               \
case 103:               \
case 104:               \
case 105:               \
case 106:               \
case 107:               \
case 108:               \
case 109:               \
case 110:               \
case 111:               \
case 112:               \
case 113:               \
case 114:               \
case 115:               \
case 116:               \
case 117:               \
case 118:               \
case 119:               \
case 120:               \
case 121:               \
case 122:               \
case 123:               \
case 124:               \
case 125:               \
case 126

#define JSTR_CASE_PRINT \
	JSTR_CASE_GRAPH \
case 32:

#define JSTR_CASE_CNTRL \
case 0:                 \
case 1:                 \
case 2:                 \
case 3:                 \
case 4:                 \
case 5:                 \
case 6:                 \
case 7:                 \
case 8:                 \
case 9:                 \
case 10:                \
case 11:                \
case 12:                \
case 13:                \
case 14:                \
case 15:                \
case 16:                \
case 17:                \
case 18:                \
case 19:                \
case 20:                \
case 21:                \
case 22:                \
case 23:                \
case 24:                \
case 25:                \
case 26:                \
case 27:                \
case 28:                \
case 29:                \
case 30:                \
case 31:                \
case 127:

#define JSTR_CASE_PUNCT \
case '!':               \
case '"':               \
case '#':               \
case '$':               \
case '%':               \
case '&':               \
case '\'':              \
case '(':               \
case ')':               \
case '*':               \
case '+':               \
case ',':               \
case '-':               \
case '.':               \
case '/':               \
                        \
case ':':               \
case ';':               \
case '<':               \
case '=':               \
case '>':               \
case '?':               \
case '@':               \
                        \
case '[':               \
case '\\':              \
case ']':               \
case '^':               \
case '_':               \
case '`':               \
                        \
case '{':               \
case '|':               \
case '}':               \
case '~':

#define JSTR_CASE_VOWEL       \
	JSTR_CASE_VOWEL_UPPER \
	JSTR_CASE_VOWEL_LOWER

#define JSTR_CASE_CONSONANT       \
	JSTR_CASE_CONSONANT_UPPER \
	JSTR_CASE_CONSONANT_LOWER

#define JSTR_CASE_ALPHA \
	JSTR_CASE_LOWER \
	JSTR_CASE_UPPER

#define JSTR_CASE_ALNUM \
	JSTR_CASE_DIGIT \
	JSTR_CASE_ALPHA

#if ((__GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19)) && defined _BSD_SOURCE) \
|| defined _DEFAULT_SOURCE
#	define JSTR_HAVE_STRCASECMP  1
#	define JSTR_HAVE_STRNCASECMP 1
#endif

#ifdef _XOPEN_SOURCE
#	define JSTR_HAVE_MEMCCPY 1
#endif /* Misc || Xopen */

#ifdef _GNU_SOURCE
#	define JSTR_HAVE_MEMMEM	     1
#	define JSTR_HAVE_MEMRCHR	     1
#	define JSTR_HAVE_STRCHRNUL	     1
#	define JSTR_HAVE_FGETS_UNLOCKED     1
#	define JSTR_HAVE_FPUTS_UNLOCKED     1
#	define JSTR_HAVE_GETWC_UNLOCKED     1
#	define JSTR_HAVE_GETWCCHAR_UNLOCKED 1
#	define JSTR_HAVE_FGETWC_UNLOCKED    1
#	define JSTR_HAVE_FPUTWC_UNLOCKED    1
#	define JSTR_HAVE_PUTWCHAR_UNLOCKED  1
#	define JSTR_HAVE_FGETWS_UNLOCKED    1
#	define JSTR_HAVE_FPUTWS_UNLOCKED    1
#	define JSTR_HAVE_WMEMPCPY	     1
#	define JSTR_HAVE_MEMPCPY	     1
#	define JSTR_HAVE_STRCASESTR	     1
#endif /* Gnu */

#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 24) && _POSIX_C_SOURCE >= 199309L) \
|| ((__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19) && defined _SVID_SOURCE || defined _BSD_SOURCE)  \
|| (__GLIBC__ == 2 && __GLIBC_MINOR__ <= 23 && defined _POSIX_C_SOURCE)
#	define JSTR_HAVE_GETC_UNLOCKED	   1
#	define JSTR_HAVE_GETCHAR_UNLOCKED 1
#	define JSTR_HAVE_PUTC_UNLOCKED	   1
#	define JSTR_HAVE_PUTCHAR_UNLOCKED 1
#endif /* Posix || Bsd  */

#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 19) && defined _DEFAULT_SOURCE) \
|| defined _SVID_SOURCE || defined _BSD_SOURCE
#	define JSTR_HAVE_FREAD_UNLOCKED    1
#	define JSTR_HAVE_FWRITE_UNLOCKED   1
#	define JSTR_HAVE_FPUTC_UNLOCKED    1
#	define JSTR_HAVE_FGETC_UNLOCKED    1
#	define JSTR_HAVE_CLEARERR_UNLOCKED 1
#	define JSTR_HAVE_FEOF_UNLOCKED	    1
#	define JSTR_HAVE_FERROR_UNLOCKED   1
#	define JSTR_HAVE_FILENO_UNLOCKED   1
#	define JSTR_HAVE_FFLUSH_UNLOCKED   1
#endif /* Default || Svid || Bsd */

#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 10)) && _POSIX_C_SOURCE >= 200809L \
|| defined _GNU_SOURCE
#	define JSTR_HAVE_STPCPY  1
#	define JSTR_HAVE_STRNLEN 1
#	define JSTR_HAVE_STRNDUP 1
#endif /* Posix || Gnu */

#if _XOPEN_SOURCE >= 500                                                                      \
|| (__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19 && (defined _BSD_SOURCE || defined _SVID_SOURCE)) \
|| (__GLIBC__ > 2 || __GLIBC__ == 2 && __GLIBC_MINOR__ >= 12 && __POSIX_C_SOURCE >= 200809L)
#	define JSTR_HAVE_STRDUP 1
#endif /* Xopen || Bsd || Svid || Posix */

#ifdef _GNU_SOURCE
#	define JSTR_HAVE_STRDUPA  1
#	define JSTR_HAVE_STRNDUPA 1
#endif /* Gnu */

#if defined __linux__ && defined __GLIBC__
#	define JSTR_HAVE_REALLOC_MREMAP 1
#endif /* Gnu */

#define JSTR_RST JSTR_RESTRICT

#include "jstr_config.h"
#include "libc-pointer-arith.h"

#define PJSTR_ALIGN_UP_STR(base)       PJSTR_ALIGN_UP((uintptr_t)base, PJSTR_MALLOC_ALIGNMENT)
#define PJSTR_PTR_IS_ALIGNED_STR(base) PJSTR_PTR_IS_ALIGNED(base, PJSTR_MALLOC_ALIGNMENT)

#if defined __x86_64__ || defined _M_X64
#	define JSTR_ARCH_x86_64 1
#elif defined i386 || defined __i386__ || defined __i386 || defined _M_IX86
#	define JSTR_ARCH_x86_32 1
#elif defined __ARM_ARCH_2__
#	define JSTR_ARCH_ARM2 1
#elif defined __ARM_ARCH_3__ || defined __ARM_ARCH_3M__
#	define JSTR_ARCH_ARM3 1
#elif defined __ARM_ARCH_4T__ || defined __TARGET_ARM_4T
#	define JSTR_ARCH_ARM4T 1
#elif defined __ARM_ARCH_5_ || defined __ARM_ARCH_5E_
#	define JSTR_ARCH_ARM5 1
#elif defined __ARM_ARCH_6T2_ || defined __ARM_ARCH_6T2_
#	define JSTR_ARCH_ARM6T2 1
#elif defined __ARM_ARCH_6__ || defined __ARM_ARCH_6J__ || defined __ARM_ARCH_6K__ || defined __ARM_ARCH_6Z__ || defined __ARM_ARCH_6ZK__
#	define JSTR_ARCH_ARM6 1
#elif defined __ARM_ARCH_7__ || defined __ARM_ARCH_7A__ || defined __ARM_ARCH_7R__ || defined __ARM_ARCH_7M__ || defined __ARM_ARCH_7S__
#	define JSTR_ARCH_ARM7 1
#elif defined __ARM_ARCH_7A__ || defined __ARM_ARCH_7R__ || defined __ARM_ARCH_7M__ || defined __ARM_ARCH_7S__
#	define JSTR_ARCH_ARM7A 1
#elif defined __ARM_ARCH_7R__ || defined __ARM_ARCH_7M__ || defined __ARM_ARCH_7S__
#	define JSTR_ARCH_ARM7R 1
#elif defined __ARM_ARCH_7M__
#	define JSTR_ARCH_ARM7M 1
#elif defined __ARM_ARCH_7S__
#	define JSTR_ARCH_ARM7S 1
#elif defined __aarch64__ || defined _M_ARM64
#	define JSTR_ARCH_ARM64 1
#elif defined mips || defined __mips__ || defined __mips
#	define JSTR_ARCH_MIPS 1
#elif defined __sh__
#	define JSTR_ARCH_SH 1
#elif defined __powerpc || defined __powerpc__ || defined __powerpc64__ || defined __POWERPC__ || defined __ppc__ || defined __PPC__ || defined _ARCH_PPC
#	define JSTR_ARCH_POWERPC 1
#elif defined __PPC64__ || defined __ppc64__ || defined _ARCH_PPC64
#	define JSTR_ARCH_POWERPC64 1
#elif defined _ARCH_PWR4
#	define JSTR_ARCH_POWERPC4 1
#elif defined _ARCH_PWR5X
#	define JSTR_ARCH_POWERPC5X 1
#elif defined _ARCH_PWR6
#	define JSTR_ARCH_POWERPC6 1
#elif defined _ARCH_PWR7
#	define JSTR_ARCH_POWERPC7 1
#elif defined _ARCH_PWR8
#	define JSTR_ARCH_POWERPC8 1
#elif defined _ARCH_PWR9
#	define JSTR_ARCH_POWERPC9 1
#elif defined __sparc__ || defined __sparc
#	define JSTR_ARCH_SPARC 1
#elif defined __m68k__
#	define JSTR_ARCH_M68K 1
#elif defined __alpha__
#	define JSTR_ARCH_ALPHA 1
#elif defined __hppa__
#	define JSTR_ARCH_HPPA 1
#elif defined __riscv_zbb
#	define JSTR_ARCH_RISCV 1
#else
#	define JSTR_ARCH_GENERIC 1
#endif

#define JSTR_FUNC_VOID	      JSTR_NOTHROW JSTR_MAYBE_UNUSED JSTR_NONNULL_ALL
#define JSTR_FUNC	      JSTR_FUNC_VOID JSTR_WARN_UNUSED
#define JSTR_FUNC_NOWARN      JSTR_FUNC_VOID JSTR_WARN_UNUSED
#define JSTR_FUNC_CONST	      JSTR_FUNC JSTR_CONST
#define JSTR_FUNC_PURE	      JSTR_FUNC JSTR_PURE
#define JSTR_FUNC_RET_NONNULL JSTR_FUNC JSTR_RETURNS_NONNULL

#ifdef __cplusplus
#	define PJSTR_BEGIN_DECLS extern "C" {
#	define PJSTR_END_DECLS	 }
#else
#	define PJSTR_BEGIN_DECLS
#	define PJSTR_END_DECLS
#endif

#endif /* JSTR_MACROS_DEF_H */
