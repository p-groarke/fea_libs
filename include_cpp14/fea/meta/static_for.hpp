#pragma once
#include <type_traits>

namespace fea {

namespace detail {
template <class Func, size_t... I>
constexpr void static_for(Func func, std::index_sequence<I...>) {
#if FEA_CPP17
	// TODO : test it.
	return (func(std::integral_constant<size_t, I>{}), ...);
#else
	char dummy[] = { (void(func(std::integral_constant<size_t, I>{})), 0)... };
#endif
}
} // namespace detail

// Call a for loop at compile time.
// Your lambda is provided with an integral_constant.
// Accept it with auto, access the index with '::value'.
template <size_t N, class Func>
constexpr void static_for(Func func) {
	detail::static_for(func, std::make_index_sequence<N>{});
}
} // namespace fea
