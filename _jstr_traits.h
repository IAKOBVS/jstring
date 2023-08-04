#ifndef JTRAITS_H_DEF
#define JTRAITS_H_DEF

#ifdef __cplusplus

#	include "_jstr_macros.h"
#	include <utility>

typedef struct jstr_t jstr_t;

namespace jstr {
namespace traits {
template <typename T, typename Arg>
JSTR_INLINE static constexpr int is_same_decay() JSTR_NOEXCEPT
{
	using namespace std;
	return is_same<T, typename decay<Arg>::type>::value;
}

template <typename T, typename Arg>
JSTR_INLINE static constexpr int is_same_decay(Arg) JSTR_NOEXCEPT
{
	return is_same_decay<T, Arg>();
}

template <typename Str>
JSTR_INLINE static constexpr int are_strings() JSTR_NOEXCEPT
{
	using namespace std;
	return (
	is_same_decay<const char *, Str>()
	|| is_same_decay<char *, Str>()
	|| is_same_decay<jstr_t *, Str>()
	|| is_same<jstr_t &, Str>::value
	|| is_same<jstr_t &&, Str>::value
	|| is_same<const jstr_t &, Str>::value
	|| is_same<const jstr_t &&, Str>::value
	|| is_same<volatile jstr_t &, Str>::value
	|| is_same<volatile jstr_t &&, Str>::value
	|| is_same<const volatile jstr_t &, Str>::value
	|| is_same<const volatile jstr_t &&, Str>::value);
}

template <typename Str, typename... StrArgs, typename = typename std::enable_if<sizeof...(StrArgs) != 0>::type>
JSTR_INLINE static constexpr int are_strings() JSTR_NOEXCEPT
{
	return are_strings<Str>()
	       && are_strings<StrArgs...>();
}

template <typename Str>
JSTR_INLINE static constexpr int are_strings(Str) JSTR_NOEXCEPT
{
	return are_strings<Str>();
}

template <typename Str, typename... StrArgs>
JSTR_INLINE static constexpr int are_strings(Str, StrArgs...) JSTR_NOEXCEPT
{
	return are_strings<Str, StrArgs...>();
}

template <typename Str, typename... StrArgs>
JSTR_INLINE static constexpr void assert_are_strings(Str &&, StrArgs &&...) JSTR_NOEXCEPT
{
	static_assert(are_strings<Str, StrArgs...>(), "Passing non-strings as string arguments!");
}

//

template <typename T, typename Arg>
JSTR_INLINE static constexpr int are_same() JSTR_NOEXCEPT
{
	return std::is_same<T, Arg>::value;
}

template <typename T, typename Arg, typename... Args, typename = typename std::enable_if<sizeof...(Args) != 0>::type>
JSTR_INLINE static constexpr int are_same() JSTR_NOEXCEPT
{
	return are_same<T, Arg>()
	       && are_same<T, Args...>();
}

template <typename T, typename Arg>
JSTR_INLINE static constexpr int are_same(Arg) JSTR_NOEXCEPT
{
	return are_same<T, Arg>();
}

template <typename T, typename Arg, typename... Args>
JSTR_INLINE static constexpr int are_same(Arg, Args...) JSTR_NOEXCEPT
{
	return are_same<T, Arg, Args...>();
}

template <typename T, typename Arg, typename... Args>
JSTR_INLINE static constexpr void assert_are_same(Arg &&, Args &&...) JSTR_NOEXCEPT
{
	static_assert(are_same<T, Arg, Args...>(), "Passing non-T as T arguments!");
}

}
}

#endif /* __cplusplus */

#endif // JTRAITS_H_DEF
