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

#ifndef __cplusplus
#	include "pp_va_args_macros.h"
#endif /* !__cpluslus */

#include "config.h"
#include "macros.h"
#include "templates.h"

#ifdef __cplusplus
#	include "jtraits.h"
#endif /* __cpluslus */

#define PRIV_JSTR_MIN_ALLOC(new_cap)                              \
	((new_cap < JSTR_CFG_MIN_CAP / JSTR_CFG_ALLOC_MULTIPLIER) \
	 ? (JSTR_CFG_MIN_CAP)                                     \
	 : (new_cap * JSTR_CFG_ALLOC_MULTIPLIER))
#define PRIV_JSTR_MIN_ALLOCEXACT(new_cap) \
	((new_cap < JSTR_CFG_MIN_CAP)     \
	 ? (JSTR_CFG_MIN_CAP)             \
	 : (new_cap))
#define PRIV_JSTR_ALLOC_ONLY(p, _cap, new_cap, do_fail) \
	do {                                            \
		(_cap) = PRIV_JSTR_MIN_ALLOC(new_cap);  \
		(p) = (char *)malloc((_cap));           \
		PRIV_JSTR_MALLOC_ERR((p), do_fail);     \
	} while (0)
/*
  exit(1) if ptr is NULL.
*/
JSTR_INLINE
static void
jstr_err(char *JSTR_RST const p) JSTR_NOEXCEPT
{
	if (jstr_unlikely(p == NULL))
		priv_jstr_err_exit();
}

typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t cap;
#ifdef __cplusplus
	JSTR_INLINE
	jstr_ty(void) JSTR_NOEXCEPT
	{
		this->data = NULL;
	}
#	if JSTR_CFG_FREE_ON_DESTRUCTOR_CPP
	JSTR_INLINE
	~jstr_ty(void) JSTR_NOEXCEPT
	{
		::free(this->data);
#		if JSTR_CFG_NULLIFY_PTR_ON_DESTRUCTOR_CPP
		this->data = NULL;
#		endif /* JSTR_CFG_NULLIFY_PTR_ON_DESTRUCTOR_CPP */
	}
