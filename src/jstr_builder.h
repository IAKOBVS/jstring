#ifndef JSTR_BUILDER_DEF_H
#define JSTR_BUILDER_DEF_H 1

#include "jstr_macros.h"

PJSTR_BEGIN_DECLS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#ifdef __cplusplus
#	include <utility>
#endif

#ifndef __cplusplus
#	include "jstr_pp_va_args_macros.h"
#endif /* !__cpluslus */

#include "jstr_config.h"
#include "jstr_macros.h"
#include "jstr_templates.h"

#ifdef __cplusplus
#	include "jtraits.h"
#endif /* __cpluslus */

#define PJSTR_MIN_ALLOC(new_cap)                           \
	((new_cap < PJSTR_MIN_CAP / JSTR_ALLOC_MULTIPLIER) \
	 ? (PJSTR_MIN_CAP)                                 \
	 : (new_cap * JSTR_ALLOC_MULTIPLIER))

#define PJSTR_MIN_ALLOCEXACT(new_cap) \
	((new_cap < PJSTR_MIN_CAP)    \
	 ? (PJSTR_MIN_CAP)            \
	 : (new_cap))

#define PJSTR_ALLOC_ONLY(p, _cap, new_cap, do_fail) \
	do {                                        \
		(_cap) = PJSTR_MIN_ALLOC(new_cap);  \
		(p) = (char *)malloc((_cap));       \
		PJSTR_MALLOC_ERR((p), do_fail);     \
	} while (0)

/*
  exit(1) if ptr is NULL.
*/
JSTR_INLINE
JSTR_NOTHROW
static void
jstr_err(char *JSTR_RST const p) JSTR_NOEXCEPT
{
	if (jstr_unlikely(p == NULL))
		pjstr_err_exit();
}

typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t capacity;

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
			pjstr_err_exit();
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
		fprintf(stderr, "size:%zu\ncapacity:%zu\n", this->size, this->capacity);
		fprintf(stderr, "strlen():%zu\n", strlen(this->data));
		fprintf(stderr, "data puts():%s\n", this->data);
		fputs("data:", stderr);
		fwrite(this->data, 1, this->size, stderr);
		fputc('\n', stderr);
	}

#endif /* __cpluslus */

} jstr_ty;

PJSTR_BEGIN_DECLS

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_alloc(char **JSTR_RST const _s,
	   size_t *JSTR_RST const _sz,
	   size_t *JSTR_RST const _cap,
	   const size_t _top)
{
	*_sz = 0;
	*_cap = PJSTR_MIN_ALLOC(PJSTR_ALIGN_UP_STR(_top));
	*_s = (char *)malloc(*_cap);
	PJSTR_MALLOC_ERR(*_s, return);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_allocexact(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const size_t _top) JSTR_NOEXCEPT
{
	*_sz = 0;
	*_cap = PJSTR_MIN_ALLOC(PJSTR_ALIGN_UP_STR(_top));
	*_s = (char *)malloc(*_cap);
	PJSTR_MALLOC_ERR(*_s, return);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_allocexact_assign_len(char **JSTR_RST const _s,
			   size_t *JSTR_RST const _sz,
			   size_t *JSTR_RST const _cap,
			   const char *JSTR_RST const _src,
			   const size_t _srclen) JSTR_NOEXCEPT
{
	jstr_allocexact(_s, _sz, _cap, _srclen + 1);
	if (jstr_unlikely(*_s == NULL))
		return;
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen);
	*(*_s + _srclen) = '\0';
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_alloc_assign_len(char **JSTR_RST const _s,
		      size_t *JSTR_RST const _sz,
		      size_t *JSTR_RST const _cap,
		      const char *JSTR_RST const _src,
		      const size_t _srclen) JSTR_NOEXCEPT
{
	PJSTR_ALLOC_ONLY(*_s, *_cap, _srclen, return);
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen);
	*(*_s + _srclen) = '\0';
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_allocmore_assign_len(char **JSTR_RST const _s,
			  size_t *JSTR_RST const _sz,
			  size_t *JSTR_RST const _cap,
			  const char *JSTR_RST const _src,
			  const size_t _srclen) JSTR_NOEXCEPT
{
	PJSTR_ALLOC_ONLY(*_s, *_cap, _srclen * 2, return);
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen);
	*(*_s + _srclen) = '\0';
}

/*
  free(p) and set p to NULL.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
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
JSTR_NOTHROW
static void
jstr_debug(const jstr_ty *JSTR_RST const _j)
{
	fprintf(stderr, "size:%zu\ncapacity:%zu\n", _j->size, _j->capacity);
	fprintf(stderr, "strlen():%zu\n", strlen(_j->data));
	fprintf(stderr, "data puts:%s\n", _j->data);
	fputs("data:", stderr);
	fwrite(_j->data, 1, _j->size, stderr);
	fputc('\n', stderr);
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_append_len(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const char *JSTR_RST const _src,
		const size_t _srclen) JSTR_NOEXCEPT
{
	if (*_cap < *_sz + _srclen)
		PJSTR_REALLOC(*_s, *_cap, *_sz + _srclen, return);
	memcpy(*_s + *_sz, _src, _srclen);
	*(*_s + (*_sz += _srclen)) = '\0';
}

/*
   Assign SRC to DST.
   S is NUL terminated.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_assign_len(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const char *JSTR_RST const _src,
		const size_t _srclen) JSTR_NOEXCEPT
{
	if (*_cap < _srclen)
		PJSTR_REALLOC(*_s, *_cap, _srclen * JSTR_ALLOC_MULTIPLIER, return);
	memcpy(*_s, _src, _srclen);
	*(*_s + _srclen) = '\0';
	*_sz = _srclen;
}

/*
   Push C to end of S.
   S is NUL terminated.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_push_back(char **JSTR_RST const _s,
	       size_t *JSTR_RST const _sz,
	       size_t *JSTR_RST const _cap,
	       const char _c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_cap == *_sz + 1))
		PJSTR_REALLOCEXACT(*_s, *_cap, *_sz * JSTR_ALLOC_MULTIPLIER, return);
	*(*_s + *_sz) = _c;
	*(*_s + ++*_sz) = '\0';
}

/*
   Push C to end of S.
   S is NUL terminated.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_push_front(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const char _c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_cap == *_sz + 1))
		PJSTR_REALLOCEXACT(*_s, *_cap, *_sz * JSTR_ALLOC_MULTIPLIER, return);
	memmove(*_s + 1, *_s, (*_sz)++ + 1);
	**_s = _c;
}

/* Pop s[size]. */
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_back(char *JSTR_RST const _s,
	      size_t *JSTR_RST const _sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_sz == 0))
		return;
	*(_s + (*_sz)-- - 1) = '\0';
}

