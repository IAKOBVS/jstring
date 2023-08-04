#ifndef JSTR_TEMPLATES_DEF_H
#define JSTR_TEMPLATES_DEF_H

#ifdef __cplusplus
extern "C" {
#	include <stddef.h>
#	include <stdlib.h>
#	include <string.h>
#	include <stdio.h>
}
#	include <utility>
#endif /* __cplusplus */

#include "_jstr_macros.h"
#include "_jstr_traits.h"

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

#define JSTR_GROW(oldcap, newcap) \
	while (((oldcap) *= 2) < (newcap))

#define JSTR_REALLOC(p, oldcap, newcap, malloc_fail) \
	do {                                         \
		JSTR_GROW(oldcap, newcap);           \
		(p) = (char *)realloc(p, newcap);    \
		JSTR_MALLOC_ERR(p, malloc_fail);     \
	} while (0)

#ifdef __cplusplus

namespace jstr {

namespace priv {

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static constexpr size_t strlen_args() JSTR_NOEXCEPT
{
	return 0;
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
	return strlen(std::forward<Str>(s))
	       + strlen_args(std::forward<StrArgs>(args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_assign(char **JSTR_RST const dst,
	   const char (&s)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, s, N - 1);
	*dst += (N - 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void cat_assign(char **JSTR_RST dst,
		       char *JSTR_RST src) JSTR_NOEXCEPT
{
#	ifdef JSTR_HAS_STPCPY
	*dst = stpcpy(*dst, src);
#	else
	while (*src)
		**dst++ = *src++;
#	endif
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic push
#endif /* defined(__GNUC__) || defined(__clang__) */
	
JSTR_INLINE
JSTR_NONNULL_ALL
static void cat_assign(char **JSTR_RST const dst,
		       jstr_t *JSTR_RST const src) JSTR_NOEXCEPT;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* defined(__GNUC__) || defined(__clang__) */

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
	*s = (char *)malloc(*sz * 2);
	if (unlikely(!*s)) {
		JSTR_ERR();
		return;
	}
	*cap = *sz * 2;
	char *p = *s;
	priv::cat_loop_assign(&p, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*p = '\0';
}

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
	const size_t new_cap = *sz + priv::strlen_args(std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	if (*cap < new_cap)
		JSTR_REALLOC(*s, *cap, *sz, return);
	if (unlikely(!*s)) {
		JSTR_ERR();
		return;
	}
	char *p = *s + *sz;
	*cap = *sz * 2;
	priv::cat_loop_assign(&p, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*p = '\0';
	*sz = new_cap;
}

} /* namespace jstr */

#endif /* __cplusplus */

#endif /* JSTR_TEMPLATES_DEF_H */
