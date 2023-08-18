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
#	include "_jtraits.h"
#endif /* __cpluslus */

#define PRIVATE_JSTR_MIN_ALLOC(new_cap)                   \
	((new_cap < JSTR_MIN_CAP / JSTR_ALLOC_MULTIPLIER) \
	 ? (JSTR_MIN_CAP)                                 \
	 : (new_cap * JSTR_ALLOC_MULTIPLIER))

#define PRIVATE_JSTR_MIN_ALLOCEXACT(new_cap) \
	((new_cap < JSTR_MIN_CAP)            \
	 ? (JSTR_MIN_CAP)                    \
	 : (new_cap))

#define PRIVATE_JSTR_ALLOC_ONLY(p, _cap, new_cap, do_fail) \
	(_cap) = PRIVATE_JSTR_MIN_ALLOC(new_cap);          \
	(p) = (char *)malloc((_cap));                      \
	JSTR_MALLOC_ERR((p), do_fail);

/*
  exit(1) if ptr is NULL.
*/
JSTR_INLINE
static void
jstr_err(char *JSTR_RST const p) JSTR_NOEXCEPT
{
	if (jstr_unlikely(p == NULL))
		JSTR_ERR_EXIT();
}

typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t _cap;

#ifdef __cplusplus

	JSTR_INLINE
	jstr_ty(void) JSTR_NOEXCEPT
	{
		this->data = NULL;
	}

#	if JSTR_FREE_ON_DESTRUCTOR_CPP
	JSTR_INLINE
	~jstr_ty(void) JSTR_NOEXCEPT
	{
		::free(this->data);
#		if JSTR_NULLIFY_PTR_ON_DESTRUCTOR_CPP
		this->data = NULL;
#		endif /* JSTR_NULLIFY_PTR_ON_DESTRUCTOR_CPP */
	}

#	endif /* JSTR_FREE_ON_DESTRUCTOR_CPP */

	/*
	  free(STR) and set STR to NULL.
	*/
	JSTR_INLINE
	void
	free(void) JSTR_NOEXCEPT
	{
		::free(this->data);
#	if JSTR_NULLIFY_PTR_ON_DELETE
		this->data = NULL;
#	endif /* JSTR_NULLIFY_PTR_ON_DELETE */
	}

	/*
	  exit(1) if ptr is NULL.
	*/
	JSTR_INLINE
	void
	err_exit(void) JSTR_NOEXCEPT
	{
		if (jstr_unlikely(this->data == NULL))
			JSTR_ERR_EXIT();
	}

	JSTR_INLINE
	void
	print(void) JSTR_NOEXCEPT
	{
		fwrite(this->data, 1, this->size, stdout);
	}

	JSTR_INLINE
	void
	print_stderr(void) JSTR_NOEXCEPT
	{
		fwrite(this->data, 1, this->size, stderr);
	}

	JSTR_INLINE
	void
	debug_print(void) JSTR_NOEXCEPT
	{
		fprintf(stderr, "size:%zu\ncap:%zu\n", this->size, this->_cap);
		fprintf(stderr, "strlen:%zu\n", strlen(this->data));
		fprintf(stderr, "data puts():%s\n", this->data);
		fputs("data:", stderr);
		fwrite(this->data, 1, this->size, stderr);
		fputc('\n', stderr);
	}