/*
   Pop s[size].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_back_j(jstr_ty *JSTR_RST const _j) JSTR_NOEXCEPT
{
	jstr_pop_back(_j->data, &_j->size);
}

/* Pop s[0]. */
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_front(char *JSTR_RST const _s,
	       size_t *JSTR_RST const _sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_sz == 0))
		return;
	memmove(_s, _s + 1, (*_sz)--);
}

/* Pop s[0]. */
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_front_j(jstr_ty *JSTR_RST const _j) JSTR_NOEXCEPT
{
	jstr_pop_front(_j->data, &_j->size);
}

PJSTR_END_DECLS

#ifdef __cplusplus

/*
  Insert multiple strings to S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_alloc_appendmore_j(jstr_ty *JSTR_RST const _j,
			Str &&arg,
			StrArgs &&...args) JSTR_NOEXCEPT
{
	jstr_alloc_appendmore(&_j->data, &_j->size, &_j->capacity, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

/*
  Append multiple strings to end of S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_appendmore_j(jstr_ty *JSTR_RST const _j,
		  Str &&arg,
		  StrArgs &&...args) JSTR_NOEXCEPT
{
	jstr_appendmore(&_j->data, &_j->size, &_j->capacity, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

#else

#	define jstr_appendmore_f(_s, _sz, ...)                                           \
		do {                                                                      \
			JSTR_ASSERT_IS_STR(*(_s));                                        \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                      \
			PJSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                   \
			size_t _ARR_VA_ARGS[PJSTR_PP_NARG(__VA_ARGS__)];                  \
			*(_sz) += PJSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			char *p = *(_s) + *(_sz);                                         \
			PJSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);            \
			*p = '\0';                                                        \
		} while (0)

#	define jstr_appendmore(_s, _sz, _cap, ...)                                                             \
		do {                                                                                            \
			JSTR_ASSERT_IS_STR(*(_s));                                                              \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                                            \
			JSTR_ASSERT_IS_SIZE(*(_cap));                                                           \
			PJSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                                         \
			size_t _ARR_VA_ARGS[PJSTR_PP_NARG(__VA_ARGS__)];                                        \
			const size_t _NEW_SZ = *(_sz) + PJSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			if (*(_cap) < _NEW_SZ)                                                                  \
				PJSTR_REALLOC(*(_s), *(_cap), _NEW_SZ + 1, break);                              \
			char *p = *(_s) + *(_sz);                                                               \
			PJSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);                                  \
			*p = '\0';                                                                              \
			*(_sz) = _NEW_SZ;                                                                       \
		} while (0)

#	define jstr_alloc_appendmore(_s, _sz, _cap, ...)                                \
		do {                                                                     \
			JSTR_ASSERT_IS_STR(*(_s));                                       \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                     \
			JSTR_ASSERT_IS_SIZE(*(_cap));                                    \
			PJSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                  \
			size_t _ARR_VA_ARGS[PJSTR_PP_NARG(__VA_ARGS__)];                 \
			*(_sz) = PJSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			*(_cap) = PJSTR_MIN_ALLOC(PJSTR_ALIGN_UP_STR(*(_sz)));           \
			*(_s) = malloc(*(_cap));                                         \
			PJSTR_MALLOC_ERR(*((_s)), break);                                \
			char *p = *(_s);                                                 \
			PJSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);           \
			*p = '\0';                                                       \
		} while (0)

#	define jstr_appendmore_j(_j, ...)	 jstr_appendmore(&((_j)->data), &((_j)->size), &((_j)->capacity), __VA_ARGS__)
#	define jstr_alloc_appendmore_j(_j, ...) jstr_alloc_appendmore(&((_j)->data), &((_j)->size), &((_j)->capacity), __VA_ARGS__)

#endif /* __cplusplus */

#endif /* JSTR_BUILDER DEF_H */
