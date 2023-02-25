#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#include "/home/james/c/vargc.h"

#include <stddef.h>

#if defined(__PRETTY_FUNCTION__)
	#define CURR_FUNC __PRETTY_FUNCTION__
#elif defined(__FUNCTION__)
	#define CURR_FUNC __FUNCTION__
#elif defined(__func__)
	#define CURR_FUNC __func__
#else
	#define CURR_FUNC ""
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define ALWAYS_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
    #define ALWAYS_INLINE __forceinline inline
#else
    #define ALWAYS_INLINE inline
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define RESTRICT restrict
#elif defined(__GNUC__) || defined(__clang__)
# define RESTRICT __restrict__
#elif defined(_MSC_VER)
# define RESTRICT __restrict
#else
# define RESTRICT
#endif

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__clang__) && __has_builtin(__builtin_expect))
  #define likely(x) __builtin_expect(!!(x), 1)
  #define unlikely(x) __builtin_expect(!!(x), 0)
#else
  #define likely(x) (x)
  #define unlikely(x) (x)
#endif

#define JSTR_MIN_SIZE 8
#define MAX(a,b) ((a)>(b)?(a):(b))

/* #define jstrNew(JSTR, CONST_STRING) \ */
/* 	do { \ */
/* 		JSTR.len = strlen(CONST_STRING); \ */
/* 		JSTR.size = MAX(2 * JSTR.len, JSTR_MIN_SIZE); \ */
/* 		if (unlikely(!(JSTR.data = malloc(JSTR.size)))) { \ */
/* 			perror(""); \ */
/* 		} \ */
/* 		memcpy(JSTR.data, CONST_STRING, JSTR.len); \ */
/* 	} while (0) */

#define jstrPr(JSTR) printf("string: %s: \nsize is %zu\nlen is %zu", JSTR.data, JSTR.size, JSTR.len)

typedef struct Jstr {
	char *data;
	size_t size;
	size_t len;
} Jstr;

int private_jstrCat(Jstr *RESTRICT dest, ...);
#define jstrCat(JSTR, ...) private_jstrCat(JSTR, __VA_ARGS__, NULL)

void jstrInit(Jstr *RESTRICT dest);
void jstrDeleteFast(Jstr *RESTRICT dest);
void jstrDelete(Jstr *RESTRICT dest);
int jstrNew(Jstr *RESTRICT dest, const char *RESTRICT src, const size_t srcLen);
int jstrPushback(Jstr *dest, const char c);
void jstrPopback(Jstr *RESTRICT dest);
int jstrAppend(Jstr *dest, const char *RESTRICT src, const size_t srcLen);
/* swaps dest with src and updates the Jstr struct members */
void jstrSwap(Jstr *RESTRICT dest, Jstr *RESTRICT src);
void jstrSwapStr(Jstr *RESTRICT dest, char **RESTRICT src, size_t *srcLen, size_t *srcSize);
int jstrShrink(Jstr *RESTRICT dest);
int jstrReserve(Jstr *RESTRICT dest, size_t size);
/* replaces dest->data with dest and reallocs if needed */
int jstrReplace(Jstr *RESTRICT dest, char *RESTRICT src, const size_t srcLen);
/* compares two Jstr, and if equal, returns 0 */
int jstrCmp(Jstr *RESTRICT dest, Jstr *RESTRICT src);

#undef ALWAYS_INLINE
#undef RESTRICT

#endif
