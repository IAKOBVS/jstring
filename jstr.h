#ifndef JSTR_H_DEF__
#define JSTR_H_DEF__

/* rename to the right path */

#define JSTR_PATH_TO_PP_VA_ARGS_MACROS_H "/home/james/c/pp_macros/pp_va_args_macros.h"

/*
   you are expected to error-check memory allocation
   by checking for NULL in data member after every function
   except those that do not require require allocation;
   e.g., pop_back, pop_front.

   at() will do bounds checking by asserting that index <= this->size.
   it will crash at runtime if assertion fails.

   _u (unsafe) functions will not do any allocations.

   _f (force/fast) functions will force allocations.
*/

#define JSTR_64_BIT
#define JSTR_ALIGN_POWER_OF_TWO

#include "macros.h"
#include "jstd.h"

#ifndef __cplusplus
#	include <stdarg.h>
#	include <stdlib.h>
#	include <string.h>
#	include <assert.h>
#endif // ! __cplusplus

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
#	include "jtraits.hpp"
#endif // __cplusplus

#define JSTR_MIN_CAP 8
#define JSTR_MULTIPLIER 2

#ifdef JSTR_HAS_STATIC_ASSERT
#	define JSTR_IS_STR_VA_ARGS(...) PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);
#else
#	define JSTR_IS_STR_VA_ARGS(...)
#endif // JSTR_HAS_STATIC_ASSERT

#define JSTR_GET_MALLOC_SIZE(x) (((x) > 4) ? JSTR_NEXT_POW2(JSTR_MULTIPLIER * (x)) : JSTR_MIN_CAP)

#define jstr_init(this_)         \
do {                             \
	((this_)->data) = NULL;  \
	((this_)->capacity) = 0; \
	((this_)->size) = 0;     \
} while (0)

#define jstr_dealloc(this_)      \
do {                             \
	free(((this_)->data));   \
	((this_)->data) = NULL;  \
	((this_)->capacity) = 0; \
	((this_)->size) = 0;     \
} while (0)

typedef struct jstring_t jstring_t;

#ifndef __cplusplus // ! __cplusplus

#define jstring(name) jstring_t name = {0}

void jstr_push_back_u(jstring_t *this_, const char c) JSTR_NOEXCEPT__;
void jstr_push_back_f(jstring_t *this_, const char c) JSTR_NOEXCEPT__;
void jstr_push_back(jstring_t *this_, const char c) JSTR_NOEXCEPT__;