#endif /* __cpluslus */

} jstr_ty;

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_alloc(char **JSTR_RST const _s,
	   size_t *JSTR_RST const _sz,
	   size_t *JSTR_RST const _cap,
	   const size_t _top) JSTR_NOEXCEPT
{
	*_sz = 0;
	*_cap = PRIVATE_JSTR_MIN_ALLOC(_top);
	*_s = (char *)malloc(*_cap);
	JSTR_MALLOC_ERR(*_s, return);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_allocexact(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const size_t _top) JSTR_NOEXCEPT
{
	*_sz = 0;
	*_s = (char *)malloc(_top);
	JSTR_MALLOC_ERR(*_s, return);
	*_cap = _top;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_allocexact_append_mem(char **JSTR_RST const _s,
			   size_t *JSTR_RST const _sz,
			   size_t *JSTR_RST const _cap,
			   const char *JSTR_RST const _src,
			   const size_t _srclen) JSTR_NOEXCEPT
{
	jstr_allocexact(_s, _sz, _cap, _srclen + 1);
	if (jstr_unlikely(*_s == NULL))
		return;
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen + 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_alloc_append_mem(char **JSTR_RST const _s,
		      size_t *JSTR_RST const _sz,
		      size_t *JSTR_RST const _cap,
		      const char *JSTR_RST const _src,
		      const size_t _srclen) JSTR_NOEXCEPT
{
	PRIVATE_JSTR_ALLOC_ONLY(*_s, *_cap, _srclen, return);
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen + 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_allocmore_append_mem(char **JSTR_RST const _s,
			  size_t *JSTR_RST const _sz,
			  size_t *JSTR_RST const _cap,
			  const char *JSTR_RST const _src,
			  const size_t _srclen) JSTR_NOEXCEPT
{
	PRIVATE_JSTR_ALLOC_ONLY(*_s, *_cap, _srclen * 2, return);
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen + 1);
}

/*
  free(p) and set p to NULL.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_free(char *JSTR_RST p) JSTR_NOEXCEPT
{
	free(p);
#if JSTR_NULLIFY_PTR_ON_DELETE
	p = NULL;
#endif /* JSTR_NULLIFY_PTR_ON_DELETE */
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_debug(const jstr_ty *JSTR_RST const j)
{
	fprintf(stderr, "size:%zu\ncap:%zu\n", j->size, j->_cap);
	fprintf(stderr, "strlen:%zu\n", strlen(j->data));
	fprintf(stderr, "data puts:%s\n", j->data);
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
jstr_alloc_cat_j(jstr_ty *JSTR_RST const j,
		 Str &&arg,
		 StrArgs &&...args) JSTR_NOEXCEPT
{
	jstr_alloc_cat(&j->data, &j->size, &j->_cap, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

/*
  Append multiple strings to end of S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_cat_j(jstr_ty *JSTR_RST const j,
	   Str &&arg,
	   StrArgs &&...args) JSTR_NOEXCEPT
{
	jstr_cat(&j->data, &j->size, &j->_cap, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

#else

#	define jstr_cat_f(_s, _sz, ...)                                                 \
		do {                                                                     \
			JSTR_ASSERT_IS_STR(*(_s));                                       \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                     \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                   \
			size_t _ARR_VA_ARGS[JSTR_PP_NARG(__VA_ARGS__)];                  \
			*(_sz) += JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			char *p = *(_s) + *(_sz);                                        \
			JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);            \
			*p = '\0';                                                       \
		} while (0)

#	define jstr_cat(_s, _sz, _cap, ...)                                                                \
		do {                                                                                        \
			JSTR_ASSERT_IS_STR(*(_s));                                                          \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                                        \
			JSTR_ASSERT_IS_SIZE(*(_cap));                                                       \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                                      \
			size_t _ARR_VA_ARGS[JSTR_PP_NARG(__VA_ARGS__)];                                     \
			const size_t _newsz = *_sz + JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			if (*(_cap) < newsz)                                                                \
				JSTR_REALLOC(*(_s), *(_cap), newsz + 1, break);                             \
			char *p = *(_s) + *(_sz);                                                           \
			JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);                               \
			*p = '\0';                                                                          \
			*(_sz) = newsz;                                                                     \
		} while (0)

#	define jstr_alloc_cat_f(_s, _sz, ...)                                          \
		do {                                                                    \
			JSTR_ASSERT_IS_STR(*(_s));                                      \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                    \
			JSTR_ASSERT_IS_SIZE(*(_cap));                                   \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                  \
			size_t _ARR_VA_ARGS[JSTR_PP_NARG(__VA_ARGS__)];                 \
			*(_sz) = JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			char *p = *(_s);                                                \
			JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);           \
			*p = '\0';                                                      \
		} while (0)

#	define jstr_alloc_cat(_s, _sz, _cap, ...)                                      \
		do {                                                                    \
			JSTR_ASSERT_IS_STR(*(_s));                                      \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                    \
			JSTR_ASSERT_IS_SIZE(*(_cap));                                   \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                  \
			size_t _ARR_VA_ARGS[JSTR_PP_NARG(__VA_ARGS__)];                 \
			*(_sz) = JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			*(_cap) = PRIVATE_JSTR_MIN_ALLOC(*(_sz));                       \
			*(_s) = malloc(*(_cap));                                        \
			JSTR_MALLOC_ERR(*((_s)), break);                                \
			char *p = *(_s);                                                \
			JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);           \
			*p = '\0';                                                      \
		} while (0)

#	define jstr_cat_j(j, ...)	 jstr_cat(&((j)->data), &((j)->size), &((j)->_cap), __VA_ARGS__)
#	define jstr_alloc_cat_j(j, ...) jstr_alloc_cat(&((j)->data), &((j)->size), &((j)->_cap), __VA_ARGS__)

#endif /* __cplusplus */

#endif /* JSTR_BUILDER H_DEF */
