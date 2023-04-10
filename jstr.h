#ifndef JSTR_H_DEF__
#define JSTR_H_DEF__

/* rename to the right path */

#define JSTR_PATH_TO_PP_VA_ARGS_MACROS_H "/home/james/c/pp_macros/pp_va_args_macros.h"

/*
   at() will do bounds checking by asserting that index <= this->size.
   it will crash at runtime if assertion fails.

   _u (unsafe) functions will not do any allocations.
   _f (force/fast) functions will force allocations.
   _s (safe) functons will check if object has been allocated with malloc.
*/

#define JSTR_ALIGN_POWER_OF_TWO
#define JSTR_64_BIT

#if defined(__GNUC__) || defined(__GLIBC__)
#	ifndef _GNU_SOURCE
#		define _GNU_SOURCE
#	endif
#endif

#ifndef __cplusplus
#	include <stdarg.h>
#	include <stdlib.h>
#	include <string.h>
#	include <assert.h>
#endif // ! __cplusplus

#include "macros.h"
#include "jstd.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#	include <sys/cdefs.h>
#	include JSTR_PATH_TO_PP_VA_ARGS_MACROS_H
#ifdef __cplusplus
}
#	include <type_traits>
#	include <cstring>
#	include <cstdarg>
#	include <cstdlib>
#	include <utility>
#	include <cassert>
#endif // __cplusplus

#define JSTR_MIN_CAP 8
#define JSTR_MULTIPLIER 2

#define jstr_init(this_)         \
do {                             \
	((this_)->data) = NULL;  \
	((this_)->capacity) = 0; \
	((this_)->size) = 0;     \
} while (0)

#define jstr_delete(this_)       \
do {                             \
	((this_)->capacity) = 0; \
	((this_)->size) = 0;     \
	free(((this_)->data));   \
	((this_)->data) = NULL;  \
} while (0)

#ifndef __cplusplus // ! __cplusplus

#define jstring(name) jstring_t name = {0}

typedef struct jstring_t jstring_t;

