#ifndef JSTR_BUILDER_H_DEF
#define JSTR_BUILDER_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#	include <utility>
#endif /* __cpluslus */

#include "_jstr_config.h"
#include "_jstr_macros.h"
#include "_jstr_pp_va_args_macros.h"
#include "_jstr_templates.h"

#ifdef __cplusplus
#	include "_jstr_traits.h"
#endif /* __cpluslus */

JSTR_NOINLINE
JSTR_COLD
static void JSTR_ERR_EXIT()
{
	fprintf(stderr, "%s:%d:%s:Can't malloc:", __FILE__, __LINE__, __func__);
	perror("");
	exit(1);
}

/*
  exit(1) if ptr is NULL.
*/
JSTR_INLINE
static void jstr_err(char *JSTR_RST const p) JSTR_NOEXCEPT
{
	if (unlikely(!p))
		JSTR_ERR_EXIT();
}

typedef struct Jstring {
	size_t size;
	size_t cap;
	char *data;

#ifdef __cplusplus

	JSTR_INLINE
	Jstring(void) JSTR_NOEXCEPT
	{
		this->data = NULL;
	}

#	if JSTR_FREE_ON_DESTRUCTOR_CPP
	JSTR_INLINE
	~Jstring(void) JSTR_NOEXCEPT
	{
		free(this->data);
#		if JSTR_NULLIFY_PTR_ON_DESTRUCTOR_CPP
		this->data = NULL;
#		endif /* JSTR_NULLIFY_PTR_ON_DESTRUCTOR_CPP */
	}

#	endif /* JSTR_FREE_ON_DESTRUCTOR_CPP */

	/*
	  free(STR) and set STR to NULL.
	*/
	JSTR_INLINE
	void del(void) JSTR_NOEXCEPT
	{
		free(this->data);
#	if JSTR_NULLIFY_PTR_ON_DELETE
		this->data = NULL;
#	endif /* JSTR_NULLIFY_PTR_ON_DELETE */
	}

	/*
	  Execute JSTR_ERR().
	*/
	JSTR_INLINE
	void err(void) JSTR_NOEXCEPT
	{
		if (unlikely(!this->data))
			JSTR_ERR_EXIT();
	}

	JSTR_INLINE
	void print(void) JSTR_NOEXCEPT
	{
		fwrite(this->data, 1, this->size, stdout);
	}

	JSTR_INLINE
	void print_stderr(void) JSTR_NOEXCEPT
	{
		fwrite(this->data, 1, this->size, stderr);
	}

	JSTR_INLINE
	void debug_print(void) JSTR_NOEXCEPT
	{
		fprintf(stderr, "size:%zu\ncap:%zu\n", this->size, this->cap);
		fprintf(stderr, "strlen:%zu\n", strlen(this->data));
		fprintf(stderr, "data puts():%s\n", this->data);
		fputs("data:", stderr);
		fwrite(this->data, 1, this->size, stderr);
		fputc('\n', stderr);
	}

#endif /* __cpluslus */

} Jstring;

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc(char **JSTR_RST const s,
		       size_t *JSTR_RST const sz,
		       size_t *JSTR_RST const cap,
		       const size_t _top) JSTR_NOEXCEPT
{
	*sz = 0;
	*cap = (unlikely(_top) < JSTR_MIN_CAP / JSTR_GROWTH_MULTIPLIER)
	       ? JSTR_MIN_CAP
	       : (_top * JSTR_GROWTH_MULTIPLIER);
	*s = (char *)malloc(*cap);
	JSTR_MALLOC_ERR(*s, return);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_allocexact(char **JSTR_RST const s,
			    size_t *JSTR_RST const sz,
			    size_t *JSTR_RST const cap,
			    const size_t _top) JSTR_NOEXCEPT
{
	*sz = 0;
	*s = (char *)malloc(_top);
	JSTR_MALLOC_ERR(*s, return);
	*cap = _top;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_allocexact_append_mem(char **JSTR_RST const s,
				       size_t *JSTR_RST const sz,
				       size_t *JSTR_RST const cap,
				       const char *JSTR_RST const _src,
				       const size_t _srclen) JSTR_NOEXCEPT
{
	jstr_allocexact(s, sz, cap, _srclen + 1);
	if (unlikely(!*s))
		return;
	*sz = _srclen;
	memcpy(*s, _src, _srclen + 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_append_mem(char **JSTR_RST const s,
				  size_t *JSTR_RST const sz,
				  size_t *JSTR_RST const cap,
				  const char *JSTR_RST const _src,
				  const size_t _srclen) JSTR_NOEXCEPT
{
	jstr_alloc(s, sz, cap, _srclen * JSTR_GROWTH_MULTIPLIER);
	if (unlikely(!*s))
		return;
	*sz = _srclen;
	memcpy(*s, _src, _srclen + 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_allocmore_append_mem(char **JSTR_RST const s,
				      size_t *JSTR_RST const sz,
				      size_t *JSTR_RST const cap,
				      const char *JSTR_RST const _src,
				      const size_t _srclen) JSTR_NOEXCEPT
{
	jstr_alloc(s, sz, cap, _srclen * JSTR_GROWTH_MULTIPLIER * 2);
	if (unlikely(!*s))
		return;
	*sz = _srclen;
	memcpy(*s, _src, _srclen + 1);
}

/*
  free(p) and set p to NULL.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_del(char *JSTR_RST p) JSTR_NOEXCEPT
{
	free(p);
#if JSTR_NULLIFY_PTR_ON_DELETE
	p = NULL;
#endif /* JSTR_NULLIFY_PTR_ON_DELETE */
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_debug(const Jstring *JSTR_RST const j)
{
	fprintf(stderr, "size:%zu\ncap:%zu\n", j->size, j->cap);
	fprintf(stderr, "strlen:%zu\n", strlen(j->data));
	fprintf(stderr, "data puts():%s\n", j->data);
	fputs("data:", stderr);
	fwrite(j->data, 1, j->size, stderr);
	fputc('\n', stderr);
}

#ifdef __cplusplus
}
#endif /* __cpluslus */

#ifdef __cplusplus

/*
  Insert multiple strings to S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_alloc_cat_j(Jstring *JSTR_RST const j,
		 Str &&arg,
		 StrArgs &&...args) JSTR_NOEXCEPT
{
	jstr_alloc_cat(&j->data, &j->size, &j->cap, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

/*
  Append multiple strings to end of S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_cat_j(Jstring *JSTR_RST const j,
	   Str &&arg,
	   StrArgs &&...args) JSTR_NOEXCEPT
{
	jstr_cat(&j->data, &j->size, &j->cap, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

#else

#	define jstr_cat_f(s, sz, ...)                                                  \
		do {                                                                    \
			JSTR_ASSERT_IS_STR(*(s));                                       \
			JSTR_ASSERT_IS_SIZE(*(sz));                                     \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                  \
			size_t _ARR_VA_ARGS[JSTR_PP_NARG(__VA_ARGS__)];                 \
			*(sz) += JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			char *p = *(s) + *(sz);                                         \
			JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);           \
			*p = '\0';                                                      \
		} while (0)

#	define jstr_cat(s, sz, cap, ...)                                                                 \
		do {                                                                                      \
			JSTR_ASSERT_IS_STR(*(s));                                                         \
			JSTR_ASSERT_IS_SIZE(*(sz));                                                       \
			JSTR_ASSERT_IS_SIZE(*(cap));                                                      \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                                    \
			size_t _ARR_VA_ARGS[JSTR_PP_NARG(__VA_ARGS__)];                                   \
			const size_t newsz = *sz + JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			if (*(cap) < newsz)                                                               \
				JSTR_REALLOC(*(s), *(cap), newsz + 1, break);                             \
			char *p = *(s) + *(sz);                                                           \
			JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);                             \
			*p = '\0';                                                                        \
			*(sz) = newsz;                                                                    \
		} while (0)

#	define jstr_alloc_cat_f(s, sz, ...)                                           \
		do {                                                                   \
			JSTR_ASSERT_IS_STR(*(s));                                      \
			JSTR_ASSERT_IS_SIZE(*(sz));                                    \
			JSTR_ASSERT_IS_SIZE(*(cap));                                   \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                 \
			size_t _ARR_VA_ARGS[JSTR_PP_NARG(__VA_ARGS__)];                \
			*(sz) = JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			char *p = *(s);                                                \
			JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);          \
			*p = '\0';                                                     \
		} while (0)

#	define jstr_alloc_cat(s, sz, cap, ...)                                        \
		do {                                                                   \
			JSTR_ASSERT_IS_STR(*(s));                                      \
			JSTR_ASSERT_IS_SIZE(*(sz));                                    \
			JSTR_ASSERT_IS_SIZE(*(cap));                                   \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                 \
			size_t _ARR_VA_ARGS[JSTR_PP_NARG(__VA_ARGS__)];                \
			*(sz) = JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			*(cap) = *(sz)*JSTR_GROWTH_MULTIPLIER;                         \
			*(s) = malloc(*(cap));                                         \
			JSTR_MALLOC_ERR(*((s)), break);                                \
			char *p = *(s);                                                \
			JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);          \
			*p = '\0';                                                     \
		} while (0)

#	define jstr_cat_j(j, ...)	 jstr_cat(&((j)->data), &((j)->size), &((j)->cap), __VA_ARGS__)
#	define jstr_alloc_cat_j(j, ...) jstr_alloc_cat(&((j)->data), &((j)->size), &((j)->cap), __VA_ARGS__)

#endif /* __cplusplus */

#endif /* JSTR_BUILDER H_DEF */
