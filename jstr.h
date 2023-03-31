#ifndef JSTR_H_DEF__
#define JSTR_H_DEF__

#ifdef __cplusplus
#	define JSTR_NOEXCEPT__ noexcept
#else
#	define JSTR_NOEXCEPT__
#endif

#define JSTR_ALIGN_POWER_OF_TWO
#define JSTR_64_BIT

#if defined(__GNUC__) || defined(__GLIBC__)
#	ifndef _GNU_SOURCE
#		define _GNU_SOURCE
#	endif
#endif

#ifndef __cplusplus
#	include <stddef.h>
#endif // ! __cplusplus

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#	 include "macros.h"
#	 include <sys/cdefs.h>
#	 include "/home/james/c/pp_macros/pp_va_args_macros.h"
#ifdef __cplusplus
}
#	include <cstdarg>
#	include <cstring>
#	include <cstdlib>
#endif // __cplusplus

#define JSTR_MIN_CAP 8
#define JSTR_MULTIPLIER 2

#ifdef __cplusplus
#	define JSTR_CAST(T) (T)
#else
#	define JSTR_CAST(T)
#endif // __cplusplus

typedef struct jstring_t jstring_t;

#ifdef __cplusplus
void private_jstr_constructor_cap(jstring_t *RESTRICT this_, const std::size_t cap, const char *RESTRICT s, const std::size_t slen) JSTR_NOEXCEPT__;
extern "C" {
#endif // __cplusplus


int private_jstr_cat(jstring_t *RESTRICT this_, const size_t len, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int private_jstr_cat_s(jstring_t *RESTRICT this_, const size_t len, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

void jstr_delete(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__;

void jstr_init(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__;

int jstr_new_alloc(jstring_t *RESTRICT this_, const size_t size) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_new_append(jstring_t *RESTRICT this_, const size_t srclen, const char *RESTRICT const src_, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int private_jstr_new_cat(jstring_t *RESTRICT this_, const size_t arglen, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

void jstr_push_back_noalloc(jstring_t *this_, const char c) JSTR_NOEXCEPT__;
int jstr_push_back_nocheck(jstring_t *this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_push_back(jstring_t *this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_push_back_s(jstring_t *this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

void jstr_push_front_noalloc(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__;
int jstr_push_front_nocheck(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_push_front(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_push_front_s(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

void jstr_pop_back(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__;
void jstr_pop_back_s(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__;

void jstr_pop_front(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__;
void jstr_pop_front_s(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__;

void private_jstr_append_noalloc(jstring_t *dest, const char *RESTRICT src, const size_t srclen) JSTR_NOEXCEPT__;
int private_jstr_append(jstring_t *dest, const char *RESTRICT src, const size_t srclen) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int private_jstr_append_s(jstring_t *dest, const char *RESTRICT src, const size_t srclen) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

void jstr_swap(jstring_t *RESTRICT this_, jstring_t *RESTRICT other_) JSTR_NOEXCEPT__;
void jstr_swap_str(jstring_t *RESTRICT this_, char **RESTRICT other_, size_t *otherlen, size_t *other_cap) JSTR_NOEXCEPT__;

int jstr_shrink_to_fit_nocheck(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_shrink_to_fit(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

int jstr_shrink_to_nocheck(jstring_t *RESTRICT this_, const size_t cap) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_shrink_to(jstring_t *RESTRICT this_, const size_t cap) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

void jstr_shrink_to_size_nocheck(jstring_t *RESTRICT this_, const size_t size) JSTR_NOEXCEPT__;
void jstr_shrink_to_size(jstring_t *RESTRICT this_, const size_t size) JSTR_NOEXCEPT__;
void jstr_shrink_to_size_s(jstring_t *RESTRICT this_, const size_t size) JSTR_NOEXCEPT__;

int jstr_reserve(jstring_t *RESTRICT this_, const size_t cap) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_reserve_s(jstring_t *RESTRICT this_, const size_t cap) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_reserve_nocheck(jstring_t *RESTRICT this_, const size_t cap, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

int private_jstr_replace(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t srclen, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
void jstr_replace_noalloc(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t srclen) JSTR_NOEXCEPT__;
int jstr_replace_nocheck(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t srclen) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

void jstr_replace_jstr_noalloc(jstring_t *RESTRICT dest, const jstring_t *RESTRICT const src) JSTR_NOEXCEPT__;
int jstr_replace_jstr_nocheck(jstring_t *RESTRICT dest, const jstring_t *RESTRICT const src) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_replace_jstr(jstring_t *RESTRICT dest, const jstring_t *RESTRICT const src, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

/* if equals returns 0 */
int jstr_cmp(const jstring_t *RESTRICT const this_, const jstring_t *RESTRICT const other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_cmp_nocheck(const jstring_t *RESTRICT const this_, const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_cmp_str(const jstring_t *RESTRICT const this_, const char *RESTRICT const s, const size_t slen) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

#ifdef __USE_GNU
int jstr_case_cmp_nocheck(const jstring_t *RESTRICT const this_, const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_case_cmp(const jstring_t *RESTRICT const this_, const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

int jstr_case_cmp_str(const jstring_t *RESTRICT const this_, const char *RESTRICT const s) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
#endif

/* finds first occurence of character from end of string */
char *jstr_rchr(const jstring_t *RESTRICT const this_, int c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
/* memchr */
char *jstr_chr(const jstring_t *RESTRICT const this_, int c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

#ifdef __USE_GNU
char *private_jstr_str(jstring_t *haystack, const char *RESTRICT const needle, size_t needlelen, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

#	define jstr_str(this_, ...)                                                                             \
		(PP_NARG(__VA_ARGS__) == 1)                                                                     \
			? private_jstr_str(this_, PP_FIRST_ARG(__VA_ARGS__), strlen(PP_FIRST_ARG(__VA_ARGS__))) \
			: private_jstr_str(this_, __VA_ARGS__, 0)

#else

char *jstr_str(jstring_t *haystack, const char *RESTRICT needle) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

#endif

int jstr_dup(jstring_t *RESTRICT this_, jstring_t *RESTRICT other) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_dup_s(jstring_t *RESTRICT this_, jstring_t *RESTRICT other) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

int jstr_ndup(jstring_t *RESTRICT this_, jstring_t *RESTRICT other, const size_t n) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;
int jstr_ndup_s(jstring_t *RESTRICT this_, jstring_t *RESTRICT other, const size_t n) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

void jstr_rev(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__;

int jstr_rev_dup(jstring_t *RESTRICT src, char **RESTRICT dest) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED;

#ifdef __cplusplus
}
#endif // __cplusplus

typedef struct jstring_t {
#ifndef __cplusplus
	size_t size; 
	size_t capacity;
	char *data;
#else
	std::size_t size;
	std::size_t capacity;
	char *data;
	ALWAYS_INLINE jstring_t() JSTR_NOEXCEPT__
	{
		jstr_init(this);
	}

	ALWAYS_INLINE ~jstring_t() JSTR_NOEXCEPT__
	{
		free(this->data);
		this->data = nullptr;
		this->capacity = 0;
		this->size = 0;
	}

	ALWAYS_INLINE jstring_t(const char *RESTRICT s) JSTR_NOEXCEPT__
	{
		(void)jstr_new_append(this, strlen(s), s);
	}

	template <std::size_t N>
	ALWAYS_INLINE jstring_t(const char (&s)[N]) JSTR_NOEXCEPT__
	{
		(void)jstr_new_append(this, N - 1, s);
	}

	ALWAYS_INLINE jstring_t(const char *RESTRICT s, const std::size_t slen) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_new_append(this, slen, s)))
			return;
		*(this->data + slen) = '\0';
	}

	ALWAYS_INLINE jstring_t(const std::size_t size) JSTR_NOEXCEPT__
	{
		(void)jstr_new_alloc(this, size);
	}

	ALWAYS_INLINE jstring_t(const std::size_t cap, const char *RESTRICT s) JSTR_NOEXCEPT__
	{
		private_jstr_constructor_cap(this, cap, s, strlen(s));
	}

	ALWAYS_INLINE jstring_t(const std::size_t cap, const char *RESTRICT s, const std::size_t slen) JSTR_NOEXCEPT__
	{
		private_jstr_constructor_cap(this, cap, s, slen);
	}

	template <std::size_t N>
	ALWAYS_INLINE jstring_t(const std::size_t cap, const char (&s)[N]) JSTR_NOEXCEPT__
	{
		private_jstr_constructor_cap(this, cap, s, N - 1);
	}

	ALWAYS_INLINE jstring_t(const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__
	{
		(void)jstr_new_append(this, other->size, other->data);
	}

	ALWAYS_INLINE int alloc() JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_new_alloc(this, 8)))
			return 0;
		return 1;
	}

	ALWAYS_INLINE int alloc(const size_t cap) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_new_alloc(this, cap)))
			return 0;
		return 1;
	}

	template <std::size_t N>
	ALWAYS_INLINE int alloc(const char (&s)[N]) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_new_append(this, N - 1, s)))
			return 0;
		return 1;
	}

	ALWAYS_INLINE int alloc(const char *RESTRICT const s, const size_t slen) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_new_append(this, slen, s)))
			return 0;
		return 1;
	}

	ALWAYS_INLINE int alloc(const char *RESTRICT const s) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_new_append(this, strlen(s), s)))
			return 0;
		return 1;
	}
		
	ALWAYS_INLINE CONST char *begin() JSTR_NOEXCEPT__
	{
		return (char *)this->data;
	}

	ALWAYS_INLINE CONST char *end() JSTR_NOEXCEPT__
	{
		return (char *)this->data + this->size;
	}

	ALWAYS_INLINE CONST const char *cbegin() JSTR_NOEXCEPT__
	{
		return (const char *)this->data;
	}

	ALWAYS_INLINE CONST const char *cend() JSTR_NOEXCEPT__
	{
		return (const char *)this->data + this->size;
	}

	ALWAYS_INLINE void pop_back() JSTR_NOEXCEPT__
	{
		jstr_pop_back(this);
	}

	ALWAYS_INLINE void pop_back_s() JSTR_NOEXCEPT__
	{
		jstr_pop_back_s(this);
	}

	ALWAYS_INLINE void pop_front() JSTR_NOEXCEPT__
	{
		jstr_pop_front(this);
	}

	ALWAYS_INLINE void pop_front_s() JSTR_NOEXCEPT__
	{
		jstr_pop_front_s(this);
	}

	ALWAYS_INLINE int push_front(const char c) JSTR_NOEXCEPT__
	{
		return jstr_push_front(this, c);
	}

	ALWAYS_INLINE int push_front_s(const char c) JSTR_NOEXCEPT__
	{
		return jstr_push_front_s(this, c);
	}

	ALWAYS_INLINE int push_back(const char c) JSTR_NOEXCEPT__
	{
		return jstr_push_back(this, c);
	}

	ALWAYS_INLINE int push_back_s(const char c) JSTR_NOEXCEPT__
	{
		return jstr_push_back_s(this, c);
	}

	ALWAYS_INLINE int operator+=(const char *RESTRICT const s) JSTR_NOEXCEPT__
	{
		return this->append(s);
	}

	ALWAYS_INLINE int operator+=(const char c) JSTR_NOEXCEPT__
	{
		return this->push_back(c);
	}

	template <std::size_t N>
	ALWAYS_INLINE int operator+=(const char (&s)[N]) JSTR_NOEXCEPT__
	{
		return private_jstr_append(this, s, N - 1);
	}

	template <std::size_t N>
	ALWAYS_INLINE int operator=(const char (&s)[N]) JSTR_NOEXCEPT__
	{
		return this->replace(s);
	}

	ALWAYS_INLINE int operator=(const char *RESTRICT const s) JSTR_NOEXCEPT__
	{
		return this->replace(s);
	}

	ALWAYS_INLINE int operator=(const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__
	{
		return this->replace(other);
	}
	
	ALWAYS_INLINE int append(const char *RESTRICT const s, const size_t slen) JSTR_NOEXCEPT__
	{
		if (!private_jstr_append(this, s, slen))
			return 0;
		*(this->data + slen) = '\0';
		return 1;
	}

	template <std::size_t N>
	ALWAYS_INLINE int append(const char (&s)[N]) JSTR_NOEXCEPT__
	{
		return private_jstr_append(this, s, N - 1);
	}

	ALWAYS_INLINE int append(const char *RESTRICT const s) JSTR_NOEXCEPT__
	{
		return private_jstr_append(this, s, strlen(s));
	}

	ALWAYS_INLINE int append_s(const char *RESTRICT const s, const size_t slen) JSTR_NOEXCEPT__
	{
		if (!private_jstr_append_s(this, s, slen))
			return 0;
		*(this->data + slen) = '\0';
		return 1;
	}

	template <std::size_t N>
	ALWAYS_INLINE int append_s(const char (&s)[N]) JSTR_NOEXCEPT__
	{
		return private_jstr_append_s(this, s, N - 1);
	}

	ALWAYS_INLINE int append_s(const char *RESTRICT const s) JSTR_NOEXCEPT__
	{
		return private_jstr_append_s(this, s, strlen(s));
	}

	template <std::size_t N>
	ALWAYS_INLINE CONST int str(const char (&s)[N]) JSTR_NOEXCEPT__
	{
		return private_jstr_str(this, s, N - 1);
	}

	ALWAYS_INLINE CONST char *str(const char *RESTRICT const s) JSTR_NOEXCEPT__
	{
		return private_jstr_str(this, s, strlen(s));
	}

	ALWAYS_INLINE CONST char *chr(const int c) JSTR_NOEXCEPT__
	{
		return jstr_chr(this, c);
	}

	ALWAYS_INLINE CONST char *rchr(const int c) JSTR_NOEXCEPT__
	{
		return jstr_rchr(this, c);
	}

	ALWAYS_INLINE void swap(jstring_t *other) JSTR_NOEXCEPT__
	{
		jstr_swap(this, other);
	}

	ALWAYS_INLINE int reserve(const std::size_t cap) JSTR_NOEXCEPT__
	{
		return jstr_reserve(this, cap);
	}

	ALWAYS_INLINE int reserve_add(const std::size_t add_cap) JSTR_NOEXCEPT__
	{
		return jstr_reserve(this, this->capacity + add_cap);
	}

	ALWAYS_INLINE int replace(const char *RESTRICT const s) JSTR_NOEXCEPT__
	{
		return private_jstr_replace(this, s, strlen(s));
	}

	ALWAYS_INLINE int replace(const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__
	{
		return jstr_replace_jstr(this, other);
	}

	template <std::size_t N>
	ALWAYS_INLINE int replace(const char (&s)[N]) JSTR_NOEXCEPT__
	{
		return private_jstr_replace(this, s, N - 1);
	}

	ALWAYS_INLINE int shrink_to_fit() JSTR_NOEXCEPT__
	{
		return jstr_shrink_to_fit(this);
	}

	ALWAYS_INLINE CONST int cmp(const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__
	{
		return jstr_cmp_nocheck(this, other);
	}

	template <std::size_t N>
	ALWAYS_INLINE CONST int cmp(const char (&s)[N]) JSTR_NOEXCEPT__
	{
		return jstr_cmp_str(this, s, N - 1);
	}

#	ifdef __USE_GNU

	ALWAYS_INLINE CONST int casecmp(const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__
	{
		return jstr_case_cmp_str(this, other->data);
	}

	ALWAYS_INLINE CONST int casecmp(const char *RESTRICT const s) JSTR_NOEXCEPT__
	{
		return jstr_case_cmp_str(this, s);
	}

#	endif // __USE_GNU

#endif // __cplusplus
} jstring_t;

#ifdef JSTR_HAS_GENERIC
#	define jstr_replace(dest, ...) _Generic((PP_FIRST_ARG(__VA_ARGS__)),                                                        \
		jstring_t *: jstr_replace_jstr(dest, (jstring_t *)PP_FIRST_ARG(__VA_ARGS__)),                                       \
		JSTR_GENERIC_CASE_STR((PP_NARG(__VA_ARGS__) == 2)                                                                   \
			? private_jstr_replace(dest, (char *)__VA_ARGS__, 0)                                                        \
			: private_jstr_replace(dest, (char *)PP_FIRST_ARG(__VA_ARGS__), strlen((char *)PP_FIRST_ARG(__VA_ARGS__)))) \
	)
#else
#	define jstr_replace(dest, src, srclen)          \
		private_jstr_replace(dest, src, srclen)
#endif // JSTR_HAS_GENERIC

#ifdef JSTR_HAS_GENERIC
#	define jstr_cat(this_jstr, ...)                                                          \
		generic_jstr_cat(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)

#	define generic_jstr_cat(this_jstr, len, arg1, ...) _Generic((PP_FIRST_ARG(__VA_ARGS__)), \
		void *: jstr_append(this_jstr, arg1, len),                                       \
		JSTR_GENERIC_CASE_STR(private_jstr_cat(this_jstr, len, arg1, __VA_ARGS__))       \
	)

#	define jstr_cat_s(this_jstr, ...)                                                          \
		generic_jstr_cat_s(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)

#	define generic_jstr_cat_s(this_jstr, len, arg1, ...) _Generic((PP_FIRST_ARG(__VA_ARGS__)), \
		void *: jstr_append_s(this_jstr, arg1, len),                                       \
		JSTR_GENERIC_CASE_STR(private_jstr_cat_s(this_jstr, len, arg1, __VA_ARGS__))       \
	)
#else
#	define jstr_cat(this_jstr, ...)                                                                                           \
		(PP_NARG(__VA_ARGS__) > 1)                                                                                        \
			? private_jstr_cat(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)                          \
			: private_jstr_append(this_jstr, PP_FIRST_ARG(__VA_ARGS__), PP_STRLEN_VA_ARGS(PP_FIRST_ARG(__VA_ARGS__)))

#	define jstr_cat_s(this_jstr, ...)                                                                                           \
		(PP_NARG(__VA_ARGS__) > 1)                                                                                          \
			? private_jstr_cat_s(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)                          \
			: private_jstr_append_s(this_jstr, PP_FIRST_ARG(__VA_ARGS__), PP_STRLEN_VA_ARGS(PP_FIRST_ARG(__VA_ARGS__)))
#endif // JSTR_HAS_GENERIC

#define jstr_new_cat(this_jstr, ...) private_jstr_new_cat(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)

#define PRIVATE_JSTR_NEW_FIRST_INT(this_jstr, ...)                         \
	(PP_NARG(__VA_ARGS__) == 1)                                        \
		? PRIVATE_JSTR_NEW_ALLOC(this_jstr, __VA_ARGS__)           \
		: PRIVATE_JSTR_NEW_APPEND_WITH_LEN(this_jstr, __VA_ARGS__)

#define PRIVATE_JSTR_NEW_ALLOC(this_jstr, ...)                       \
	jstr_new_alloc(this_jstr, (size_t)PP_FIRST_ARG(__VA_ARGS__))

#define PRIVATE_JSTR_NEW_APPEND(this_jstr, ...)                                                                  \
	jstr_new_append(this_jstr, strlen((char *)PP_FIRST_ARG(__VA_ARGS__)), (char *)PP_FIRST_ARG(__VA_ARGS__))

#define PRIVATE_JSTR_NEW_APPEND_WITH_LEN(this_jstr, ...)      \
	jstr_new_append(this_jstr, (size_t)__VA_ARGS__, NULL)

#define PRIVATE_JSTR_NEW_CAT(this_jstr, ...)                                                       \
	private_jstr_new_cat(this_jstr, PP_STRLEN_VA_ARGS((char *)__VA_ARGS__), __VA_ARGS__, NULL)

#define PRIVATE_JSTR_NEW_ADD_STR(this_jstr, ...)                  \
(                                                                 \
	(PP_NARG(__VA_ARGS__) == 1)                               \
		? PRIVATE_JSTR_NEW_APPEND(this_jstr, __VA_ARGS__) \
		: PRIVATE_JSTR_NEW_CAT(this_jstr, __VA_ARGS__)    \
)

#ifdef JSTR_HAS_GENERIC
#	define jstr_new(this_jstr, ...) _Generic((PP_FIRST_ARG(__VA_ARGS__)),               \
		JSTR_GENERIC_CASE_SIZE(PRIVATE_JSTR_NEW_FIRST_INT(this_jstr, __VA_ARGS__)), \
		JSTR_GENERIC_CASE_STR(PRIVATE_JSTR_NEW_ADD_STR(this_jstr, __VA_ARGS__))     \
	)
#else
#	define jstr_new(this_jstr, size) jstr_new_alloc(this_jstr, size)
#endif // JSTR_HAS_GENERIC

#ifdef JSTR_HAS_GENERIC
#	define jstr_add(this_jstr, ...) _Generic((PP_FIRST_ARG(__VA_ARGS__)),                                                         \
		JSTR_GENERIC_CASE_SIZE(jstr_reserve_nocheck(this_jstr, ((this_jstr)->capacity) + (size_t)PP_FIRST_ARG(__VA_ARGS__))), \
		JSTR_GENERIC_CASE_STR(jstr_append(this_jstr, (char *)__VA_ARGS__))                                                    \
	)
#endif // JSTR_HAS_GENERIC

#define jstr_append(this_jstr, ...)                                                                            \
(                                                                                                              \
	PP_NARG(__VA_ARGS__) == 2                                                                              \
		? private_jstr_append(this_jstr, __VA_ARGS__, 0)                                               \
		: private_jstr_append(this_jstr, PP_FIRST_ARG(__VA_ARGS__), strlen(PP_FIRST_ARG(__VA_ARGS__))) \
)

#define private_jstr_reserve_x(this_jstr, multiplier)                                   \
	jstr_reserve_nocheck_exact(this_jstr, ((multiplier) * ((this_jstr)->capacity)))

#define jstr_reserve_2x(this_jstr) private_jstr_reserve_x(this_jstr, 2)
#define jstr_reserve_4x(this_jstr) private_jstr_reserve_x(this_jstr, 4)
#define jstr_reserve_8x(this_jstr) private_jstr_reserve_x(this_jstr, 8)
#define jstr_reserve_16x(this_jstr) private_jstr_reserve_x(this_jstr, 16)
#define jstr_reserve_32x(this_jstr) private_jstr_reserve_x(this_jstr, 32)
#define jstr_reserve_64x(this_jstr) private_jstr_reserve_x(this_jstr, 64)

#define private_jstr_reserve_s_x(this_jstr, multiplier)                                   \
	jstr_reserve_s_nocheck_exact(this_jstr, ((multiplier) * ((this_jstr)->capacity)))

#define jstr_reserve_s_2x(this_jstr) private_jstr_reserve_s_x(this_jstr, 2)
#define jstr_reserve_s_4x(this_jstr) private_jstr_reserve_s_x(this_jstr, 4)
#define jstr_reserve_s_8x(this_jstr) private_jstr_reserve_s_x(this_jstr, 8)
#define jstr_reserve_s_16x(this_jstr) private_jstr_reserve_s_x(this_jstr, 16)
#define jstr_reserve_s_32x(this_jstr) private_jstr_reserve_s_x(this_jstr, 32)
#define jstr_reserve_s_64x(this_jstr) private_jstr_reserve_s_x(this_jstr, 64)

#ifndef __cplusplus
#	define jstr_foreach(elem, jstr)                          \
		for (char *elem = ((jstr)->data); *elem; ++elem)
#	define jstr_begin(this_jstr) ((this_jstr)->data)
#	define jstr_end(this_jstr) (((this_jstr)->data) + ((this_jstr)->size))

#	define jstr_cbegin(this_jstr) ((const char *)((this_jstr)->data))
#	define jstr_cend(this_jstr) ((const char *)(((this_jstr)->data) + ((this_jstr)->size)))
#endif // __cplusplus

#define jstr_typeof_data char *

#define jstr_foreach_index(elem, jstr)                        \
	for (size_t i = 0, end__ = jstr.size; i < end__; ++i)

#endif // JSTR_H_DEF__