int jstr_new(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_new_append(jstring_t *JSTR_RESTRICT__ this_, size_t srclen, const char *JSTR_RESTRICT__ src_, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int private_jstr_new_cat(jstring_t *JSTR_RESTRICT__ this_, size_t arglen, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

void jstr_push_back_u(jstring_t *this_, const char c) JSTR_NOEXCEPT__;
int jstr_push_back_f(jstring_t *this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_push_back(jstring_t *this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_push_back_s(jstring_t *this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

void jstr_push_front_u(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__;
int jstr_push_front_f(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_push_front(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_push_front_s(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

void jstr_pop_back(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__;
void jstr_pop_back_s(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__;

void jstr_pop_front(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__;
void jstr_pop_front_s(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__;

void private_jstr_append_u(jstring_t *dest, const char *JSTR_RESTRICT__ src, size_t srclen) JSTR_NOEXCEPT__;
int private_jstr_append(jstring_t *dest, const char *JSTR_RESTRICT__ src, size_t srclen) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int private_jstr_append_s(jstring_t *dest, const char *JSTR_RESTRICT__ src, size_t srclen) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

void jstr_swap(jstring_t *JSTR_RESTRICT__ this_, jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__;
void jstr_swap_str(jstring_t *JSTR_RESTRICT__ this_, char **JSTR_RESTRICT__ other_, size_t *other_len, size_t *other_cap) JSTR_NOEXCEPT__;

int jstr_shrink_to_fit_f(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_shrink_to_fit(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

int jstr_shrink_to_f(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_shrink_to(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

void jstr_shrink_to_size_f(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__;
int jstr_shrink_to_size(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__;
int jstr_shrink_to_size_s(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__;

int jstr_reserve(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_reserve_s(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_reserve_f(jstring_t *JSTR_RESTRICT__ this_, size_t cap, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

int private_jstr_assign(jstring_t *JSTR_RESTRICT__ dest, const char *JSTR_RESTRICT__ src, size_t srclen, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
void jstr_assign_u(jstring_t *JSTR_RESTRICT__ dest, const char *JSTR_RESTRICT__ src, size_t srclen) JSTR_NOEXCEPT__;
int jstr_assign_f(jstring_t *JSTR_RESTRICT__ dest, const char *JSTR_RESTRICT__ src, size_t srclen) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

void jstr_assign_jstr_u(jstring_t *JSTR_RESTRICT__ dest, const jstring_t *JSTR_RESTRICT__ src) JSTR_NOEXCEPT__;
int jstr_assign_jstr_f(jstring_t *JSTR_RESTRICT__ dest, const jstring_t *JSTR_RESTRICT__ src) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_assign_jstr(jstring_t *JSTR_RESTRICT__ dest, const jstring_t *JSTR_RESTRICT__ src, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

/* if equals returns 0 */
int jstr_cmp(const jstring_t *JSTR_RESTRICT__ const this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_cmp_f(const jstring_t *JSTR_RESTRICT__ const this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_cmp_str(const jstring_t *JSTR_RESTRICT__ const this_, const char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

int jstr_casecmp_f(const jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_casecmp_jstr(const jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_casecmp_str(const jstring_t *JSTR_RESTRICT__ this_, const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

/* memrchr */
char *jstr_rchr(const jstring_t *JSTR_RESTRICT__ this_, const int c) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

#ifdef JSTR_HAS_MEMMEM__
#	define jstr_str(this_, ...)                                                                             \
		(PP_NARG(__VA_ARGS__) == 1)                                                                     \
			? private_jstr_str(this_, PP_FIRST_ARG(__VA_ARGS__), strlen(PP_FIRST_ARG(__VA_ARGS__))) \
			: private_jstr_str(this_, __VA_ARGS__, 0)
char *private_jstr_str(jstring_t *haystack, const char *JSTR_RESTRICT__ needle, size_t needlelen, ...) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
#else
char *jstr_str(jstring_t *haystack, const char *JSTR_RESTRICT__ needle) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
#endif

int jstr_dup(jstring_t *JSTR_RESTRICT__ this_, jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_dup_s(jstring_t *JSTR_RESTRICT__ this_, jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

int jstr_ndup(jstring_t *JSTR_RESTRICT__ this_, jstring_t *JSTR_RESTRICT__ other_, size_t n) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_ndup_s(jstring_t *JSTR_RESTRICT__ this_, jstring_t *JSTR_RESTRICT__ other_, size_t n) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

void jstr_rev(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__;

int jstr_rev_dup(jstring_t *JSTR_RESTRICT__ src, char **JSTR_RESTRICT__ dest) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

#endif // ! __cplusplus

typedef struct jstring_t {
#ifndef __cplusplus // ! __cpluslus
	size_t size; 
	size_t capacity;
	char *data;
} jstring_t;
#else // __cplusplus
	std::size_t size;
	std::size_t capacity;
	char *data;

	JSTR_INLINE__
	jstring_t() JSTR_NOEXCEPT__ { jstr_init(this); }

	JSTR_INLINE__
	~jstring_t() JSTR_NOEXCEPT__
	{
		free(this->data);
		this->data = nullptr;
		this->capacity = 0;
		this->size = 0;
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	size_t csize() JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->size; }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	size_t clength() JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->size; }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	size_t ccapacity() JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->capacity; }
	
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	const char *cdata() JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->data; }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	const char *c_str() JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->data; }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *begin() JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->data; }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *end() JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->data + this->size; }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	const char *cbegin() JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->data; }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	const char *cend() JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->data + this->size; }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int empty() JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return !!this->size; }

	JSTR_INLINE__
	jstring_t(const jstring_t& other_) JSTR_NOEXCEPT__
	{
		this->size = other_.size;
		if (!this->capacity) {
			this->data = (char *)std::malloc(other_.capacity);
			if (unlikely(!this->data))
				return;
		} else if (this->capacity < other_.capacity) {
			char *tmp = (char *)std::realloc(this->data, other_.capacity);
			if (unlikely(!tmp))
				return;
			this->data = tmp;
		}
		std::memcpy(this->data, other_.data, other_.size + 1);
		this->capacity = other_.capacity;
	}

	JSTR_INLINE__
	jstring_t(jstring_t&& other_) JSTR_NOEXCEPT__
	{
		this->size = other_.size;
		this->capacity = other_.capacity;
		this->data = other_.data;
		other_.data = nullptr;
		other_.capacity = 0;
		other_.size = 0;
	}

	JSTR_INLINE__
	jstring_t& operator=(const jstring_t& other_) JSTR_NOEXCEPT__
	{
		this->size = other_.size;
		if (!this->capacity) {
			this->data = (char *)std::malloc(other_.capacity);
			if (unlikely(!this->data))
				return *this;
		} else if (this->capacity < other_.capacity) {
			char *tmp = (char *)std::realloc(this->data, other_.capacity);
			if (unlikely(!tmp))
				return *this;
			this->data = tmp;
		}
		std::memcpy(this->data, other_.data, other_.size + 1);
		this->capacity = other_.capacity;
		return *this;
	}

	JSTR_INLINE__
	jstring_t& operator=(jstring_t&& other_) JSTR_NOEXCEPT__
	{
		this->size = other_.size;
		this->capacity = other_.capacity;
		this->data = other_.data;
		other_.data = nullptr;
		other_.capacity = 0;
		other_.size = 0;
		return *this;
	}

JSTR_PRIVATE__

	static constexpr void assert_are_strings() JSTR_NOEXCEPT__ {}

	template <typename Arg, typename... Args>
	JSTR_INLINE__
	static constexpr void assert_are_strings(Arg&& arg, Args&&... args) JSTR_NOEXCEPT__
	{
		static_assert(std::is_same<const char *, std::decay_t<Arg>>::value
			|| std::is_same<char *, std::decay_t<Arg>>::value, "Wrong argument type passed!");
		assert_are_strings(args...);
	}

	template <typename T>
	static constexpr void assert_are_type() JSTR_NOEXCEPT__ {}

	template <typename T, typename Arg, typename... Args>
	JSTR_INLINE__
	static constexpr void assert_are_type(Arg&& arg, Args&&... args)
	{
		static_assert(std::is_same<T, std::decay_t<Arg>>::value, "Wrong argument type passed!");
		assert_are_type<T>(args...);
	}

	static constexpr int args_are_strings() JSTR_NOEXCEPT__ { return 1; }

	template <typename Arg, typename... Args>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	static constexpr int args_are_strings(Arg&& arg, Args&&... args) JSTR_NOEXCEPT__
	{
		return (std::is_same<const char *, std::decay_t<Arg>>::value
		|| std::is_same<const char *, std::decay_t<Arg>>::value)
		&& args_are_strings(args...);
	}

	template <typename T>
	static constexpr int args_are_type() JSTR_NOEXCEPT__ { return 1; }

	template <typename T, typename Arg, typename... Args>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	static constexpr int args_are_type(Arg&& arg, Args&&... args) JSTR_NOEXCEPT__ { return std::is_same<T, std::decay_t<Arg>>::value && args_are_type<T>(args...); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static constexpr std::size_t strlen_args() JSTR_NOEXCEPT__ { return 0; }

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static constexpr std::size_t strlen(const char (&s)[N]) JSTR_NOEXCEPT__ { return N - 1; }
	
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static std::size_t strlen(const char *s) JSTR_NOEXCEPT__ { return std::strlen(s); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static std::size_t strlen(char *s) JSTR_NOEXCEPT__ { return std::strlen(s); }

#	if __cplusplus >= 201703L

	template <typename T, typename... Args>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	size_t strlen_args(T arg, Args&&... args) JSTR_NOEXCEPT__ { return (strlen(arg) + ... + strlen(args)); }

#	else

	template <typename T, typename... Args>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	size_t strlen_args(T s, Args... args) JSTR_NOEXCEPT__ { return strlen(s) + strlen_args(args...); }

#	endif // __cplusplus 17

JSTR_PUBLIC__

	template <typename T, typename U, typename... Args>
	JSTR_INLINE__
	jstring_t(T arg1, U arg2, Args&&... args) JSTR_NOEXCEPT__ { this->cat_new(arg1, arg2, std::forward<Args>(args)...); }

#	if __cplusplus >= 201103L

JSTR_PRIVATE__

	template <typename T, typename... Args>
	JSTR_INLINE__
	void cat_new(T arg, Args&&... args) JSTR_NOEXCEPT__
	{
		static_assert(sizeof...(args), "At least two arguments needed!");
		assert_are_strings(arg, args...);
		const std::size_t arglen_1 = strlen(arg);
		size_t arglen = strlen_args(std::forward<Args>(args)...);
		if (unlikely(!alloc(arglen_1 + arglen)))
			return;
		char *tmp = this->data;
		memcpy(tmp, arg, arglen_1);
		tmp += arglen_1;
		for (auto arg : { std::forward<Args>(args)... })
			while (*arg)
				*tmp++ = *arg++;
		*tmp = '\0';
		this->size = arglen;
	}

	template <typename T, typename... Args>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int cat_impl(T arg, Args&&... args) JSTR_NOEXCEPT__
	{
		static_assert(sizeof...(args), "At least two arguments needed! Use append instead.");
		assert_are_strings(arg, args...);
		const std::size_t arglen_1 = strlen(arg);
		size_t arglen = strlen_args(std::forward<Args>(args)...);
		if (unlikely(!this->reserve_add(arglen_1 + arglen)))
			return 0;
		char *tmp = this->data + this->size;
		memcpy(tmp, arg, arglen_1);
		tmp += arglen_1;
		for (auto arg : { std::forward<Args>(args)... })
			while (*arg)
				*tmp++ = *arg++;
		*tmp = '\0';
		this->size += arglen;
		return 1;
	}

JSTR_PUBLIC__

	template <typename T>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int cat(T arg) JSTR_NOEXCEPT__ { return this->append(std::forward<T>(arg)); }

	template <typename T, typename U, typename... Args>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int cat(T arg1, U arg2, Args&&... args) JSTR_NOEXCEPT__ { return this->cat_impl(arg1, arg2, std::forward<Args>(args)...); }

	template <typename T, typename... Args>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int cat_s(T arg1, T arg2, Args&&... args) JSTR_NOEXCEPT__
	{
		if (unlikely(!this->capacity))
			return 0;
		return this->cat(arg1, arg2, std::forward<Args>(args)...);
	}

#	else

	template <typename T, typename U, typename... Args>
	JSTR_INLINE__
	jstring_t(T arg1, U arg2, Args&&... args) JSTR_NOEXCEPT__
	{
		assert_are_strings(arg1, arg2, args...);
		return private_jstr_cat(this, strlen_args(arg1, arg2, std::forward<Args>(args)...), arg1, arg2, args...);
	}

	template <typename T, typename U, typename... Args>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int cat(T arg1, U arg2, Args&&... args) JSTR_NOEXCEPT__
	{
		assert_are_strings(arg1, arg2, args...);
		return private_jstr_cat(this, strlen_args(arg1, arg2, std::forward<Args>(args)...), arg1, arg2, args...);
	}

	template <typename T, typename U, typename... Args>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int cat_s(T arg1, U arg2, Args&&... args) JSTR_NOEXCEPT__ { return private_jstr_cat_s(this, strlen_args(arg1, arg2, std::forward<Args>(args)...), arg1, arg2, args...); }

#	endif // __cplusplus 11

	template <typename T, typename... Args>
	JSTR_INLINE__
	jstring_t(T arg, Args&&... args) JSTR_NOEXCEPT__ { private_jstr_new_cat(this, strlen_args(arg, std::forward<args>(args)...), arg, args...); }

	JSTR_INLINE__
	jstring_t(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ { private_jstr_new_append_void(this, strlen(s), s); }

	template <std::size_t N>
	JSTR_INLINE__
	jstring_t(const char (&s)[N]) JSTR_NOEXCEPT__ { private_jstr_new_append_void(this, N - 1, s); }

	JSTR_INLINE__
	jstring_t(const char *JSTR_RESTRICT__ s, const std::size_t slen) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_new_append(this, slen, s)))
			return;
		*(this->data + slen) = '\0';
	}

	JSTR_INLINE__
	jstring_t(const std::size_t cap) JSTR_NOEXCEPT__
	{
		this->size = 0;
		private_jstr_new_void(this, cap);
	}

	JSTR_INLINE__
	jstring_t(const std::size_t cap, const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ { private_jstr_constructor_cap(this, cap, s, strlen(s)); }

	JSTR_INLINE__
	jstring_t(const std::size_t cap, const char *JSTR_RESTRICT__ s, const std::size_t slen) JSTR_NOEXCEPT__ { private_jstr_constructor_cap(this, cap, s, slen); }

	template <std::size_t N>
	JSTR_INLINE__
	jstring_t(const std::size_t cap, const char (&s)[N]) JSTR_NOEXCEPT__ { private_jstr_constructor_cap(this, cap, s, N - 1); }

	JSTR_INLINE__
	jstring_t(const std::size_t cap, const std::size_t future_size) JSTR_NOEXCEPT__
	{
		private_jstr_new_void(this, cap);
		if (unlikely(this->data))
			return;
		this->size = future_size;
	}

	JSTR_INLINE__
	jstring_t(const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ { private_jstr_new_append_void(this, other_->size, other_->data); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int alloc() JSTR_NOEXCEPT__ { return jstr_new(this, 8); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int alloc(size_t cap) JSTR_NOEXCEPT__ { return jstr_new(this, cap); }

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int alloc(const char (&s)[N]) JSTR_NOEXCEPT__ { return jstr_new_append(this, N - 1, s); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int alloc(const char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__ { return jstr_new_append(this, slen, s); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int alloc(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ { return jstr_new_append(this, strlen(s), s); }

	JSTR_INLINE__
	void pop_back() JSTR_NOEXCEPT__ { jstr_pop_back(this); }

	JSTR_INLINE__
	void pop_back_s() JSTR_NOEXCEPT__ { jstr_pop_back_s(this); }

	JSTR_INLINE__
	void pop_front() JSTR_NOEXCEPT__ { jstr_pop_front(this); }

	JSTR_INLINE__
	void pop_front_s() JSTR_NOEXCEPT__ { jstr_pop_front_s(this); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int push_front(const char c) JSTR_NOEXCEPT__ { return jstr_push_front(this, c); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int push_front_s(const char c) JSTR_NOEXCEPT__ { return jstr_push_front_s(this, c); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int push_back(const char c) JSTR_NOEXCEPT__ { return jstr_push_back(this, c); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int push_back_s(const char c) JSTR_NOEXCEPT__ { return jstr_push_back_s(this, c); }

	JSTR_INLINE__
	void lower() JSTR_NOEXCEPT__ { jstd_tolower_s(this->data); }

	JSTR_INLINE__
	void upper() JSTR_NOEXCEPT__ { jstd_toupper_s(this->data); }

	JSTR_INLINE__
	void operator-=(const std::size_t size) JSTR_NOEXCEPT__ { this->size -= size; }

	JSTR_INLINE__
	jstring_t operator-(const std::size_t size) JSTR_NOEXCEPT__
	{
		jstring_t tmp;
		tmp.capacity = MAX(JSTR_NEXT_POW2(2 * (this->size - size)), JSTR_MIN_CAP);
		tmp.data = JSTR_CAST__(char *)malloc(tmp.capacity);
		if (likely(tmp.data)) {
			tmp.size = this->size - size;
			memcpy(tmp.data, this->data, tmp.size);
			(tmp.data)[tmp.size] = '\0';
		} else {
			tmp.capacity = 0;
		}
		return tmp;
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *operator[](const std::size_t index) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->data + index; }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char &at(const std::size_t index) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		assert(index <= this->size);
		return *(this->data + index);
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int operator+=(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ { return this->append(s); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int operator+=(const char c) JSTR_NOEXCEPT__ { return this->push_back(c); }

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int operator+=(const char (&s)[N]) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return private_jstr_append(this, s, N - 1); }

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	jstring_t operator+(const char (&s)[N]) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->operator_plus(s, N - 1); }
	
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	jstring_t operator+(const char *JSTR_RESTRICT__ s) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->operator_plus(s, strlen(s)); }

JSTR_PRIVATE__

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	jstring_t operator_plus(const char *JSTR_RESTRICT__ s, const std::size_t slen) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		jstring_t tmp;
		tmp.capacity = MAX(JSTR_NEXT_POW2(2 * (this->size + slen)), JSTR_MIN_CAP);
		tmp.data = JSTR_CAST__(char *)malloc(tmp.capacity);
		if (likely(tmp.data)) {
			memcpy(tmp.data, this->data, this->size);
			memcpy(tmp.data + this->size, s, slen + 1);
			tmp.size = this->size + slen;
		} else {
			tmp.capacity = 0;
		}
		return tmp;
	}

JSTR_PUBLIC__

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int operator=(const char (&s)[N]) JSTR_NOEXCEPT__ { return this->assign(s); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int operator=(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ { return this->assign(s); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int operator=(const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ { return this->assign(other_); }
	
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int append(const char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__
	{
		if (!private_jstr_append(this, s, slen))
			return 0;
		*(this->data + slen) = '\0';
		return 1;
	}

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int append(const char (&s)[N]) JSTR_NOEXCEPT__ { return private_jstr_append(this, s, N - 1); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int append(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ { return private_jstr_append(this, s, strlen(s)); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int append_s(const char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__
	{
		if (!private_jstr_append_s(this, s, slen))
			return 0;
		*(this->data + slen) = '\0';
		return 1;
	}

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int append_s(const char (&s)[N]) JSTR_NOEXCEPT__ { return private_jstr_append_s(this, s, N - 1); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int append_s(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ { return private_jstr_append_s(this, s, strlen(s)); }

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int str(const char (&s)[N]) JSTR_NOEXCEPT__ { return private_jstr_str(this, s, N - 1); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *str(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ { return private_jstr_str(this, s, strlen(s)); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *chr(int c) JSTR_NOEXCEPT__ { return jstr_chr(this, c); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *rchr(int c) JSTR_NOEXCEPT__ { return jstr_rchr(this, c); }

	JSTR_INLINE__ void swap(jstring_t *other_) JSTR_NOEXCEPT__ { jstr_swap(this, other_); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int reserve(const std::size_t cap) JSTR_NOEXCEPT__ { return jstr_reserve(this, cap); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int reserve_add(const std::size_t add_cap) JSTR_NOEXCEPT__ { return jstr_reserve(this, this->capacity + add_cap); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int assign(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ { return private_jstr_assign(this, s, strlen(s)); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int assign(const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ { return jstr_assign_jstr(this, other_); }

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int assign(const char (&s)[N]) JSTR_NOEXCEPT__ { return private_jstr_assign(this, s, N - 1); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int shrink_to_fit() JSTR_NOEXCEPT__ { return jstr_shrink_to_fit(this); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int cmp(const jstring_t *JSTR_RESTRICT__ other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return jstr_cmp(this, other_); }

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int cmp(const char (&s)[N]) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return jstr_cmp_str(this, s, N - 1); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int cmp(const char *JSTR_RESTRICT__ s) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return jstr_cmp_str(this, s, MIN(strlen(s), this->size)); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int casecmp(const jstring_t *JSTR_RESTRICT__ other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return jstr_casecmp_str(this, other_->data); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int casecmp(const char *JSTR_RESTRICT__ s) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return jstr_casecmp_str(this, s); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int count_c(const int c) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return jstd_count_c(this->data, c); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int count_s(const char *JSTR_RESTRICT__ needle) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
#ifdef JSTR_HAS_MEMMEM__
	{ return jstd_count_s(this->data, this->size, needle, strlen(needle)); }
#else
	{ return jstd_count_s(this->data, needle); }
#endif // JSTR_HAS_MEMMEM__

#ifdef JSTR_HAS_MEMMEM__
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int count_s(const char *JSTR_RESTRICT__ needle, std::size_t needlelen) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return jstd_count_s(this->data, this->size, needle, needlelen); }
#endif // JSTR_HAS_MEMMEM__

	template <std::size_t N>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int count_s(const char (&needle)[N]) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
#ifdef JSTR_HAS_MEMMEM__
		{ return jstd_count_s(this->data, this->size, needle, N - 1); }
#else
		{ return jstd_count_s(this->data, needle); }
#endif // JSTR_HAS_MEMMEM__

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int count_s(const jstring_t *JSTR_RESTRICT__ needle) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
#ifdef JSTR_HAS_MEMMEM__
		{ return jstd_count_s(this->data, this->size, needle->data, needle->size); }
#else
		{ return jstd_count_s(this->data, needle); }
#endif // JSTR_HAS_MEMMEM__

#endif // __cplusplus templates

JSTR_PRIVATE__

#ifdef __cplusplus

	JSTR_INLINE__
	void private_jstr_constructor_cap(jstring_t *JSTR_RESTRICT__ this_,
					const std::size_t cap,
					const char *JSTR_RESTRICT__ const s,
					const std::size_t slen) JSTR_NOEXCEPT__
	{
		this_->capacity = MAX(JSTR_NEXT_POW2(cap), JSTR_MIN_CAP);
		this_->data = (char *)std::malloc(this_->capacity);
		if (unlikely(!this_->data)) {
			this_->capacity = 0;
			this_->size = 0;
			return;
		}
		std::memcpy(this_->data, s, slen + 1);
		*(this_->data + slen) = '\0';
		this_->size = slen;
	}

	JSTR_INLINE__
	void private_jstr_new_append_void(jstring_t *JSTR_RESTRICT__ this_,
					const size_t slen,
					const char *JSTR_RESTRICT__ const s,
					...) JSTR_NOEXCEPT__
	{
		this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * slen));
		this_->data = JSTR_CAST__(char *)malloc(this_->capacity);
		if (unlikely(!this_->data)) {
			jstr_init(this_);
			return;
		}
		this_->size = slen;
		memcpy(this_->data, s, slen + 1);
	}

	JSTR_INLINE__
	void private_jstr_new_void(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__
	{
		this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * size));
		this_->data = JSTR_CAST__(char *)malloc(this_->capacity);
		if (unlikely(!this_->data)) {
			this_->capacity = 0;
			this_->data = NULL;
			return;
		}
	}

#endif // __cpluslus

	JSTR_INLINE__
	int jstr_reserve_f_exact(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__
	{
		char *JSTR_RESTRICT__ tmp;
		if (unlikely(!(tmp = JSTR_CAST__(char *)realloc(this_->data, cap))))
			return 0;
		this_->data = tmp;
		this_->capacity = cap;
		return 1;
	}

	JSTR_INLINE__
	int jstr_reserve_f(jstring_t *JSTR_RESTRICT__ this_, size_t cap, ...) JSTR_NOEXCEPT__
	{
		size_t tmp_cap = this_->capacity * JSTR_MULTIPLIER;
		while (tmp_cap < cap)
			tmp_cap *= 2;
		char *JSTR_RESTRICT__ tmp;
		if (unlikely(!(tmp = JSTR_CAST__(char *)realloc(this_->data, tmp_cap))))
			return 0;
		this_->data = tmp;
		this_->capacity = tmp_cap;
		return 1;
	}

	JSTR_INLINE__
	int jstr_reserve(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__
	{
		if (cap > this_->capacity)
			return jstr_reserve_f(this_, cap);
		return 1;
	}

	int private_jstr_cat(jstring_t *JSTR_RESTRICT__ this_, size_t len, ...) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_reserve(this_, this_->size + len + 1)))
			return 0;
		char *JSTR_RESTRICT__ tmp = this_->data + len;
		this_->size += len;
		va_list ap;
		va_start(ap, len);
		for (const char *JSTR_RESTRICT__ argv = va_arg(ap, const char *);
				argv;
				argv = va_arg(ap, const char *))
			do {
				*tmp++ = *argv++;
			} while (*argv);
		*tmp = '\0';
		va_end(ap);
		return 1;
	}

	int private_jstr_cat_s(jstring_t *JSTR_RESTRICT__ this_, size_t len, ...) JSTR_NOEXCEPT__
	{
		if (unlikely(!this_->capacity))
			return 0;
		if (unlikely(!jstr_reserve(this_, this_->size + len + 1)))
			return 0;
		char *JSTR_RESTRICT__ tmp = this_->data + len;
		this_->size += len;
		va_list ap;
		va_start(ap, len);
		for (const char *JSTR_RESTRICT__ argv = va_arg(ap, const char *);
				argv;
				argv = va_arg(ap, const char *))
			do {
				*tmp++ = *argv++;
			} while (*argv);
		*tmp = '\0';
		va_end(ap);
		return 1;
	}

	JSTR_INLINE__
	void private_jstr_append_u(jstring_t *JSTR_RESTRICT__ this_,
				const char *JSTR_RESTRICT__ const s,
				size_t slen) JSTR_NOEXCEPT__
	{
		memcpy(this_->data + this_->size, s, slen + 1);
		this_->size = this_->size + slen;
	}

	JSTR_INLINE__
	int private_jstr_append(jstring_t *JSTR_RESTRICT__ this_,
				const char *JSTR_RESTRICT__ const s,
				size_t slen) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_reserve(this_, this_->size + slen)))
			return 0;
		private_jstr_append_u(this_, s, slen);
		return 1;
	}

	JSTR_INLINE__
	int private_jstr_append_s(jstring_t *JSTR_RESTRICT__ this_,
				const char *JSTR_RESTRICT__ const s,
				size_t slen) JSTR_NOEXCEPT__
	{
		if (unlikely(!this_->capacity))
			return 0;
		return private_jstr_append(this_, s, slen);
	}

#ifdef __cplusplus

JSTR_INLINE__
int jstr_new_append(jstring_t *JSTR_RESTRICT__ this_,
		const size_t slen,
		const char *JSTR_RESTRICT__ const s,
		...) JSTR_NOEXCEPT__
{
	this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * slen));
	this_->data = JSTR_CAST__(char *)malloc(this_->capacity);
	if (unlikely(!this_->data)) {
		jstr_init(this_);
		return 0;
	}
	this_->size = slen;
	memcpy(this_->data, s, slen + 1);
	return 1;
}

int private_jstr_new_cat(jstring_t *JSTR_RESTRICT__ this_,
			const size_t arglen,
			...) JSTR_NOEXCEPT__
{
	this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * arglen));
	this_->data = JSTR_CAST__(char *)malloc(this_->capacity);
	if (unlikely(!this_->data)) {
		jstr_init(this_);
		return 0;
	}
	this_->size = arglen;
	char *tmp = this_->data;
	va_list ap;
	va_start(ap, arglen);
	for (const char *JSTR_RESTRICT__ argv = va_arg(ap, const char *);
			argv;
			argv = va_arg(ap, const char *))
		do {
			*tmp++ = *argv++;
		} while (*argv);
	*tmp = '\0';
	va_end(ap);
	return 1;
}

JSTR_INLINE__
int jstr_new(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__
{
	this_->size = 0;
	this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * size));
	this_->data = JSTR_CAST__(char *)malloc(this_->capacity * sizeof(*this_->data));
	if (unlikely(!this_->data)) {
		this_->data = NULL;
		this_->capacity = 0;
		return 0;
	}
	return 1;
}

#else

#define jstr_new(this_, size_)                                                                       \
do {                                                                                                 \
	((this_)->size) = 0;                                                                         \
	((this_)->capacity) = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * size_));                          \
	((this_)->data) = JSTR_CAST__(char *)malloc(((this_)->capacity) * sizeof(*((this_)->data))); \
	if (unlikely(!((this_)->data)))                                                              \
		((this_)->capacity) = 0;                                                             \
} while (0)

#define jstr_new_cat(this_, ...)                                                                 \
do {                                                                                             \
	((this_)->size) = (PP_STRLEN_VA_ARGS(__VA_ARGS__));                                      \
	((this_)->capacity) = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * ((this_)->size)));            \
	((this_)->data) = malloc(((this_)->capacity) * sizeof(*((this_)->data)));                \
	if (likely(((this_)->data))) {                                                           \
		if (PP_NARG(__VA_ARGS__) == 1) {                                                 \
			memcpy(((this_)->data), PP_FIRST_ARG(__VA_ARGS__), ((this_)->size) + 1); \
		} else {                                                                         \
			size_t i = 0;                                                            \
			const char *args[] = { __VA_ARGS__, NULL };                              \
			for (const char **JSTR_RESTRICT__ argsp = args; *argsp; ++argsp)         \
				while (**argsp)                                                  \
					((this_)->data)[i++] = *(*argsp)++;                      \
			((this_)->data)[i] = '\0';                                               \
		}                                                                                \
	} else {                                                                                 \
		((this_)->capacity) = 0;                                                         \
		((this_)->size) = 0;                                                             \
	}                                                                                        \
} while (0)

#define jstr_cat(this_, ...)                                                                                       \
do {                                                                                                               \
	((this_)->size) += (PP_STRLEN_VA_ARGS(__VA_ARGS__));                                                       \
	((this_)->capacity) = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * ((this_)->size)));                              \
	((this_)->data) = realloc(((this_)->data), ((this_)->capacity) * sizeof(*((this_)->data)));                \
	if (unlikely(!((this_)->data))) {                                                                          \
		((this_)->capacity) = 0;                                                                           \
		((this_)->size) = 0;                                                                               \
	} else {                                                                                                   \
		if (PP_NARG(__VA_ARGS__) == 1) {                                                                   \
			memcpy(((this_)->data) + ((this_)->size), PP_FIRST_ARG(__VA_ARGS__), ((this_)->size) + 1); \
		} else {                                                                                           \
			size_t i = ((this_)->size);                                                                \
			const char *args[] = { __VA_ARGS__, NULL };                                                \
			for (const char **JSTR_RESTRICT__ argsp = args; *argsp; ++argsp)                           \
				while (**argsp)                                                                    \
					((this_)->data)[i++] = *(*argsp)++;                                        \
			((this_)->data)[i] = '\0';                                                                 \
		}                                                                                                  \
	}                                                                                                          \
} while (0)

#endif // __cpluslus

	JSTR_INLINE__
	void jstr_swap(jstring_t *JSTR_RESTRICT__ this_, jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__
	{
		const jstring_t tmp = *other_;
		*other_ = *this_;
		*this_ = tmp;
	}
	JSTR_INLINE__
	void jstr_swap_str(jstring_t *JSTR_RESTRICT__ this_,
			char **JSTR_RESTRICT__ other_,
			size_t *JSTR_RESTRICT__ other_len,
			size_t *JSTR_RESTRICT__ other_capacity) JSTR_NOEXCEPT__
	{
		char *JSTR_RESTRICT__ const tmp_other_ = *other_;
		const size_t tmp_other_len = *other_len;
		const size_t tmp_other_cap = *other_capacity;
		*other_ = this_->data;
		*other_len = this_->size;
		*other_capacity = this_->capacity;
		this_->data = tmp_other_;
		this_->size = tmp_other_len;
		this_->capacity = tmp_other_cap;
	}

	JSTR_INLINE__
	int jstr_cmp_f(const jstring_t *JSTR_RESTRICT__ this_,
			const jstring_t *JSTR_RESTRICT__ const other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{ return memcmp(this_->data, other_->data, this_->size); }

	JSTR_INLINE__
	int jstr_cmp(const jstring_t *JSTR_RESTRICT__ this_,
			const jstring_t *JSTR_RESTRICT__ const other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		if (this_->size == other_->size)
			return jstr_cmp_f(this_, other_);
		return 1;
	}

	JSTR_INLINE__
	int jstr_cmp_str(const jstring_t *JSTR_RESTRICT__ this_,
			const char *JSTR_RESTRICT__ const s,
			size_t slen) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		if (this_->size == slen)
			return memcmp(this_, s, slen);
		return 1;
	}


	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int jstr_casecmp_f(const jstring_t *JSTR_RESTRICT__ const this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return jstd_casecmp(this_->data, other_->data); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int jstr_casecmp_str(const jstring_t *JSTR_RESTRICT__ const this_, const char *JSTR_RESTRICT__ s) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return jstd_casecmp(this_->data, s); }

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int jstr_casecmp_jstr(const jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ const other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{ return (this_->size == other_->size) ? jstd_casecmp(this_->data, other_->data) : 1; }

	JSTR_INLINE__
	void jstr_assign_u(jstring_t *JSTR_RESTRICT__ dest,
			const char *JSTR_RESTRICT__ src,
			size_t srclen) JSTR_NOEXCEPT__
	{
		memcpy(dest->data, src, srclen + 1);
		dest->size = srclen;
	}

	JSTR_INLINE__
	void jstr_assign_jstr_u(jstring_t *JSTR_RESTRICT__ dest,
				const jstring_t *JSTR_RESTRICT__ const src) JSTR_NOEXCEPT__
	{
		memcpy(dest->data, src->data, src->size + 1);
		dest->size = src->size;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_assign_f(jstring_t *JSTR_RESTRICT__ dest,
			const char *JSTR_RESTRICT__ src,
			size_t srclen) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_reserve_f(dest, srclen)))
			return 0;
		jstr_assign_u(dest, src, srclen);
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_assign_jstr_f(jstring_t *JSTR_RESTRICT__ dest,
			const jstring_t *JSTR_RESTRICT__ const src) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_reserve_f(dest, src->size)))
			return 0;
		jstr_assign_jstr_u(dest, src);
		return 1;
	}

	JSTR_INLINE__
	int private_jstr_assign(jstring_t *JSTR_RESTRICT__ dest,
			const char *JSTR_RESTRICT__ src,
			const size_t srclen,
			...) JSTR_NOEXCEPT__
	{
		if (dest->capacity < srclen)
			return jstr_assign_f(dest, src, srclen);
		jstr_assign_u(dest, src, srclen);
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_assign_jstr(jstring_t *JSTR_RESTRICT__ dest,
			const jstring_t *JSTR_RESTRICT__ const src,
			...) JSTR_NOEXCEPT__
	{
		if (dest->capacity < src->capacity)
			return jstr_assign_f(dest, src->data, src->capacity);
		jstr_assign_jstr_u(dest, src);
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_reserve_s(jstring_t *JSTR_RESTRICT__ this_,
			size_t cap) JSTR_NOEXCEPT__
	{
		if (unlikely(!this_->capacity))
			return 0;
		return jstr_reserve(this_, cap);
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_shrink_to_fit_f(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__
	{
		char *JSTR_RESTRICT__ tmp;
		if (unlikely(!(tmp = JSTR_CAST__(char *)realloc(this_->data, this_->size + 1))))
			return 0;
		this_->capacity = this_->size + 1;
		this_->data = tmp;
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_shrink_to_fit(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__
	{
		if (unlikely(this_->capacity == this_->size))
			return 1;
		return jstr_shrink_to_fit_f(this_);
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_shrink_to_f(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__
	{
		char *JSTR_RESTRICT__ tmp;
		if (unlikely(!(tmp = JSTR_CAST__(char *)realloc(this_->data, cap))))
			return 0;
		this_->data = tmp;
		this_->data[this_->size = cap] = '\0';
		this_->capacity = cap;
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_shrink_to(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__
	{
		if (cap > this_->capacity)
			return 1;
		return jstr_shrink_to_f(this_, cap);
	}

	JSTR_INLINE__
	void jstr_shrink_to_size_f(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__
	{
		this_->data[this_->size = size] = '\0';
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_shrink_to_size(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__
	{
		if (size < this_->size) {
			jstr_shrink_to_size_f(this_, size);
			return 1;
		}
		return 0;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_shrink_to_size_s(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__
	{
		if ((!!this_->size) & (size < this_->size)) {
			jstr_shrink_to_size_f(this_, size);
			return 1;
		}
		return 0;
	}

	JSTR_INLINE__
	void jstr_push_back_u(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		this_->data[this_->size] = c;
		this_->data[++this_->size] = '\0';
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_push_back_f(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_reserve_f(this_, this_->size * JSTR_MULTIPLIER)))
			return 0;
		jstr_push_back_u(this_, c);
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_push_back(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		if (unlikely(this_->size == this_->capacity))
			return jstr_push_back_f(this_, c);
		jstr_push_back_u(this_, c);
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_push_back_s(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		if (unlikely(!this_->capacity))
			return 0;
		return jstr_push_back(this_, c);
	}

	JSTR_INLINE__
	void jstr_pop_back(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__
	{
		this_->data[--this_->size] = '\0';
	}

	JSTR_INLINE__
	void jstr_pop_back_s(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__
	{
		if (unlikely(!this_->size))
			return;
		jstr_pop_back(this_);
	}

	JSTR_INLINE__
	void jstr_push_front_u(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		memmove(this_->data + 1, this_->data, ++this_->size);
		*this_->data = c;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_push_front_f(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		if (unlikely(!jstr_reserve_f(this_, this_->size * JSTR_MULTIPLIER)))
			return 0;
		jstr_push_front_u(this_, c);
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_push_front(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		if (unlikely(this_->capacity == this_->size))
			return jstr_push_front_f(this_, c);
		jstr_push_front_u(this_, c);
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_push_front_s(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		if (unlikely(!this_->capacity))
			return 0;
		return jstr_push_front(this_, c);
	}

	JSTR_INLINE__
	void jstr_pop_front(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__ { memmove(this_->data, this_->data + 1, this_->size--); }

	JSTR_INLINE__
	void jstr_pop_front_s(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__
	{
		if (unlikely(!this_->size))
			return;
		memmove(this_->data, this_->data + 1, this_->size--);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
#ifdef JSTR_HAS_MEMMEM__
	char *private_jstr_str(jstring_t *haystack, const char *JSTR_RESTRICT__ needle, size_t needlelen, ...) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return JSTR_CAST__(char *)memmem(haystack->data, haystack->size, needle, needlelen); }
#else
	char *jstr_str(jstring_t *haystack, const char *JSTR_RESTRICT__ needle) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return strstr(haystack->data, needle); }
#endif // JSTR_HAS_MEMMEM__

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *jstr_rchr(const jstring_t *JSTR_RESTRICT__ this_, int c) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
#ifdef JSTR_HAS_MEMRCHR__
	{ return JSTR_CAST__(char *)memrchr(this_->data, c, this_->size); }
#else
	{ return jstd_rchr(this_->data, c, this_->size); }
#endif // JSTR_HAS_MEMRCHR__

	JSTR_INLINE__
	void jstr_rev(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__
	{
		char *JSTR_RESTRICT__ begin = this_->data;
		char *JSTR_RESTRICT__ end = this_->data + this_->size - 1;
		for (char tmp_begin; begin < end; ++begin, --end) {
			tmp_begin = *begin;
			*begin = *end;
			*end = tmp_begin;
		}
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_rev_dup(jstring_t *JSTR_RESTRICT__ src, char **JSTR_RESTRICT__ dest) JSTR_NOEXCEPT__
	{
		*dest = JSTR_CAST__(char *)malloc(src->size + 1);
		if (unlikely(!*dest))
			return 0;
		char *JSTR_RESTRICT__ const begin = src->data - 1;
		char *JSTR_RESTRICT__ end = src->data + src->size - 1;
		while (begin < end)
			**dest++ = *end--;
		**dest = '\0';
		return 1;
	}

	JSTR_INLINE__
	char *jstr_chr(const jstring_t *JSTR_RESTRICT__ this_, int c) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return JSTR_CAST__(char *)memchr(this_->data, c, this_->size); }

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_dup(jstring_t *JSTR_RESTRICT__ this_, jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__
	{
		other_->data = JSTR_CAST__(char *)malloc(this_->capacity);
		if (unlikely(!other_->data))
			return 0;
		memcpy(other_->data, this_->data, this_->size + 1);
		other_->capacity = this_->capacity;
		other_->size = this_->size;
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_dup_s(jstring_t *JSTR_RESTRICT__ this_, jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__
	{
		if (unlikely(!this_->size))
			return 0;
		return jstr_dup(this_, other_);
	}

	JSTR_INLINE__
	static int impl_jstr_ndup(jstring_t *JSTR_RESTRICT__ this_,
			jstring_t *JSTR_RESTRICT__ other_,
			size_t n) JSTR_NOEXCEPT__
	{
		other_->data = JSTR_CAST__(char *)malloc(n + 1);
		if (unlikely(!other_->data))
			return 0;
		memcpy(other_->data, this_->data, n + 1);
		*(other_->data + n) = '\0';
		other_->capacity = n + 1;
		other_->size = n;
		return 1;
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_ndup(jstring_t *JSTR_RESTRICT__ this_,
			jstring_t *JSTR_RESTRICT__ other_,
			size_t n) JSTR_NOEXCEPT__
	{
		if (n > this_->size)
			return 0;
		return impl_jstr_ndup(this_, other_, n);
	}

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	int jstr_ndup_s(jstring_t *JSTR_RESTRICT__ this_,
			jstring_t *JSTR_RESTRICT__ other_,
			size_t n) JSTR_NOEXCEPT__
	{
		if ((!this_->size)
		| (n > this_->size))
			return 0;
		return impl_jstr_ndup(this_, other_, n);
	}

#ifndef __cplusplus
	JSTR_INLINE__
	JSTR_CONST__
	char *jstr_at(const jstring_t *this_jstr, const size_t index) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		assert(index <= this_jstr->size);
		return this_jstr->data + index;
	}
#endif // ! __cpluslus

#ifdef __cplusplus

} jstring_t;

#endif // __cplusplus end of struct

#ifndef __cplusplus

#define jstr_assign(dest, ...)                                                                                 \
	PP_NARG(__VA_ARGS__) == 2                                                                              \
		? private_jstr_assign(this_jstr, __VA_ARGS__, 0)                                               \
		: private_jstr_assign(this_jstr, PP_FIRST_ARG(__VA_ARGS__), strlen(PP_FIRST_ARG(__VA_ARGS__))) \

#define jstr_append(this_jstr, ...)                                                                            \
(                                                                                                              \
	PP_NARG(__VA_ARGS__) == 2                                                                              \
		? private_jstr_append(this_jstr, __VA_ARGS__, 0)                                               \
		: private_jstr_append(this_jstr, PP_FIRST_ARG(__VA_ARGS__), strlen(PP_FIRST_ARG(__VA_ARGS__))) \
)

#define private_jstr_reserve_x(this_jstr, multiplier)                      \
 jstr_reserve_f_exact(this_jstr, ((multiplier) * ((this_jstr)->capacity)))

#define jstr_reserve_2x(this_jstr) private_jstr_reserve_x(this_jstr, 2)
#define jstr_reserve_4x(this_jstr) private_jstr_reserve_x(this_jstr, 4)
#define jstr_reserve_8x(this_jstr) private_jstr_reserve_x(this_jstr, 8)
#define jstr_reserve_16x(this_jstr) private_jstr_reserve_x(this_jstr, 16)
#define jstr_reserve_32x(this_jstr) private_jstr_reserve_x(this_jstr, 32)
#define jstr_reserve_64x(this_jstr) private_jstr_reserve_x(this_jstr, 64)

#define private_jstr_reserve_s_x(this_jstr, multiplier)                      \
 jstr_reserve_s_f_exact(this_jstr, ((multiplier) * ((this_jstr)->capacity)))

#define jstr_reserve_s_2x(this_jstr) private_jstr_reserve_s_x(this_jstr, 2)
#define jstr_reserve_s_4x(this_jstr) private_jstr_reserve_s_x(this_jstr, 4)
#define jstr_reserve_s_8x(this_jstr) private_jstr_reserve_s_x(this_jstr, 8)
#define jstr_reserve_s_16x(this_jstr) private_jstr_reserve_s_x(this_jstr, 16)
#define jstr_reserve_s_32x(this_jstr) private_jstr_reserve_s_x(this_jstr, 32)
#define jstr_reserve_s_64x(this_jstr) private_jstr_reserve_s_x(this_jstr, 64)

#define jstr_foreach(elem, jstr)                          \
 	for (char *elem = ((jstr)->data); *elem; ++elem)
#define jstr_begin(this_jstr) ((this_jstr)->data)
#define jstr_end(this_jstr) (((this_jstr)->data) + ((this_jstr)->size))

#define jstr_cbegin(this_jstr) ((const char *)((this_jstr)->data))
#define jstr_cend(this_jstr) ((const char *)(((this_jstr)->data) + ((this_jstr)->size)))

#define jstr_foreach_index(elem, jstr)                 \
 for (size_t i = 0, end__ = jstr.size; i < end__; ++i)

#endif // ! __cplusplus : c macros

#endif // JSTR_H_DEF__
