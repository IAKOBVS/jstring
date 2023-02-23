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

#define jstrNew(JSTR, CONST_STRING) \
	do { \
		JSTR.len = strlen(CONST_STRING); \
		JSTR.size = MAX(2 * JSTR.len, JSTR_MIN_SIZE); \
		if (unlikely(!(JSTR.data = malloc(JSTR.size)))) { \
			perror(""); \
		} \
		memcpy(JSTR.data, CONST_STRING, JSTR.len); \
	} while (0)

#define jstrInit(JSTR) \
	Jstr JSTR = { \
		.size = 0, \
		.len = 0 \
	}

#define jstrDelete(JSTR) \
	do { \
		if (JSTR.size) \
			free(JSTR.data); \
		JSTR.size = 0; \
	} while (0)
#define jstrDeletePtr(JSTR) \
	do { \
		if (JSTR->size) \
			free(JSTR->str); \
		JSTR->size = 0; \
	} while (0)

#define jstrPr(JSTR) printf("string: %s: \nsize is %zu\nlen is %zu", JSTR.data, JSTR.size, JSTR.len)

#define jstrShrink(JARR) \
	do { \
		if (unlikely(!(JARR.data = realloc(JARR.data, JARR.len + 1)))) { \
			perror(CURR_FUNC); \
		} \
		JARR.size = JARR.len; \
	} while (0)

#define jstrReserve(JARR, JSTR_ALLOC_SIZE) \
	do { \
		if (unlikely(!(JARR.data = malloc(JSTR_ALLOC_SIZE)))) { \
			perror(CURR_FUNC); \
		} \
		JARR.size = JSTR_ALLOC_SIZE; \
	} while (0)

#define jstrResize(JSTR_ALLOC_SIZE) \
	do { \
		if (unlikely(!(JARR.data = realloc(JSTR_ALLOC_SIZE)))) { \
			perror(CURR_FUNC); \
		} \
		if (JSTR_ALLOC_SIZE < JARR.len) JARR.len = JSTR_ALLOC_SIZE; \
		JARR.size = JSTR_ALLOC_SIZE; \
	} while (0)

typedef struct Jstr {
	char *data;
	size_t size;
	size_t len;
} Jstr;

int private_jstrCat(Jstr *RESTRICT dest, ...);
#define jstrCat(JSTR, ...) private_jstrCat(dest, __VA_ARGS__, NULL)

int jstrPush(Jstr *dest, const char c);
int jstrPushStr(Jstr *dest, const char *RESTRICT src, const size_t srcLen);
int jstrRev(Jstr *RESTRICT dest);

#undef ALWAYS_INLINE
#undef RESTRICT

#endif
