#ifndef JSTR_TEMPLATES_H
#define JSTR_TEMPLATES_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#ifdef __cplusplus
#	include <utility>
#endif /* _cpluslus */

#include "jstr-config.h"
#include "jtraits.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_NOTHROW
static void
pjstr_err(const char *R const FILE_,
	  const int LINE_,
	  const char *R const func_) JSTR_NOEXCEPT
{
#if JSTR_ERR_MSG_ON_MALLOC_ERROR
	fprintf(stderr, "%s:%d:%s\n:Can't malloc:", FILE_, LINE_, func_);
	perror("");
#endif
#if JSTR_EXIT_ON_MALLOC_ERROR
	exit(1);
#endif
}

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_NOTHROW
static void
pjstr_err_exit(void) JSTR_NOEXCEPT
{
#if JSTR_ERR_MSG_ON_MALLOC_ERROR
	fprintf(stderr, "%s:%d:%s\n:Can't malloc:", __FILE__, __LINE__, __func__);
	perror("");
#endif
	exit(1);
}

PJSTR_END_DECLS

#define PJSTR_MALLOC_ERR(p, malloc_fail)                         \
	do {                                                     \
		if (jstr_unlikely((p) == NULL)) {                \
			pjstr_err(__FILE__, __LINE__, __func__); \
			malloc_fail;                             \
		}                                                \
	} while (0)
#define PJSTR_GROW(old_cap, new_cap)                                         \
	do {                                                                 \
		JSTR_ASSERT_IS_SIZE(old_cap);                                \
		JSTR_ASSERT_IS_SIZE(new_cap);                                \
		while ((old_cap *= JSTR_GROWTH) < (new_cap))                 \
			;                                                    \
		(old_cap) = PJSTR_ALIGN_UP(old_cap, PJSTR_MALLOC_ALIGNMENT); \
	} while (0)
#define PJSTR_REALLOC(p, old_cap, new_cap, malloc_fail) \
	do {                                            \
		JSTR_ASSERT_IS_STR(p);                  \
		JSTR_ASSERT_IS_SIZE(old_cap);           \
		JSTR_ASSERT_IS_SIZE(new_cap);           \
		PJSTR_GROW(old_cap, new_cap);           \
		(p) = (char *)realloc(p, old_cap);      \
		PJSTR_MALLOC_ERR(p, malloc_fail);       \
	} while (0)
#define PJSTR_REALLOCEXACT(p, old_cap, new_cap, malloc_fail) \
	do {                                                 \
		JSTR_ASSERT_IS_STR(p);                       \
		JSTR_ASSERT_IS_SIZE(old_cap);                \
		JSTR_ASSERT_IS_SIZE(new_cap);                \
		(old_cap) = PJSTR_ALIGN_UP_STR(new_cap);     \
		(p) = (char *)realloc(p, old_cap);           \
		PJSTR_MALLOC_ERR(p, malloc_fail);            \
	} while (0)
#ifdef __cplusplus

