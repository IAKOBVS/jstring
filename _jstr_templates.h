#ifndef JSTR_TEMPLATES_DEF_H
#define JSTR_TEMPLATES_DEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef __cplusplus
#	include <utility>
#endif /* __cpluslus */

#include "_jstr_config.h"
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
#define JSTR_GROW(old_cap, new_cap) \
	while (((old_cap) *= JSTR_GROWTH_MULTIPLIER) < (new_cap))
#define JSTR_REALLOC(p, old_cap, new_cap, malloc_fail) \
	do {                                           \
		JSTR_GROW(old_cap, new_cap);           \
		(p) = (char *)realloc(p, old_cap);     \
		JSTR_MALLOC_ERR(p, malloc_fail);       \
	} while (0)
#ifdef __cplusplus

namespace jstr {

namespace _private {

#	if 0

JSTR_WARN_UNUSED
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
#		ifdef JSTR_HAS_STPCPY
	*dst = stpcpy(*dst, src);
#		else
	while (*src)
		**dst++ = *src++;
#		endif
}

JSTR_INLINE
static constexpr void cat_loop_assign(char **) JSTR_NOEXCEPT {}

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

#	endif

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static constexpr size_t strlen_args(size_t *) JSTR_NOEXCEPT
{
	return 0;
}

template <typename Str,
	  typename = typename std::enable_if<traits::are_strings<Str>(), int>::type>
JSTR_WARN_UNUSED
JSTR_CONST
JSTR_INLINE
JSTR_NONNULL_ALL static size_t
strlen(size_t **JSTR_RST _strlen_arr,
       Str &&arg) JSTR_NOEXCEPT
{
	return ((*(*_strlen_arr)++) = ::strlen(std::forward<Str>(arg)));
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<traits::are_strings<Str, StrArgs...>(), int>::type>
JSTR_WARN_UNUSED
JSTR_CONST
JSTR_INLINE
JSTR_NONNULL_ALL static size_t
strlen_args(size_t *_strlen_arr,
	    Str &&s,
	    StrArgs &&...args) JSTR_NOEXCEPT
{
	return strlen(&_strlen_arr, std::forward<Str>(s))
	       + strlen_args(_strlen_arr, std::forward<StrArgs>(args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_assign(char **JSTR_RST const dst,
	   size_t **JSTR_RST _strlen_arr,
	   const char (&src)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, src, N - 1);
	*dst += (N - 1);
	++*_strlen_arr;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void cat_assign(char **JSTR_RST dst,
		       size_t **JSTR_RST _strlen_arr,
		       const char *JSTR_RST src) JSTR_NOEXCEPT
{
	memcpy(*dst, src, *(*_strlen_arr));
	*dst += *(*_strlen_arr)++;
}

JSTR_INLINE
static constexpr void cat_loop_assign(char **, size_t *) JSTR_NOEXCEPT {}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<traits::are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_loop_assign(char **JSTR_RST const dst,
		size_t *_strlen_arr,
		Str &&arg,
		StrArgs &&...args) JSTR_NOEXCEPT
{
	cat_assign(dst, &_strlen_arr, std::forward<Str>(arg));
	cat_loop_assign(dst, _strlen_arr, std::forward<StrArgs>(args)...);
}

} /* namespace _private */

/*
   Insert multiple strings to S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<traits::are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
alloc_cat(char **JSTR_RST const s,
	  size_t *const sz,
	  size_t *const cap,
	  Str &&arg,
	  StrArgs &&...args) JSTR_NOEXCEPT
{
	size_t _strlen_arr[1 + sizeof...(args)];
	*sz = _private::strlen_args(_strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*cap = *sz * 2;
	*s = (char *)malloc(*cap);
	JSTR_MALLOC_ERR(*s, return);
	char *p = *s;
	_private::cat_loop_assign(&p, _strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
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
	    size_t *const sz,
	    Str &&arg,
	    StrArgs &&...args) JSTR_NOEXCEPT
{
	size_t _strlen_arr[1 + sizeof...(args)];
#	if 0
	*sz = _private::strlen_args(std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	else
	*sz = _private::strlen_args(_strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	endif
	_private::cat_loop_assign(&s, _strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
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
    size_t *const sz,
    size_t *const cap,
    Str &&arg,
    StrArgs &&...args) JSTR_NOEXCEPT
{
	size_t _strlen_arr[1 + sizeof...(args)];
#	if 0
	const size_t newsz = *sz + _private::strlen_args(std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	else
	const size_t newsz = *sz + _private::strlen_args(_strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	endif
	if (*cap < *sz + 1)
		JSTR_REALLOC(*s, *cap, newsz + 1, return);
	char *p = *s + *sz;
#	if 0
	_private::cat_loop_assign(&p, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	else
	_private::cat_loop_assign(&p, _strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	endif
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
      size_t *const sz,
      Str &&arg,
      StrArgs &&...args) JSTR_NOEXCEPT
{
	s += *sz;
	*sz += _private::strlen_args(std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	_private::cat_loop_assign(&s, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*s = '\0';
}

} /* namespace jstr */

#endif /* __cplusplus */

#endif /* JSTR_TEMPLATES_DEF_H */
