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
#endif /* _cpluslus */

#include "_config.h"
#include "_jtraits.h"
#include "_macros.h"

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
static void
priv_jstr_err(const char *JSTR_RST const FILE_,
	      const int LINE_,
	      const char *JSTR_RST const func_)
{
#if JSTR_CFG_PRINT_ERR_MSG_ON_MALLOC_ERROR
	fprintf(stderr, "%s:%d:%s\n:Can't malloc:", FILE_, LINE_, func_);
	perror("");
#endif
#if JSTR_CFG_EXIT_ON_MALLOC_ERROR
	exit(1);
#endif
}

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
static void
priv_jstr_err_exit(void)
{
#if JSTR_CFG_PRINT_ERR_MSG_ON_MALLOC_ERROR
	fprintf(stderr, "%s:%d:%s\n:Can't malloc:", __FILE__, __LINE__, __func__);
	perror("");
#endif
	exit(1);
}

#define PRIV_JSTR_MALLOC_ERR(p, malloc_fail)                         \
	do {                                                         \
		if (jstr_unlikely((p) == NULL)) {                    \
			priv_jstr_err(__FILE__, __LINE__, __func__); \
			malloc_fail;                                 \
		}                                                    \
	} while (0)
#define PRIV_JSTR_GROW(old_cap, new_cap)                                      \
	do {                                                                  \
		JSTR_ASSERT_IS_SIZE(old_cap);                                 \
		JSTR_ASSERT_IS_SIZE(new_cap);                                 \
		while (((old_cap) *= JSTR_CFG_GROWTH_MULTIPLIER) < (new_cap)) \
			;                                                     \
	} while (0)
#define PRIV_JSTR_REALLOC(p, old_cap, new_cap, malloc_fail) \
	do {                                                \
		JSTR_ASSERT_IS_STR(p);                      \
		JSTR_ASSERT_IS_SIZE(old_cap);               \
		JSTR_ASSERT_IS_SIZE(new_cap);               \
		PRIV_JSTR_GROW(old_cap, new_cap);           \
		(p) = (char *)realloc(p, old_cap);          \
		PRIV_JSTR_MALLOC_ERR(p, malloc_fail);       \
	} while (0)

#if JSTR_HAVE_REALLOC_MREMAP
#	define JSTR_IS_MMAP(_cap) ((jstr_unlikely((_cap) > JSTR_MIN_MMAP - 1)))
#endif /* JSTR_HAVE_REALLOC_MREMAP */

#ifdef __cplusplus

