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

#ifdef __cplusplus
#	include "_jstr_traits.h"
#endif /* __cpluslus */

#define JSTR_EXTERN_C  1
#define JSTR_NAMESPACE 0

/* This is executed every time a malloc error is encountered. */
JSTR_NOINLINE
JSTR_COLD
JSTR_MAYBE_UNUSED
static void JSTR_ERR(void) JSTR_NOEXCEPT
{
#if JSTR_PRINT_ERR_MSG_ON_MALLOC_ERROR
	fprintf(stderr, "%s:%d:%s:Can't malloc:", __FILE__, __LINE__, __func__);
	perror("");
#endif /* JSTR_PRINT_ERR_MSG_ON_MALLOC_ERROR */
#if JSTR_EXIT_ON_MALLOC_ERROR
	exit(1);
#endif /* JSTR_EXIT_ON_MALLOC_ERROR */
}

#define JSTR_MALLOC_ERR(p, malloc_fail) \
	do {                            \
		if (unlikely(!(p))) {   \
			JSTR_ERR();     \
			malloc_fail;    \
		}                       \
	} while (0)

#define JSTR_GROW(old_cap, new_cap) \
	while (((old_cap) *= JSTR_GROWTH_MULTIPLIER) < (new_cap))

#define JSTR_REALLOC(p, old_cap, new_cap, malloc_fail) \
	do {                                           \
		JSTR_GROW(old_cap, new_cap);           \
		(p) = (char *)realloc(p, old_cap);     \
		JSTR_MALLOC_ERR(p, malloc_fail);       \
	} while (0)

JSTR_NOINLINE
JSTR_COLD
static void JSTR_ERR_EXIT()
{
	fprintf(stderr, "%s:%d:%s:Can't malloc:", __FILE__, __LINE__, __func__);
	perror("");
	exit(1);
}

#ifdef __cplusplus