void jstr_push_front_u(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__;
void jstr_push_front_f(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__;
void jstr_push_front(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__;

void jstr_pop_back(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__;

void jstr_pop_front(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__;

void private_jstr_append_u(jstring_t *this_, const char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__;
void private_jstr_append(jstring_t *this_, const char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__;

void jstr_swap(jstring_t *JSTR_RESTRICT__ this_, jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__;
void jstr_swap_str(jstring_t *JSTR_RESTRICT__ this_, char **JSTR_RESTRICT__ other_, size_t *other_len, size_t *other_cap) JSTR_NOEXCEPT__;

void jstr_shrink_to_fit_f(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__;
void jstr_shrink_to_fit(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__;

void jstr_shrink_to_f(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__;
void jstr_shrink_to(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__;

void jstr_shrink_to_size_f(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__;
void jstr_shrink_to_size(jstring_t *JSTR_RESTRICT__ this_, size_t size) JSTR_NOEXCEPT__;

void jstr_reserve(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__;
void jstr_reserve_f(jstring_t *JSTR_RESTRICT__ this_, size_t cap, ...) JSTR_NOEXCEPT__;

void private_jstr_assign(jstring_t *JSTR_RESTRICT__ this_, const char *JSTR_RESTRICT__ s, size_t slen, ...) JSTR_NOEXCEPT__;
void jstr_assign_u(jstring_t *JSTR_RESTRICT__ this_, const char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__;
void jstr_assign_f(jstring_t *JSTR_RESTRICT__ this_, const char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__;

void jstr_assign_jstr_u(jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__;
void jstr_assign_jstr_f(jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__;
void jstr_assign_jstr(jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ s, ...) JSTR_NOEXCEPT__;

/* if equals returns 0 */
int jstr_cmp(const jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_cmp_f(const jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_cmp_str(const jstring_t *JSTR_RESTRICT__ this_, const char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

int jstr_casecmp_f(const jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_casecmp_jstr(const jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;
int jstr_casecmp_str(const jstring_t *JSTR_RESTRICT__ this_, const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__ JSTR_WARN_UNUSED__;

/* memrchr */
char *jstr_rchr(const jstring_t *JSTR_RESTRICT__ this_, const int c) JSTR_NOEXCEPT__;

#ifdef JSTR_HAS_MEMMEM__
#	define jstr_str(this_, ...)                                                                             \
		(PP_NARG(__VA_ARGS__) == 1)                                                                     \
			? private_jstr_str(this_, PP_FIRST_ARG(__VA_ARGS__), strlen(PP_FIRST_ARG(__VA_ARGS__))) \
			: private_jstr_str(this_, __VA_ARGS__, 0)
char *private_jstr_str(const jstring_t *haystack, const char *JSTR_RESTRICT__ needle, size_t needlelen, ...) JSTR_NOEXCEPT__;
#else
char *jstr_str(const jstring_t *haystack, const char *JSTR_RESTRICT__ needle) JSTR_NOEXCEPT__;
#endif // JSTR_HAS_MEMMEM__

#endif // ! __cplusplus

typedef struct jstring_t {
#ifndef __cplusplus // ! __cpluslus
	size_t size; 
	size_t capacity;
	char *data;
} jstring_t;
#else // __cplusplus
	typedef size_t size_t;

	size_t size;
	size_t capacity;
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
	void dealloc() JSTR_NOEXCEPT__ { jstr_dealloc(this); }

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
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char& at(const size_t index) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{  
		assert(index <= this->size);
		return *(this->data + this->size);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *operator[](const size_t index) JSTR_CPP_CONST__ JSTR_NOEXCEPT__ { return this->data + index; }

	JSTR_INLINE__
	jstring_t(const jstring_t& other_) JSTR_NOEXCEPT__
	{
		this->size = other_.size;
		if (!this->capacity)
			this->data = (char *)std::malloc(other_.capacity);
		else if (this->capacity < other_.capacity)
			this->data = (char *)std::realloc(this->data, other_.capacity);
		if (unlikely(!this->data)) {
			this->size = 0;
			return;
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
			if (unlikely(!this->data)) {
				this->size = 0;
				return *this;
			}
		} else if (this->capacity < other_.capacity) {
			this->data = (char *)std::realloc(this->data, other_.capacity);
			if (unlikely(!this->data)) {
				this->size = 0;
				return *this;
			}
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

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static constexpr size_t strlen_args() JSTR_NOEXCEPT__
	{
		return 0;
	}

	template <typename Str, typename... StrArgs,
		typename = typename std::enable_if<jstd::are_strings<Str, StrArgs...>(), int>::type>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static size_t strlen_args(Str&& s, StrArgs&&... args) JSTR_NOEXCEPT__
	{
		return strlen(std::forward<Str>(s)) + strlen_args(std::forward<StrArgs>(args)...);
	}

	template <size_t N>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static constexpr size_t strlen(const char (&)[N]) JSTR_NOEXCEPT__
	{
		return N - 1;
	}
	
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static size_t strlen(jstring_t& s) JSTR_NOEXCEPT__
	{
		return s.size;
	}
	
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static size_t strlen(jstring_t&& s) JSTR_NOEXCEPT__
	{
		return s.size;
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static size_t strlen(jstring_t *s) JSTR_NOEXCEPT__
	{
		return s->size;
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static size_t strlen(Str&& s) JSTR_NOEXCEPT__
	{
		return std::strlen(strdata(std::forward<Str>(s)));
	}
	
JSTR_PUBLIC__

	template <typename Str, typename OtherStr, typename... StrArgs,
		typename = typename std::enable_if<jstd::are_strings<Str, OtherStr, StrArgs...>(), int>::type>
	JSTR_INLINE__
	jstring_t(Str&& arg1, OtherStr&& arg2, StrArgs&&... args) JSTR_NOEXCEPT__
	{
		this->cat_alloc(std::forward<Str>(arg1), std::forward<OtherStr>(arg2), std::forward<StrArgs>(args)...);
	}

#	if __cplusplus >= 201103L

JSTR_PRIVATE__

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	void cat_assign(char **destp, Str&& s) JSTR_NOEXCEPT__
	{
		while (*s)
			*(*destp)++ = *s++;
	}

	template <size_t N>
	JSTR_INLINE__
	void cat_assign(char **destp, const char (&s)[N]) JSTR_NOEXCEPT__
	{
		memcpy(*destp, s, N - 1);
		*destp += (N - 1);
	}
	
	JSTR_INLINE__
	void cat_assign(char **destp, jstring_t *s) JSTR_NOEXCEPT__
	{
		memcpy(*destp, s->data, s->size);
		*destp += s->size;
	}
	
	JSTR_INLINE__
	void cat_assign(char **destp, jstring_t& s) JSTR_NOEXCEPT__
	{
		memcpy(*destp, s.data, s.size);
		*destp += s.size;
	}
	
	JSTR_INLINE__
	void cat_assign(char **destp, jstring_t&& s) JSTR_NOEXCEPT__
	{
		memcpy(*destp, s.data, s.size);
		*destp += s.size;
	}

	JSTR_INLINE__
	void cat_loop_assign(char **) JSTR_NOEXCEPT__ {}

	template <typename Str, typename... StrArgs,
		typename = typename std::enable_if<jstd::are_strings<Str, StrArgs...>(), int>::type>
	JSTR_INLINE__
	void cat_loop_assign(char **destp, Str&& arg, StrArgs&&... args) JSTR_NOEXCEPT__
	{
		cat_assign(destp, std::forward<Str>(arg));
		cat_loop_assign(destp, std::forward<StrArgs>(args)...);
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static Str&& strdata(Str&& s) JSTR_NOEXCEPT__
	{
		return s;
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static char *strdata(jstring_t *s) JSTR_NOEXCEPT__
	{
		return s->data;
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static char *strdata(jstring_t& s) JSTR_NOEXCEPT__
	{
		return s.data;
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static char *strdata(jstring_t&& s) JSTR_NOEXCEPT__
	{
		return s.data;
	}

	template <typename Str, typename... StrArgs,
		typename = typename std::enable_if<jstd::are_strings<Str, StrArgs...>(), int>::type>
	JSTR_INLINE__
	void cat_alloc(Str&& arg, StrArgs&&... args) JSTR_NOEXCEPT__
	{
		static_assert(sizeof...(args), "At least two arguments needed!");
		const size_t arglen_1 = strlen(std::forward<Str>(arg));
		const size_t arglen = strlen_args(std::forward<StrArgs>(args)...);
		this->alloc(arglen_1 + arglen + 1);
		if (unlikely(!this->data))
			return;
		char *tmp = this->data;
		memcpy(tmp, strdata(arg), arglen_1);
		tmp += arglen_1;
		cat_loop_assign(&tmp, std::forward<StrArgs>(args)...);
		*tmp = '\0';
		this->size = arglen;
	}

	template <typename Str, typename... StrArgs,
		typename = typename std::enable_if<jstd::are_strings<Str, StrArgs...>(), int>::type>
	JSTR_INLINE__
	void cat_impl(Str&& arg, StrArgs&&... args) JSTR_NOEXCEPT__
	{
		static_assert(sizeof...(args), "At least two arguments needed! Use append instead.");
		const size_t arglen_1 = strlen(std::forward<Str>(arg));
		const size_t arglen = strlen_args(std::forward<StrArgs>(args)...);
		if (sizeof...(args) + this->size > this->capacity) {
			this->reserve_add(sizeof...(args));
			if (unlikely(!this->data)) {
				this->size = 0;
				this->capacity = 0;
				return;
			}
		}
		char *tmp = this->data + this->size;
		memcpy(tmp, arg, arglen_1);
		tmp += arglen_1;
		cat_loop_assign(&tmp, std::forward<StrArgs>(args)...);
		*tmp = '\0';
		this->size += arglen;
	}

JSTR_PUBLIC__

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	void cat(Str&& arg) JSTR_NOEXCEPT__
	{
		this->append(std::forward<Str>(arg));
	}

	template <typename Str, typename OtherStr, typename... StrArgs,
		typename = typename std::enable_if<jstd::are_strings<Str, OtherStr, StrArgs...>(), int>::type>
	JSTR_INLINE__
	void cat(Str&& arg1, OtherStr&& arg2, StrArgs&&... args) JSTR_NOEXCEPT__
	{
		this->cat_impl(std::forward<Str>(arg1), std::forward<OtherStr>(arg2), std::forward<StrArgs>(args)...);
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	jstring_t(Str&& s) JSTR_NOEXCEPT__
	{
		private_jstr_alloc_assign_void(this, strlen(std::forward<Str>(s)), strdata(std::forward<Str>(s)));
	}

	JSTR_INLINE__
	jstring_t(const char *s, size_t slen) JSTR_NOEXCEPT__
	{
		private_jstr_alloc_assign_void(this, slen, s);
	}

	JSTR_INLINE__
	jstring_t(const size_t cap) JSTR_NOEXCEPT__
	{
		this->size = 0;
		private_jstr_alloc_void(this, cap);
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	jstring_t(const size_t cap, Str&& s) JSTR_NOEXCEPT__
	{
		private_jstr_constructor_cap(this, cap, strdata(std::forward<Str>(s)), strlen(std::forward<Str>(s)));
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	jstring_t(const size_t cap, Str&& s, std::size_t slen) JSTR_NOEXCEPT__
	{
		private_jstr_constructor_cap(this, cap, strdata(std::forward(s)), slen);
	}

	JSTR_INLINE__
	jstring_t(const size_t cap, const std::size_t future_size) JSTR_NOEXCEPT__
	{
		private_jstr_alloc_void(this, cap);
		if (likely(this->data))
			this->size = future_size;
	}

	JSTR_INLINE__
	jstring_t(const jstring_t *other_) JSTR_NOEXCEPT__
	{
		private_jstr_alloc_assign_void(this, other_->size, other_->data);
	}

	JSTR_INLINE__
	void alloc() JSTR_NOEXCEPT__
	{
		jstr_alloc(this, 8);
	}

	JSTR_INLINE__
	void alloc(size_t cap) JSTR_NOEXCEPT__
	{
		jstr_alloc(this, cap);
	}

	template <size_t N>
	JSTR_INLINE__
	void alloc(const char (&s)[N]) JSTR_NOEXCEPT__
	{
		jstr_alloc_assign(this, N - 1, s);
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	void alloc(Str&& s) JSTR_NOEXCEPT__
	{
		jstr_alloc_assign(this, strlen(std::forward<Str>(s)), strdata(std::forward<Str>(s)));
	}

	JSTR_INLINE__
	void alloc(const char *s, size_t slen) JSTR_NOEXCEPT__
	{
		jstr_alloc_assign(this, slen, s);
	}

	JSTR_INLINE__
	void pop_back() JSTR_NOEXCEPT__
	{
		jstr_pop_back(this);
	}

	JSTR_INLINE__
	void pop_front() JSTR_NOEXCEPT__
	{
		jstr_pop_front(this);
	}

	JSTR_INLINE__
	void push_front(const char c) JSTR_NOEXCEPT__
	{
		jstr_push_front(this, c);
	}

	JSTR_INLINE__
	void push_back(const char c) JSTR_NOEXCEPT__
	{
		jstr_push_back(this, c);
	}

	JSTR_INLINE__
	void lower() JSTR_NOEXCEPT__
	{
		jstd_tolower_s(this->data);
	}

	JSTR_INLINE__
	void upper() JSTR_NOEXCEPT__
	{
		jstd_toupper_s(this->data);
	}

	JSTR_INLINE__
	void operator-=(const size_t size_) JSTR_NOEXCEPT__
	{
		this->size -= size_;
	}

	JSTR_INLINE__
	jstring_t operator-(const size_t size_) JSTR_NOEXCEPT__
	{
		jstring_t tmp;
		tmp.capacity = JSTR_GET_MALLOC_SIZE(this->size - size);
		tmp.data = JSTR_CAST__(char *)malloc(tmp.capacity);
		if (likely(tmp.data)) {
			tmp.size = this->size - size_;
			memcpy(tmp.data, this->data, tmp.size);
			(tmp.data)[tmp.size] = '\0';
		} else {
			tmp.capacity = 0;
		}
		return tmp;
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	void operator+=(Str &&s) JSTR_NOEXCEPT__
	{
		this->append(std::forward<Str>(s));
	}

	JSTR_INLINE__
	void operator+=(const char c) JSTR_NOEXCEPT__
	{
		this->push_back(c);
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	jstring_t operator+(Str&& s) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return this->operator_plus(strdata(std::forward<Str>(s)), strlen(std::forward<Str>(s)));
	}

JSTR_PRIVATE__

	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	jstring_t operator_plus(const char *s, const size_t slen) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		jstring_t tmp;
		tmp.capacity = JSTR_GET_MALLOC_SIZE(this->size + slen);
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

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	void operator=(Str &&s) JSTR_NOEXCEPT__
	{
		this->assign(std::forward<Str>(s));
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	void append(Str &&s) JSTR_NOEXCEPT__
	{
		private_jstr_append(this, strdata(std::forward<Str>(s)), strlen(std::forward<Str>(s)));
	}

	JSTR_INLINE__
	void append(const char *s, size_t slen) JSTR_NOEXCEPT__
	{
		private_jstr_append(this, s, slen);
		if (likely(s))
			*(this->data + slen) = '\0';
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	void append_n(Str &&s, size_t N) JSTR_NOEXCEPT__
	{
		private_jstr_append(this, strdata(std::forward<Str>(s)), N);
		*(this->data + N) = '\0';
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *str(Str&& s) JSTR_NOEXCEPT__
	{
		return private_jstr_str(this, strdata(std::forward<Str>(s)), strlen(std::forward<Str>(s)));
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *chr(int c) JSTR_NOEXCEPT__
	{
		return jstr_chr(this, c);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *rchr(int c) JSTR_NOEXCEPT__
	{
		return jstr_rchr(this, c);
	}

	JSTR_INLINE__
	void swap(jstring_t *other_) JSTR_NOEXCEPT__
	{
		jstr_swap(this, other_);
	}

	JSTR_INLINE__
	void reserve(const size_t cap) JSTR_NOEXCEPT__
	{
		jstr_reserve(this, cap);
	}

	JSTR_INLINE__
	void reserve_add(const size_t add_cap) JSTR_NOEXCEPT__
	{
		jstr_reserve(this, this->capacity + add_cap);
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	void assign(Str&& s) JSTR_NOEXCEPT__
	{
		private_jstr_assign(this, strdata(std::forward<Str>(s)), strlen(std::forward<Str>(s)));
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	void assign_n(Str&& s, size_t N) JSTR_NOEXCEPT__
	{
		private_jstr_assign(this, strdata(std::forward<Str>(s)), N);
		*(this->data + N) = '\0';
	}

	JSTR_INLINE__
	void shrink_to_fit() JSTR_NOEXCEPT__
	{
		jstr_shrink_to_fit(this);
	}

	template <typename Str,
		typename = typename std::enable_if<jstd::are_strings<Str>(), int>::type>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int cmp(Str&& s) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return jstr_cmp_str(this, strdata(std::forward<Str>(s)), strlen(std::forward<Str>(s)));
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int casecmp(const jstring_t *other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return jstr_casecmp_str(this, other_->data);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int casecmp(const char *s) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return jstr_casecmp_str(this, s);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int count_c(const int c) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return jstd_count_c(this->data, c);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int count_s(const char *needle) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
#ifdef JSTR_HAS_MEMMEM__
		return jstd_count_s(this->data, this->size, needle, strlen(needle));
#else
		return jstd_count_s(this->data, needle);
#endif // JSTR_HAS_MEMMEM__
	}

#ifdef JSTR_HAS_MEMMEM__
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int count_s(const char *needle, size_t needlelen) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return jstd_count_s(this->data, this->size, needle, needlelen);
	}
#endif // JSTR_HAS_MEMMEM__

	template <size_t N>
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int count_s(const char (&needle)[N]) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
#ifdef JSTR_HAS_MEMMEM__
		return jstd_count_s(this->data, this->size, needle, N - 1);
#else
		return jstd_count_s(this->data, needle);
#endif // JSTR_HAS_MEMMEM__
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int count_s(const jstring_t *needle) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
#ifdef JSTR_HAS_MEMMEM__
		{ return jstd_count_s(this->data, this->size, needle->data, needle->size); }
#else
		{ return jstd_count_s(this->data, needle); }
#endif // JSTR_HAS_MEMMEM__

#ifdef __USE_GNU

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *casestr(const char *needle) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return strcasestr(this->data, needle);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *casestr(const jstring_t *needle) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return strcasestr(this->data, needle->data);
	}

#endif // __USE_GNU

#endif // __cplusplus templates

JSTR_PRIVATE__

#ifdef __cplusplus

	JSTR_INLINE__
	void private_jstr_constructor_cap(jstring_t *JSTR_RESTRICT__ this_,
					const size_t cap,
					const char *JSTR_RESTRICT__ const s,
					const size_t slen) JSTR_NOEXCEPT__
	{
		this_->capacity = JSTR_GET_MALLOC_SIZE(cap);
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
	void private_jstr_alloc_assign_void(jstring_t *JSTR_RESTRICT__ this_,
					const size_t slen,
					const char *JSTR_RESTRICT__ const s,
					...) JSTR_NOEXCEPT__
	{
		this_->capacity = JSTR_GET_MALLOC_SIZE(slen);
		this_->data = JSTR_CAST__(char *)malloc(this_->capacity);
		if (unlikely(!this_->data)) {
			this_->capacity = 0;
			this_->size = 0;
			return;
		}
		this_->size = slen;
		memcpy(this_->data, s, slen + 1);
	}

	JSTR_INLINE__
	void private_jstr_alloc_void(jstring_t *JSTR_RESTRICT__ this_, size_t size_) JSTR_NOEXCEPT__
	{
		this_->capacity = JSTR_GET_MALLOC_SIZE(size_);
		this_->data = JSTR_CAST__(char *)malloc(this_->capacity);
		if (unlikely(!this_->data)) {
			this_->capacity = 0;
			this_->size = 0;
		}
	}

#endif // __cpluslus

#endif // __cpluslus templates

	JSTR_INLINE__
	void jstr_reserve_f_exact(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__
	{
		this_->data = JSTR_CAST__(char *)realloc(this_->data, cap);
		if (unlikely(!this_->data)) {
			this_->capacity = 0;
			this_->size = 0;
			return;
		}
		this_->capacity = cap;
	}

	JSTR_INLINE__
	void jstr_reserve_f(jstring_t *JSTR_RESTRICT__ this_, size_t cap, ...) JSTR_NOEXCEPT__
	{
		do {
			this_->capacity *= JSTR_MULTIPLIER;
		} while (this_->capacity < cap);
		this_->data = JSTR_CAST__(char *)realloc(this_->data, this_->capacity);
		if (unlikely(!this_->data)) {
			this_->capacity = 0;
			this_->size = 0;
		}
	}

	JSTR_INLINE__
	void jstr_reserve(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__
	{
		if (cap > this_->capacity)
			jstr_reserve_f(this_, cap);
	}

	void private_jstr_cat(jstring_t *JSTR_RESTRICT__ this_, size_t len, ...) JSTR_NOEXCEPT__
	{
		jstr_reserve(this_, this_->size + len + 1);
		if (unlikely(!this_->data)) {
			this_->capacity = 0;
			this_->size = 0;
			return;
		}
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
		return;
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
	void private_jstr_append(jstring_t *JSTR_RESTRICT__ this_,
				const char *JSTR_RESTRICT__ const s,
				size_t slen) JSTR_NOEXCEPT__
	{
		jstr_reserve(this_, this_->size + slen);
		if (unlikely(!this_->data)) {
			this_->capacity = 0;
			this_->size = 0;
			return;
		}
		private_jstr_append_u(this_, s, slen);
	}

#ifdef __cplusplus

JSTR_INLINE__
void jstr_alloc_assign(jstring_t *JSTR_RESTRICT__ this_,
		const size_t slen,
		const char *JSTR_RESTRICT__ const s,
		...) JSTR_NOEXCEPT__
{
	this_->capacity = JSTR_GET_MALLOC_SIZE(slen);
	this_->data = JSTR_CAST__(char *)malloc(this_->capacity);
	if (unlikely(!this_->data)) {
		this_->capacity = 0;
		this_->size = 0;
		return;
	}
	this_->size = slen;
	memcpy(this_->data, s, slen + 1);
}

int private_jstr_alloc_cat(jstring_t *JSTR_RESTRICT__ this_,
			const size_t arglen,
			...) JSTR_NOEXCEPT__
{
	this_->capacity = JSTR_GET_MALLOC_SIZE(arglen);
	this_->data = JSTR_CAST__(char *)malloc(this_->capacity);
	if (unlikely(!this_->data)) {
		this_->capacity = 0;
		this_->size = 0;
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
int jstr_alloc(jstring_t *JSTR_RESTRICT__ this_, size_t size_) JSTR_NOEXCEPT__
{
	this_->size = 0;
	this_->capacity = JSTR_GET_MALLOC_SIZE(size_);
	this_->data = JSTR_CAST__(char *)malloc(this_->capacity * sizeof(*this_->data));
	if (unlikely(!this_->data)) {
		this_->capacity = 0;
		return 0;
	}
	return 1;
}

#else

#define jstr_alloc(this_, size_)                                                                       \
do {                                                                                                   \
	JSTR_ASSERT_IS_SIZE(size_)                                                                     \
	                                                                                               \
	((this_)->size) = 0;                                                                           \
	((this_)->capacity) = JSTR_GET_MALLOC_SIZE(size_);                                             \
	((this_)->data) = JSTR_CAST__(char *)malloc(((this_)->capacity) * sizeof(*((this_)->data)));   \
	if (unlikely(!((this_)->data)))                                                                \
		((this_)->capacity) = 0;                                                               \
} while (0)

#define jstr_alloc_cat(this_, ...)                                                                   \
do {                                                                                                 \
	JSTR_IS_STR_VA_ARGS(__VA_ARGS__)                                                             \
	                                                                                             \
	((this_)->size) = (PP_STRLEN_VA_ARGS(__VA_ARGS__));                                          \
	((this_)->capacity) = JSTR_GET_MALLOC_SIZE((this_)->size);                                   \
	((this_)->data) = malloc(((this_)->capacity) * sizeof(*((this_)->data)));                    \
	if (likely(((this_)->data))) {                                                               \
		if (PP_NARG(__VA_ARGS__) == 1) {                                                     \
			memcpy(((this_)->data), PP_FIRST_ARG(__VA_ARGS__), ((this_)->size) + 1);     \
		} else {                                                                             \
			size_t dest_it__ = 0;                                                        \
			PP_STRCPY_VA_ARGS(((this_)->data), dest_it__, __VA_ARGS__);                  \
			((this_)->data)[dest_it__] = '\0';                                           \
		}                                                                                    \
	} else {                                                                                     \
		((this_)->capacity) = 0;                                                             \
		((this_)->size) = 0;                                                                 \
	}                                                                                            \
} while (0)

#define jstr_alloc_assign(this_, ...) private_jstr_alloc_assign(this_, __VA_ARGS__, 0)

#define private_jstr_alloc_assign(this_, ...)                                                                               \
do {                                                                                                                        \
	JSTR_ASSERT_SEMICOLON(PP_NARG(__VA_ARGS__) <= 3, "Function only accepts two arguments maximum!")                    \
	JSTR_ASSERT_IS_STR(PP_FIRST_ARG(__VA_ARGS__))                                                                       \
	JSTR_ASSERT_IS_SIZE(PP_SECOND_ARG(__VA_ARGS__))                                                                     \
	                                                                                                                    \
	((this_)->size) = (PP_NARG(__VA_ARGS__) == 3) ? PP_SECOND_ARG(__VA_ARGS__) : (strlen(PP_FIRST_ARG(__VA_ARGS__)));   \
	((this_)->capacity) = JSTR_GET_MALLOC_SIZE(((this_)->size));                                                        \
	((this_)->data) = malloc(((this_)->capacity) * sizeof(*((this_)->data)));                                           \
	if (likely(((this_)->data))) {                                                                                      \
		if (PP_NARG(__VA_ARGS__) == 3)                                                                              \
			memcpy(((this_)->data), PP_FIRST_ARG(__VA_ARGS__), PP_SECOND_ARG(__VA_ARGS__) + 1);                 \
		else                                                                                                        \
			memcpy(((this_)->data), PP_FIRST_ARG(__VA_ARGS__), ((this_)->size) + 1);                            \
	} else {                                                                                                            \
		((this_)->capacity) = 0;                                                                                    \
		((this_)->size) = 0;                                                                                        \
	}                                                                                                                   \
} while (0)

#define jstr_alloc_assign_n(this_, s, N)       \
do {                                           \
	private_jstr_alloc_assign(this, s, N); \
	if (likely(((this_)->data)))           \
		((this_)->data)[N] = '\0';     \
} while (0)

#define jstr_cat(this_, ...)                                                                                            \
do {                                                                                                                    \
	JSTR_IS_STR_VA_ARGS(__VA_ARGS__)                                                                                \
	                                                                                                                \
	((this_)->size) += (PP_STRLEN_VA_ARGS(__VA_ARGS__));                                                            \
	((this_)->capacity) = JSTR_GET_MALLOC_SIZE((this_)->size);                                                      \
	((this_)->data) = realloc(((this_)->data), ((this_)->capacity) * sizeof(*((this_)->data)));                     \
	if (unlikely(!((this_)->data))) {                                                                               \
		((this_)->capacity) = 0;                                                                                \
		((this_)->size) = 0;                                                                                    \
	} else {                                                                                                        \
		if (PP_NARG(__VA_ARGS__) == 1) {                                                                        \
			memcpy(((this_)->data) + ((this_)->size), PP_FIRST_ARG(__VA_ARGS__), ((this_)->size) + 1);      \
		} else {                                                                                                \
			size_t dest_it__ = ((this_)->size);                                                             \
			PP_STRCPY_VA_ARGS(((this_)->data), dest_it__, __VA_ARGS__);                                     \
			((this_)->data)[dest_it__] = '\0';                                                              \
		}                                                                                                       \
	}                                                                                                               \
} while (0)

#endif // __cpluslus

	JSTR_INLINE__
	void jstr_swap(jstring_t *JSTR_RESTRICT__ this_, jstring_t *JSTR_RESTRICT__ other_) JSTR_NOEXCEPT__
	{
		char *JSTR_RESTRICT__ const tmp_data = this_->data;
		const size_t tmp_size = this_->size;
		const size_t tmp_capacity = this_->capacity;
		this_->data = other_->data;
		this_->size = other_->size;
		this_->capacity = other_->capacity;
		other_->data = tmp_data;
		other_->size = tmp_size;
		other_->capacity = tmp_capacity;
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
	int jstr_cmp_f(const jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return memcmp(this_->data, other_->data, this_->size);
	}

	JSTR_INLINE__
	int jstr_cmp(const jstring_t *JSTR_RESTRICT__ this_,
			const jstring_t *JSTR_RESTRICT__ other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
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
	int jstr_casecmp_f(const jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return jstd_casecmp(this_->data, other_->data);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int jstr_casecmp_str(const jstring_t *JSTR_RESTRICT__ this_, const char *JSTR_RESTRICT__ s) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return jstd_casecmp(this_->data, s);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	int jstr_casecmp_jstr(const jstring_t *JSTR_RESTRICT__ this_, const jstring_t *JSTR_RESTRICT__ other_) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
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
	void jstr_assign_f(jstring_t *JSTR_RESTRICT__ dest,
			const char *JSTR_RESTRICT__ src,
			size_t srclen) JSTR_NOEXCEPT__
	{
		jstr_reserve_f(dest, srclen);
		if (likely(dest->data))
			jstr_assign_u(dest, src, srclen);
	}

	JSTR_INLINE__
	void jstr_assign_jstr_f(jstring_t *JSTR_RESTRICT__ dest,
			const jstring_t *JSTR_RESTRICT__ const src) JSTR_NOEXCEPT__
	{
		jstr_reserve_f(dest, src->size);
		if (likely(dest->data))
			jstr_assign_jstr_u(dest, src);
	}

	JSTR_INLINE__
	void private_jstr_assign(jstring_t *JSTR_RESTRICT__ dest,
			const char *JSTR_RESTRICT__ src,
			const size_t srclen,
			...) JSTR_NOEXCEPT__
	{
		if (dest->capacity < srclen)
			jstr_assign_f(dest, src, srclen);
		else
			jstr_assign_u(dest, src, srclen);
	}

	JSTR_INLINE__
	void jstr_assign_jstr(jstring_t *JSTR_RESTRICT__ dest,
			const jstring_t *JSTR_RESTRICT__ const src,
			...) JSTR_NOEXCEPT__
	{
		if (dest->capacity < src->capacity)
			jstr_assign_f(dest, src->data, src->capacity);
		else
			jstr_assign_jstr_u(dest, src);
	}

	JSTR_INLINE__
	void jstr_shrink_to_fit_f(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__
	{
		this_->data = JSTR_CAST__(char *)realloc(this_->data, this_->size + 1);
		if (unlikely(!this_->data)) {
			this_->size = 0;
			this_->capacity = 0;
			return;
		}
		this_->capacity = this_->size + 1;
	}

	JSTR_INLINE__
	void jstr_shrink_to_fit(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__
	{
		if (likely(this_->capacity != this_->size))
			jstr_shrink_to_fit_f(this_);
	}

	JSTR_INLINE__
	void jstr_shrink_to_f(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__
	{
		this_->data = JSTR_CAST__(char *)realloc(this_->data, cap);
		if (unlikely(!this_->data)) {
			this_->size = 0;
			this_->capacity = 0;
			return;
		}
		this_->data[this_->size = cap] = '\0';
		this_->capacity = cap;
	}

	JSTR_INLINE__
	void jstr_shrink_to(jstring_t *JSTR_RESTRICT__ this_, size_t cap) JSTR_NOEXCEPT__
	{
		if (cap > this_->capacity)
			return;
		jstr_shrink_to_f(this_, cap);
	}

	JSTR_INLINE__
	void jstr_shrink_to_size_f(jstring_t *JSTR_RESTRICT__ this_, size_t size_) JSTR_NOEXCEPT__
	{
		this_->data[this_->size = size_] = '\0';
	}

	JSTR_INLINE__
	void jstr_shrink_to_size(jstring_t *JSTR_RESTRICT__ this_, size_t size_) JSTR_NOEXCEPT__
	{
		if (size_ < this_->size)
			jstr_shrink_to_size_f(this_, size_);
	}

	JSTR_INLINE__
	void jstr_push_back_u(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		this_->data[this_->size] = c;
		this_->data[++this_->size] = '\0';
	}

	JSTR_INLINE__
	void jstr_push_back_f(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		jstr_reserve_f(this_, this_->size * JSTR_MULTIPLIER);
		if (likely(this_->data))
			jstr_push_back_u(this_, c);
	}

	JSTR_INLINE__
	void jstr_push_back(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		if (unlikely(this_->size == this_->capacity))
			jstr_push_back_f(this_, c);
		else
			jstr_push_back_u(this_, c);
	}

	JSTR_INLINE__
	void jstr_pop_back(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__
	{
		this_->data[--this_->size] = '\0';
	}

	JSTR_INLINE__
	void jstr_push_front_u(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		memmove(this_->data + 1, this_->data, ++this_->size);
		*this_->data = c;
	}

	JSTR_INLINE__
	void jstr_push_front_f(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		jstr_reserve_f(this_, this_->size * JSTR_MULTIPLIER);
		if (likely(this_->data))
			jstr_push_front_u(this_, c);
	}

	JSTR_INLINE__
	void jstr_push_front(jstring_t *JSTR_RESTRICT__ this_, const char c) JSTR_NOEXCEPT__
	{
		if (unlikely(this_->capacity == this_->size))
			jstr_push_front_f(this_, c);
		else
			jstr_push_front_u(this_, c);
	}

	JSTR_INLINE__
	void jstr_pop_front(jstring_t *JSTR_RESTRICT__ this_) JSTR_NOEXCEPT__
	{
		memmove(this_->data, this_->data + 1, this_->size--);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
#ifdef JSTR_HAS_MEMMEM__
	char *private_jstr_str(const jstring_t *haystack, const char *JSTR_RESTRICT__ needle, size_t needlelen, ...) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return JSTR_CAST__(char *)memmem(haystack->data, haystack->size, needle, needlelen);
	}
#else
	char *jstr_str(const jstring_t *haystack, const char *JSTR_RESTRICT__ needle) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return JSTR_CAST__(char *)strstr(haystack->data, needle);
	}
#endif // JSTR_HAS_MEMMEM__

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *jstr_rchr(const jstring_t *JSTR_RESTRICT__ this_, int c) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
#ifdef JSTR_HAS_MEMRCHR__
		return JSTR_CAST__(char *)memrchr(this_->data, c, this_->size);
#else
		return JSTR_CAST__(char *)jstd_rchr(this_->data, c, this_->size);
#endif // JSTR_HAS_MEMRCHR__
	}

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

#ifdef __USE_GNU

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *jstr_casestr(const jstring_t *JSTR_RESTRICT__ this_, const char *JSTR_RESTRICT__ needle) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return JSTR_CAST__(char *)strcasestr(this_->data, needle);
	}

#endif // __USE_GNU

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *jstr_chr(const jstring_t *JSTR_RESTRICT__ this_, int c) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		return JSTR_CAST__(char *)memchr(this_->data, c, this_->size);
	}

#ifndef __cplusplus
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	char *jstr_at(const jstring_t *this_, const size_t index) JSTR_CPP_CONST__ JSTR_NOEXCEPT__
	{
		assert(index <= this_->size);
		return this_->data + index;
	}
#endif // ! __cpluslus

#ifdef __cplusplus

} jstring_t;

#endif // __cplusplus end of struct

#ifndef __cplusplus

#define jstr_assign(this_, ...)                                                                           \
do {                                                                                                      \
	if (PP_NARG(__VA_ARGS__) == 2)                                                                    \
		private_jstr_assign(this_, __VA_ARGS__, 0);                                               \
	else                                                                                              \
		private_jstr_assign(this_, PP_FIRST_ARG(__VA_ARGS__), strlen(PP_FIRST_ARG(__VA_ARGS__))); \
} while (0)

#define jstr_assign_n(this_, s, N)                       \
do {                                                     \
	jstr_assign(this_, s, N);                        \
	if (likely(((this_)->data)))                     \
		((this_)->data)[((this_)->size)] = '\0'; \
} while (0)

#define jstr_append(this_, ...)                                                                           \
do {                                                                                                      \
	if (PP_NARG(__VA_ARGS__) == 2)                                                                    \
		private_jstr_append(this_, __VA_ARGS__, 0);                                               \
	else                                                                                              \
		private_jstr_append(this_, PP_FIRST_ARG(__VA_ARGS__), strlen(PP_FIRST_ARG(__VA_ARGS__))); \
} while (0)

#define jstr_append_n(this_, s, N)                       \
do {                                                     \
	jstr_append(this_, s, N);                        \
	if (likely(((this_)->data)))                     \
		((this_)->data)[((this_)->size)] = '\0'; \
} while (0)

#define private_jstr_reserve_x(this_jstr, multiplier)                             \
	jstr_reserve_f_exact(this_jstr, ((multiplier) * ((this_jstr)->capacity)))

#define jstr_reserve_2x(this_jstr) private_jstr_reserve_x(this_jstr, 2)
#define jstr_reserve_4x(this_jstr) private_jstr_reserve_x(this_jstr, 4)
#define jstr_reserve_8x(this_jstr) private_jstr_reserve_x(this_jstr, 8)
#define jstr_reserve_16x(this_jstr) private_jstr_reserve_x(this_jstr, 16)
#define jstr_reserve_32x(this_jstr) private_jstr_reserve_x(this_jstr, 32)
#define jstr_reserve_64x(this_jstr) private_jstr_reserve_x(this_jstr, 64)

#define private_jstr_reserve_s_x(this_jstr, multiplier)                             \
	jstr_reserve_s_f_exact(this_jstr, ((multiplier) * ((this_jstr)->capacity)))

#define jstr_foreach(elem, jstr)                         \
 	for (char *elem = ((jstr)->data); *elem; ++elem)

#define jstr_begin(this_jstr) ((this_jstr)->data)
#define jstr_end(this_jstr) (((this_jstr)->data) + ((this_jstr)->size))

#define jstr_cbegin(this_jstr) ((const char *)((this_jstr)->data))
#define jstr_cend(this_jstr) ((const char *)(((this_jstr)->data) + ((this_jstr)->size)))

#define jstr_foreach_index(elem, jstr)                        \
	for (size_t i = 0, end__ = jstr.size; i < end__; ++i)

#endif // ! __cplusplus : c macros

#endif // JSTR_H_DEF__