namespace jstr {

namespace _priv {

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_PURE
JSTR_WARN_UNUSED
static constexpr size_t
strlen_args() JSTR_NOEXCEPT
{
	return 0;
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_INLINE
JSTR_NONNULL_ALL static size_t
strlen_args(Str &&_s,
	    StrArgs &&..._args) JSTR_NOEXCEPT
{
	return strlen(std::forward<Str>(_s))
	       + strlen_args(std::forward<StrArgs>(_args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_assign(size_t *JSTR_RST _sz,
	   char **dst,
	   const char (&src)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, src, N - 1);
	*dst += (N - 1);
	*_sz += (N - 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
cat_assign(size_t *_sz,
	   char **dst,
	   const char *JSTR_RST src) JSTR_NOEXCEPT
{
#	if JSTR_HAVE_STPCPY
	char *const _new = stpcpy(*dst, src);
	*_sz += _new - *dst;
	*dst = _new;
#	else
	const size_t len = strlen(*dst);
	memcpy(*dst, src, len);
	*dst += len;
#	endif
}

JSTR_INLINE
static constexpr void
cat_loop_assign(size_t *, char **) JSTR_NOEXCEPT
{
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_loop_assign(size_t *_sz,
		char **dst,
		Str &&_arg,
		StrArgs &&..._args) JSTR_NOEXCEPT
{
	cat_assign(_sz, dst, std::forward<Str>(_arg));
	cat_loop_assign(_sz, dst, std::forward<StrArgs>(_args)...);
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_loop_assign(char **dst,
		Str &&_arg,
		StrArgs &&..._args) JSTR_NOEXCEPT
{
	cat_assign(dst, std::forward<Str>(_arg));
	cat_loop_assign(dst, std::forward<StrArgs>(_args)...);
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_PURE
JSTR_WARN_UNUSED
static constexpr size_t
strlen_args(size_t *) JSTR_NOEXCEPT
{
	return 0;
}

template <typename Str,
	  typename = typename std::enable_if<jtraits_are_strings<Str>(), int>::type>
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_INLINE
JSTR_NONNULL_ALL static size_t
strlen(size_t **JSTR_RST strlen_arr,
       Str &&_arg) JSTR_NOEXCEPT
{
	return ((*(*strlen_arr)++) = ::strlen(std::forward<Str>(_arg)));
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_INLINE
JSTR_NONNULL_ALL static size_t
strlen_args(size_t *strlen_arr,
	    Str &&_s,
	    StrArgs &&..._args) JSTR_NOEXCEPT
{
	return strlen(&strlen_arr, std::forward<Str>(_s))
	       + strlen_args(strlen_arr, std::forward<StrArgs>(_args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_assign(char **JSTR_RST const dst,
	   size_t **JSTR_RST strlen_arr,
	   const char (&src)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, src, N - 1);
	*dst += (N - 1);
	++*strlen_arr;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
cat_assign(char **JSTR_RST dst,
	   size_t **JSTR_RST strlen_arr,
	   const char *JSTR_RST src) JSTR_NOEXCEPT
{
	memcpy(*dst, src, *(*strlen_arr));
	*dst += *(*strlen_arr)++;
}

JSTR_INLINE
static constexpr void
cat_loop_assign(char **, size_t *) JSTR_NOEXCEPT
{
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_loop_assign(char **dst,
		size_t *strlen_arr,
		Str &&_arg,
		StrArgs &&..._args) JSTR_NOEXCEPT
{
	cat_assign(dst, &strlen_arr, std::forward<Str>(_arg));
	cat_loop_assign(dst, strlen_arr, std::forward<StrArgs>(_args)...);
}

} // namespace _priv

} // namespace jstr

/*
   Insert multiple strings to S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_alloc_cat(char **JSTR_RST const _s,
	       size_t *const _sz,
	       size_t *const _cap,
	       Str &&_arg,
	       StrArgs &&..._args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(_args)];
	*_sz = jstr::_priv::strlen_args(strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
	*_cap = *_sz * 2;
	*_s = (char *)malloc(*_cap);
	PRIV_JSTR_MALLOC_ERR(*_s, return);
	char *p = *_s;
	jstr::_priv::cat_loop_assign(&p, strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
	*p = '\0';
}

/*
   Insert multiple strings to S.
   Assumes that S have enough space.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_alloc_cat_f(char *JSTR_RST const _s,
		 size_t *const _sz,
		 Str &&_arg,
		 StrArgs &&..._args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(_args)];
#	if 0
	*_sz = jstr::_priv::strlen_args(std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
#	else
	*_sz = jstr::_priv::strlen_args(strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
#	endif
	jstr::_priv::cat_loop_assign(&_s, strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
	*_s = '\0';
}

/*
   Append multiple strings to end of S.
   Assumes that S have enough space.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_cat(char **JSTR_RST const _s,
	 size_t *const _sz,
	 size_t *const _cap,
	 Str &&_arg,
	 StrArgs &&..._args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(_args)];
#	if 0
	const size_t newsz = *_sz + jstr::_priv::strlen_args(std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
#	else
	const size_t newsz = *_sz + jstr::_priv::strlen_args(strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
#	endif
	if (*_cap < *_sz)
		PRIV_JSTR_REALLOC(*_s, *_cap, newsz + 1, return);
	char *p = *_s + *_sz;
#	if 0
	jstr::_priv::cat_loop_assign(&p, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
#	else
	jstr::_priv::cat_loop_assign(&p, strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
#	endif
	*p = '\0';
	*_sz = newsz;
}

/*
   Append multiple strings to end of S.
   Assumes that S have enough space.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_cat_f(char *_s,
	   size_t *JSTR_RST _sz,
	   Str &&_arg,
	   StrArgs &&..._args) JSTR_NOEXCEPT
{
	_s += *_sz;
	jstr::_priv::cat_loop_assign(_sz, &_s, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
	*_s = '\0';
}

#endif /* __cplusplus */

#endif /* JSTR_TMPLATES_DEF_H */