namespace jstr {

/*
   Insert multiple strings to S.
*/
template <typename Str,
	  typename... StrArgs>
JSTR_INLINE
JSTR_NONNULL_ALL static void
alloc_cat(char **JSTR_RST const s,
	  size_t *JSTR_RST const sz,
	  size_t *JSTR_RST const cap,
	  Str &&arg,
	  StrArgs &&...args) JSTR_NOEXCEPT;

#endif /* __cpluslus */

/*
  exit(1) if ptr is NULL.
*/
JSTR_INLINE
static void jstr_err(char *JSTR_RST const p) JSTR_NOEXCEPT
{
	if (unlikely(!p))
		JSTR_ERR_EXIT();
}

#ifndef __cplusplus

typedef struct jstr_t {
	size_t size;
	size_t cap;
	char *data;
} jstr_t;

#else

typedef struct jstr_t {
	size_t size;
	size_t cap;
	char *data;

	template <typename Str,
		  typename... StrArgs>
	JSTR_INLINE
	JSTR_NONNULL_ALL
	jstr_t(Str &&arg,
	       StrArgs &&...args) JSTR_NOEXCEPT
	{
		jstr::alloc_cat(&this->data, &this->size, &this->cap, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	}

	JSTR_INLINE
	JSTR_NONNULL_ALL
	jstr_t() JSTR_NOEXCEPT
	{
		this->data = NULL;
	}

	JSTR_INLINE
	JSTR_NONNULL_ALL
	jstr_t(const size_t len) JSTR_NOEXCEPT
	{
		jstr_alloc(&this->data, &this->size, &this->cap, len);
	}

	JSTR_INLINE
	JSTR_NONNULL_ALL
	jstr_t(const char *JSTR_RST const src) JSTR_NOEXCEPT
	{
		jstr_alloc_append(&this->data, &this->size, &this->cap, src);
	}

	JSTR_INLINE
	JSTR_NONNULL_ALL
	jstr_t(const char *JSTR_RST const src,
	       const size_t srclen) JSTR_NOEXCEPT
	{
		jstr_alloc_append_mem(&this->data, &this->size, &this->cap, src, srclen);
	}

#	if JSTR_FREE_ON_DESTRUCTOR_CPP

	JSTR_INLINE
	~jstr_t(void) JSTR_NOEXCEPT
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
		fprintf(stderr, "data puts():%s\n", this->data);
		fputs("data:", stderr);
		fwrite(this->data, 1, this->size, stderr);
		fputc('\n', stderr);
	}

    private:
#endif /* __cpluslus */

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc(char **JSTR_RST const s,
		       size_t *JSTR_RST const sz,
		       size_t *JSTR_RST const cap,
		       const size_t top) JSTR_NOEXCEPT
{
	*sz = 0;
	*s = (char *)malloc(top * JSTR_GROWTH_MULTIPLIER);
	JSTR_MALLOC_ERR(*s, return);
	*cap = top * JSTR_GROWTH_MULTIPLIER;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_append_mem(char **JSTR_RST const s,
				  size_t *JSTR_RST const sz,
				  size_t *JSTR_RST const cap,
				  const char *JSTR_RST const src,
				  const size_t srclen) JSTR_NOEXCEPT
{
	jstr_alloc(s, sz, cap, srclen * JSTR_GROWTH_MULTIPLIER);
	if (unlikely(!*s))
		return;
	*sz = srclen;
	memcpy(*s, src, srclen + 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_append(char **JSTR_RST const s,
			      size_t *JSTR_RST const sz,
			      size_t *JSTR_RST const cap,
			      const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_alloc_append_mem(s, sz, cap, src, strlen(src));
}

#ifdef __cplusplus
}
jstr_t;

#endif /* __cpluslus */

#ifdef __cplusplus

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc(char **JSTR_RST const s,
		       size_t *JSTR_RST const sz,
		       size_t *JSTR_RST const cap,
		       const size_t top) JSTR_NOEXCEPT
{
	*sz = 0;
	*s = (char *)malloc(top * JSTR_GROWTH_MULTIPLIER);
	JSTR_MALLOC_ERR(*s, return);
	*cap = top * JSTR_GROWTH_MULTIPLIER;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_append_mem(char **JSTR_RST const s,
				  size_t *JSTR_RST const sz,
				  size_t *JSTR_RST const cap,
				  const char *JSTR_RST const src,
				  const size_t srclen) JSTR_NOEXCEPT
{
	jstr_alloc(s, sz, cap, srclen * JSTR_GROWTH_MULTIPLIER);
	if (unlikely(!*s))
		return;
	*sz = srclen;
	memcpy(*s, src, srclen + 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_append(char **JSTR_RST const s,
			      size_t *JSTR_RST const sz,
			      size_t *JSTR_RST const cap,
			      const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_alloc_append_mem(s, sz, cap, src, strlen(src));
}

#endif /* __cpluslus */

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

JSTR_NOINLINE
JSTR_COLD
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static void jstr_debug(const jstr_t *JSTR_RST const j)
{
	fprintf(stderr, "size:%zu\ncap:%zu\n", j->size, j->cap);
	fprintf(stderr, "data:%s\n", j->data);
}

#ifdef __cplusplus

namespace priv {

JSTR_INLINE
JSTR_NONNULL_ALL
static void cat_assign(char **JSTR_RST const dst,
		       const jstr_t *JSTR_RST const src) JSTR_NOEXCEPT
{
	memcpy(*dst, src->data, src->size);
	*dst += src->size;
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static constexpr size_t strlen_args() JSTR_NOEXCEPT
{
	return 0;
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static size_t strlen_arg(const char *s) JSTR_NOEXCEPT
{
	return strlen(s);
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static size_t strlen_arg(const jstr_t *j) JSTR_NOEXCEPT
{
	return j->size;
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<traits::are_strings<Str, StrArgs...>(), int>::type>
JSTR_WARN_UNUSED
JSTR_CONST
JSTR_INLINE
JSTR_NONNULL_ALL static size_t
strlen_args(Str &&s,
	    StrArgs &&...args) JSTR_NOEXCEPT
{
	return strlen_arg(std::forward<Str>(s))
	       + strlen_args(std::forward<StrArgs>(args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_assign(char **JSTR_RST const dst,
	   const char (&src)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, src, N - 1);
	*dst += (N - 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void cat_assign(char **JSTR_RST dst,
		       const char *JSTR_RST src) JSTR_NOEXCEPT
{
#	ifdef JSTR_HAS_STPCPY
	*dst = stpcpy(*dst, src);
#	else
	while (*src)
		**dst++ = *src++;
#	endif
}

JSTR_INLINE
static constexpr void cat_loop_assign(char **JSTR_RST const) JSTR_NOEXCEPT {}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<traits::are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_loop_assign(char **JSTR_RST const dst,
		Str &&arg,
		StrArgs &&...args) JSTR_NOEXCEPT
{
	cat_assign(dst, std::forward<Str>(arg));
	cat_loop_assign(dst, std::forward<StrArgs>(args)...);
}

} /* namespace priv */

/*
   Insert multiple strings to S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<traits::are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
alloc_cat(char **JSTR_RST const s,
	  size_t *JSTR_RST const sz,
	  size_t *JSTR_RST const cap,
	  Str &&arg,
	  StrArgs &&...args) JSTR_NOEXCEPT
{
	*sz = priv::strlen_args(std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*cap = *sz * 2;
	*s = (char *)malloc(*cap);
	JSTR_MALLOC_ERR(*s, return);
	char *p = *s;
	priv::cat_loop_assign(&p, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*p = '\0';
}

/*
   Insert multiple strings to S.
   Assumes that S have enough space.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<traits::are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
alloc_cat_f(char *JSTR_RST const s,
	    size_t *JSTR_RST const sz,
	    Str &&arg,
	    StrArgs &&...args) JSTR_NOEXCEPT
{
	*sz = priv::strlen_args(std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	priv::cat_loop_assign(&s, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*s = '\0';
}

/*
   Append multiple strings to end of S.
   Assumes that S have enough space.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<traits::are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat(char **JSTR_RST const s,
    size_t *JSTR_RST const sz,
    size_t *JSTR_RST const cap,
    Str &&arg,
    StrArgs &&...args) JSTR_NOEXCEPT
{
	const size_t newsz = *sz + priv::strlen_args(std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	if (*cap < *sz + 1)
		JSTR_REALLOC(*s, *cap, newsz + 1, return);
	char *p = *s + *sz;
	priv::cat_loop_assign(&p, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*p = '\0';
	*sz = newsz;
}

/*
   Append multiple strings to end of S.
   Assumes that S have enough space.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<traits::are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_f(char *JSTR_RST s,
      size_t *JSTR_RST const sz,
      Str &&arg,
      StrArgs &&...args) JSTR_NOEXCEPT
{
	s += *sz;
	*sz += priv::strlen_args(std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	priv::cat_loop_assign(&s, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*s = '\0';
}

/*
  Insert multiple strings to S.
*/
template <typename Str,
	  typename... StrArgs>
JSTR_INLINE
JSTR_NONNULL_ALL static void
alloc_cat_j(jstr_t *JSTR_RST const j,
	    Str &&arg,
	    StrArgs &&...args) JSTR_NOEXCEPT
{
	alloc_cat(&j->data, &j->size, &j->cap, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

/*
  Append multiple strings to end of S.
*/
template <typename Str,
	  typename... StrArgs>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_j(jstr_t *JSTR_RST const j,
      Str &&arg,
      StrArgs &&...args) JSTR_NOEXCEPT
{
	cat(&j->data, &j->size, &j->cap, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

} /* namespace jstr */

#else

#	define jstr_cat_f(s, sz, ...)                                 \
		do {                                                   \
			JSTR_ASSERT_IS_STR(*(s));                      \
			JSTR_ASSERT_IS_SIZE(*(sz));                    \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__); \
			*(sz) += JSTR_PP_STRLEN_VA_ARGS(__VA_ARGS__);  \
			char *p = *(s) + *(sz);                        \
			JSTR_PP_STRCPY_VA_ARGS(p, __VA_ARGS__);        \
			*p = '\0';                                     \
		} while (0)

#	define jstr_cat(s, sz, cap, ...)                                               \
		do {                                                                    \
			JSTR_ASSERT_IS_STR(*(s));                                       \
			JSTR_ASSERT_IS_SIZE(*(sz));                                     \
			JSTR_ASSERT_IS_SIZE(*(cap));                                    \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                  \
			const size_t newsz = *sz + JSTR_PP_STRLEN_VA_ARGS(__VA_ARGS__); \
			if (*(cap) < newsz + 1)                                         \
				JSTR_REALLOC(*(s), *(cap), newsz + 1, break);           \
			char *p = *(s) + *(sz);                                         \
			JSTR_PP_STRCPY_VA_ARGS(p, __VA_ARGS__);                         \
			*p = '\0';                                                      \
			*(sz) = newsz;                                                  \
		} while (0)

#	define jstr_alloc_cat_f(s, sz, ...)                           \
		do {                                                   \
			JSTR_ASSERT_IS_STR(*(s));                      \
			JSTR_ASSERT_IS_SIZE(*(sz));                    \
			JSTR_ASSERT_IS_SIZE(*(cap));                   \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__); \
			*(sz) = JSTR_PP_STRLEN_VA_ARGS(__VA_ARGS__);   \
			char *p = *(s);                                \
			JSTR_PP_STRCPY_VA_ARGS(p, __VA_ARGS__);        \
			*p = '\0';                                     \
		} while (0)

#	define jstr_alloc_cat(s, sz, cap, ...)                        \
		do {                                                   \
			JSTR_ASSERT_IS_STR(*(s));                      \
			JSTR_ASSERT_IS_SIZE(*(sz));                    \
			JSTR_ASSERT_IS_SIZE(*(cap));                   \
			JSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__); \
			*(sz) = JSTR_PP_STRLEN_VA_ARGS(__VA_ARGS__);   \
			*(cap) = *(sz)*2;                              \
			*(s) = malloc(*(cap));                         \
			JSTR_MALLOC_ERR(*((s)), break);                \
			char *p = *(s);                                \
			JSTR_PP_STRCPY_VA_ARGS(p, __VA_ARGS__);        \
			*p = '\0';                                     \
		} while (0)

#	define jstr_cat_j(j, ...)	 jstr_cat(&((j)->data), &((j)->size), &((j)->cap), __VA_ARGS__)
#	define jstr_alloc_cat_j(j, ...) jstr_alloc_cat(&((j)->data), &((j)->size), &((j)->cap), __VA_ARGS__)

#endif /* __cpluslus */

#undef JSTR_EXTERN_C
#undef JSTR_NAMESPACE

#endif /* JSTR_BUILDER H_DEF */
