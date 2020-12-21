#pragma once
#include "fea/utils/platform.hpp"

#include <type_traits>

namespace fea {
// Checks if the all the passed in traits are true.
template <class...>
struct all_of : std::true_type {};

template <class Trait, class... Traits>
struct all_of<Trait, Traits...>
		: std::conditional<Trait::value, all_of<Traits...>,
				  std::false_type>::type {
	// Check that provided templates are type_traits. Better way?
	static_assert(Trait::value == true || Trait::value == false,
			"fea::all_of : must use type traits");
};

template <class... Traits>
FEA_INLINE_VAR constexpr bool all_of_v = all_of<Traits...>::value;


// Checks if none of the passed in traits are true.
template <class...>
struct none_of : std::true_type {};

template <class Trait, class... Traits>
struct none_of<Trait, Traits...>
		: std::conditional<Trait::value, std::false_type,
				  none_of<Traits...>>::type {
	// Check that provided templates are type_traits. Better way?
	static_assert(Trait::value == true || Trait::value == false,
			"fea::none_of : must use type traits");
};

template <class... Traits>
FEA_INLINE_VAR constexpr bool none_of_v = none_of<Traits...>::value;


// Checks if any of the passed in traits are true.
template <class...>
struct any_of : std::false_type {};

template <class Trait, class... Traits>
struct any_of<Trait, Traits...> : std::conditional<Trait::value, std::true_type,
										  any_of<Traits...>>::type {
	// Check that provided templates are type_traits. Better way?
	static_assert(Trait::value == true || Trait::value == false,
			"fea::any_of : must use type traits");
};

template <class... Traits>
FEA_INLINE_VAR constexpr bool any_of_v = any_of<Traits...>::value;


// TODO : C++14 implementation
// template <class...>
// constexpr bool all_unique_v = std::true_type{};
//
// template <class T, class... Rest>
// constexpr bool all_unique_v<T, Rest...> = std::bool_constant<
//		(!std::is_same_v<T, Rest> && ...) && all_unique_v<Rest...>>{};


namespace detail {
template <class...>
using void_t = void;
}

/*
Checks if a given type has function.
You must call this with a "detector alias", for example :

template <class T>
using has_myfunc = decltype(std::declval<T>().myfunc());

constexpr bool answer = is_detected_v<has_myfunc, your_class>;

More info : https://en.cppreference.com/w/cpp/experimental/is_detected

See unit tests for more examples.
*/
template <template <class...> class Op, class = void, class...>
struct is_detected : std::false_type {};
template <template <class...> class Op, class... Args>
struct is_detected<Op, detail::void_t<Op<Args...>>, Args...> : std::true_type {
};

template <template <class...> class Op, class... Args>
FEA_INLINE_VAR constexpr bool is_detected_v
		= is_detected<Op, void, Args...>::value;

} // namespace fea