namespace jstr {

namespace _priv {

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_PURE
JSTR_WARN_UNUSED
JSTR_NOTHROW
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
JSTR_NONNULL_ALL JSTR_NOTHROW static size_t
strlen_args(Str &&_s,
	    StrArgs &&..._args) JSTR_NOEXCEPT
{
	return strlen(std::forward<Str>(_s))
	       + strlen_args(std::forward<StrArgs>(_args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
appendmore_assign(size_t *R _sz,
		  char **dst,
		  const char (&src)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, src, N - 1);
	*dst += N - 1;
	*_sz += N - 1;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
appendmore_assign(size_t *_sz,
		  char **dst,
		  const char *R src) JSTR_NOEXCEPT
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
JSTR_NOTHROW
static constexpr void
appendmore_loop_assign(size_t *, char **) JSTR_NOEXCEPT
{
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
appendmore_loop_assign(size_t *_sz,
		       char **dst,
		       Str &&_arg,
		       StrArgs &&..._args) JSTR_NOEXCEPT
{
	appendmore_assign(_sz, dst, std::forward<Str>(_arg));
	appendmore_loop_assign(_sz, dst, std::forward<StrArgs>(_args)...);
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
appendmore_loop_assign(char **dst,
		       Str &&_arg,
		       StrArgs &&..._args) JSTR_NOEXCEPT
{
	appendmore_assign(dst, std::forward<Str>(_arg));
	appendmore_loop_assign(dst, std::forward<StrArgs>(_args)...);
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_PURE
JSTR_WARN_UNUSED
JSTR_NOTHROW
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
JSTR_NONNULL_ALL JSTR_NOTHROW static size_t
strlen(size_t *R *R strlen_arr,
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
JSTR_NONNULL_ALL JSTR_NOTHROW static size_t
strlen_args(size_t *strlen_arr,
	    Str &&_s,
	    StrArgs &&..._args) JSTR_NOEXCEPT
{
	return strlen(&strlen_arr, std::forward<Str>(_s))
	       + strlen_args(strlen_arr, std::forward<StrArgs>(_args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
appendmore_assign(char *R *R const dst,
		  size_t *R *R strlen_arr,
		  const char (&src)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, src, N - 1);
	*dst += N - 1;
	++*strlen_arr;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
appendmore_assign(char *R *R dst,
		  size_t *R *R strlen_arr,
		  const char *R src) JSTR_NOEXCEPT
{
	memcpy(*dst, src, *(*strlen_arr));
	*dst += *(*strlen_arr)++;
}

JSTR_INLINE
JSTR_NOTHROW
static constexpr void
appendmore_loop_assign(char **, size_t *) JSTR_NOEXCEPT
{
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
appendmore_loop_assign(char **dst,
		       size_t *strlen_arr,
		       Str &&_arg,
		       StrArgs &&..._args) JSTR_NOEXCEPT
{
	appendmore_assign(dst, &strlen_arr, std::forward<Str>(_arg));
	appendmore_loop_assign(dst, strlen_arr, std::forward<StrArgs>(_args)...);
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
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_alloc_appendmore(char *R *R const _s,
		      size_t *const _sz,
		      size_t *const _cap,
		      Str &&_arg,
		      StrArgs &&..._args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(_args)];
	*_sz = jstr::_priv::strlen_args(strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
	*_cap = *_sz * 2;
	*_s = (char *)malloc(*_cap);
	PJSTR_MALLOC_ERR(*_s, return);
	char *p = *_s;
	jstr::_priv::appendmore_loop_assign(&p, strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
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
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_alloc_appendmore_f(char *R const _s,
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
	jstr::_priv::appendmore_loop_assign(&_s, strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
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
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_appendmore(char *R *R const _s,
		size_t *const _sz,
		size_t *const _cap,
		Str &&_arg,
		StrArgs &&..._args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(_args)];
	const size_t newsz =
#	if 0
	*_sz + jstr::_priv::strlen_args(std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
#	else
	*_sz + jstr::_priv::strlen_args(strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
#	endif
	if (*_cap < *_sz)
		PJSTR_REALLOC(*_s, *_cap, newsz + 1, return);
	char *p = *_s + *_sz;
#	if 0
	jstr::_priv::appendmore_loop_assign(&p, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
#	else
	jstr::_priv::appendmore_loop_assign(&p, strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
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
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_appendmore_f(char *_s,
		  size_t *R _sz,
		  Str &&_arg,
		  StrArgs &&..._args) JSTR_NOEXCEPT
{
	_s += *_sz;
	jstr::_priv::appendmore_loop_assign(_sz, &_s, std::forward<Str>(_arg), std::forward<StrArgs>(_args)...);
	*_s = '\0';
}

#endif /* __cplusplus */

#undef R

#endif /* JSTR_TMPLATES_H */