#	endif /* JSTR_CFG_FREE_ON_DESTRUCTOR_CPP */
	/*
	  free(STR) and set STR to NULL.
	*/
	JSTR_INLINE
	void
	free(void) JSTR_NOEXCEPT
	{
		::free(this->data);
#	if JSTR_CFG_NULLIFY_PTR_ON_DELETE
		this->data = NULL;
#	endif /* JSTR_CFG_NULLIFY_PTR_ON_DELETE */
	}
	/*
	  exit(1) if ptr is NULL.
	*/
	JSTR_INLINE
	void
	err_exit(void) JSTR_NOEXCEPT
	{
		if (jstr_unlikely(this->data == NULL))
			priv_jstr_err_exit();
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
		fprintf(stderr, "size:%zu\ncap:%zu\n", this->size, this->cap);
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
	*_cap = PRIV_JSTR_MIN_ALLOC(_top);
	*_s = (char *)malloc(*_cap);
	PRIV_JSTR_MALLOC_ERR(*_s, return);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_alloc_j(jstr_ty *JSTR_RST const j,
	   const size_t _top) JSTR_NOEXCEPT
{
	jstr_alloc(&j->data, &j->size, &j->cap, _top);
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
	PRIV_JSTR_MALLOC_ERR(*_s, return);
	*_cap = _top;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_allocexact_j(jstr_ty *JSTR_RST const j,
		const size_t _top) JSTR_NOEXCEPT
{
	jstr_allocexact(&j->data, &j->size, &j->cap, _top);
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
JSTR_MAYBE_UNUSED
static void
jstr_allocexact_append_mem_j(jstr_ty *JSTR_RST const j,
			   const char *JSTR_RST const _src,
			   const size_t _srclen) JSTR_NOEXCEPT
{
	jstr_allocexact_append_mem(&j->data, &j->size, &j->cap, _src, _srclen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_allocexact_append(char **JSTR_RST const _s,
			   size_t *JSTR_RST const _sz,
			   size_t *JSTR_RST const _cap,
			   const char *JSTR_RST const _src) JSTR_NOEXCEPT
{
	return jstr_allocexact_append_mem(_s, _sz, _cap, _src, strlen(_src));
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_allocexact_append_j(jstr_ty *JSTR_RST const j,
			   const char *JSTR_RST const _src) JSTR_NOEXCEPT
{
	jstr_allocexact_append(&j->data, &j->size, &j->cap, _src);
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
	PRIV_JSTR_ALLOC_ONLY(*_s, *_cap, _srclen, return);
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen + 1);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_alloc_append_mem_j(jstr_ty *JSTR_RST const j,
		      const char *JSTR_RST const _src,
		      const size_t _srclen) JSTR_NOEXCEPT
{
	jstr_alloc_append_mem(&j->data, &j->size, &j->cap, _src, _srclen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_alloc_append(char **JSTR_RST const _s,
		      size_t *JSTR_RST const _sz,
		      size_t *JSTR_RST const _cap,
		      const char *JSTR_RST const _src) JSTR_NOEXCEPT
{
	return jstr_alloc_append_mem(_s, _sz, _cap, _src, strlen(_src));
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_alloc_append_j(jstr_ty *JSTR_RST const j,
		      const char *JSTR_RST const _src) JSTR_NOEXCEPT
{
	jstr_alloc_append(&j->data, &j->size, &j->cap, _src);
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
	PRIV_JSTR_ALLOC_ONLY(*_s, *_cap, _srclen * 2, return);
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen + 1);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_allocmore_append_mem_j(jstr_ty *JSTR_RST const j,
			  const char *JSTR_RST const _src,
			  const size_t _srclen) JSTR_NOEXCEPT
{
	jstr_allocmore_append_mem(&j->data, &j->size, &j->cap, _src, _srclen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_allocmore_append(char **JSTR_RST const _s,
			  size_t *JSTR_RST const _sz,
			  size_t *JSTR_RST const _cap,
			  const char *JSTR_RST const _src) JSTR_NOEXCEPT
{
	return jstr_allocmore_append_mem(_s, _sz, _cap, _src, strlen(_src));
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_allocmore_append_j(jstr_ty *JSTR_RST const j,
			  const char *JSTR_RST const _src) JSTR_NOEXCEPT
{
	jstr_allocmore_append(&j->data, &j->size, &j->cap, _src);
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
#if JSTR_CFG_NULLIFY_PTR_ON_DELETE
	p = NULL;
#endif /* JSTR_CFG_NULLIFY_PTR_ON_DELETE */
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_debug(const jstr_ty *JSTR_RST const _j)
{
	fprintf(stderr, "size:%zu\ncap:%zu\n", _j->size, _j->cap);
	fprintf(stderr, "strlen:%zu\n", strlen(_j->data));
	fprintf(stderr, "data puts:%s\n", _j->data);
	fputs("data:", stderr);
	fwrite(_j->data, 1, _j->size, stderr);
	fputc('\n', stderr);
}

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
   Return value:
   New _len of S.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *
jstr_append_mem_p_f(char *JSTR_RST const _s,
		    const char *JSTR_RST const _src,
		    const size_t _sz,
		    const size_t _srclen) JSTR_NOEXCEPT
{
	memcpy(_s, _src, _srclen + 1);
	return _s + _sz + _srclen;
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_append_mem(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const char *JSTR_RST const _src,
		const size_t _srclen) JSTR_NOEXCEPT
{
	if (*_cap < *_sz + _srclen)
		PRIV_JSTR_REALLOC(*_s, *_cap, *_sz + _srclen, return);
	*_sz = jstr_append_mem_p_f(*_s, _src, *_sz, _srclen) - *_s;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_append_mem_j(jstr_ty *JSTR_RST const j,
		const char *JSTR_RST const _src,
		const size_t _srclen) JSTR_NOEXCEPT
{
	jstr_append_mem(&j->data, &j->size, &j->cap, _src, _srclen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_append(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const char *JSTR_RST const _src) JSTR_NOEXCEPT
{
	return jstr_append_mem(_s, _sz, _cap, _src, strlen(_src));
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_append_j(jstr_ty *JSTR_RST const j,
		const char *JSTR_RST const _src) JSTR_NOEXCEPT
{
	jstr_append(&j->data, &j->size, &j->cap, _src);
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
jstr_alloc_cat_j(jstr_ty *JSTR_RST const _j,
		 Str &&arg,
		 StrArgs &&...args) JSTR_NOEXCEPT
{
	jstr_alloc_cat(&_j->data, &_j->size, &_j->cap, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

/*
  Append multiple strings to end of S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_cat_j(jstr_ty *JSTR_RST const _j,
	   Str &&arg,
	   StrArgs &&...args) JSTR_NOEXCEPT
{
	jstr_cat(&_j->data, &_j->size, &_j->cap, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

#else

#	define jstr_cat_f(_s, _sz, ...)                                                      \
		do {                                                                          \
			JSTR_ASSERT_IS_STR(*(_s));                                            \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                          \
			PRIV_JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                   \
			size_t _ARR_VA_ARGS[PRIV_JSTR_PP_NARG(__VA_ARGS__)];                  \
			*(_sz) += PRIV_JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			char *p = *(_s) + *(_sz);                                             \
			PRIV_JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);            \
			*p = '\0';                                                            \
		} while (0)
#	define jstr_cat(_s, _sz, _cap, ...)                                                                     \
		do {                                                                                             \
			JSTR_ASSERT_IS_STR(*(_s));                                                               \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                                             \
			JSTR_ASSERT_IS_SIZE(*(_cap));                                                            \
			PRIV_JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                                      \
			size_t _ARR_VA_ARGS[PRIV_JSTR_PP_NARG(__VA_ARGS__)];                                     \
			const size_t _newsz = *_sz + PRIV_JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			if (*(_cap) < newsz)                                                                     \
				PRIV_JSTR_REALLOC(*(_s), *(_cap), newsz + 1, break);                             \
			char *p = *(_s) + *(_sz);                                                                \
			PRIV_JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);                               \
			*p = '\0';                                                                               \
			*(_sz) = newsz;                                                                          \
		} while (0)
#	define jstr_alloc_cat_f(_s, _sz, ...)                                               \
		do {                                                                         \
			JSTR_ASSERT_IS_STR(*(_s));                                           \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                         \
			JSTR_ASSERT_IS_SIZE(*(_cap));                                        \
			PRIV_JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                  \
			size_t _ARR_VA_ARGS[PRIV_JSTR_PP_NARG(__VA_ARGS__)];                 \
			*(_sz) = PRIV_JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			char *p = *(_s);                                                     \
			PRIV_JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);           \
			*p = '\0';                                                           \
		} while (0)
#	define jstr_alloc_cat(_s, _sz, _cap, ...)                                           \
		do {                                                                         \
			JSTR_ASSERT_IS_STR(*(_s));                                           \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                         \
			JSTR_ASSERT_IS_SIZE(*(_cap));                                        \
			PRIV_JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                  \
			size_t _ARR_VA_ARGS[PRIV_JSTR_PP_NARG(__VA_ARGS__)];                 \
			*(_sz) = PRIV_JSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			*(_cap) = PRIV_JSTR_MIN_ALLOC(*(_sz));                               \
			*(_s) = malloc(*(_cap));                                             \
			PRIV_JSTR_MALLOC_ERR(*((_s)), break);                                \
			char *p = *(_s);                                                     \
			PRIV_JSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);           \
			*p = '\0';                                                           \
		} while (0)
#	define jstr_cat_j(_j, ...)	  jstr_cat(&((_j)->data), &((_j)->size), &((_j)->_cap), __VA_ARGS__)
#	define jstr_alloc_cat_j(_j, ...) jstr_alloc_cat(&((_j)->data), &((_j)->size), &((_j)->_cap), __VA_ARGS__)

#endif /* __cplusplus */

#endif /* JSTR_BUILDER H_DEF */
