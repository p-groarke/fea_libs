namespace fea {
namespace detail {
template <class Enum, class Func, size_t... Idx>
constexpr auto explode_enum(const Func& func, std::index_sequence<Idx...>) {
	return func(std::integral_constant<Enum, Enum(Idx)>{}...);
}

// Calls your function with each non-type enum values.
// Passes idx std::integral_constant and enum std::integral_constant.
template <class E, class Func, size_t... Idx, auto... Args>
constexpr void enum_for_each_w_idx_imp(const Func& func,
		std::index_sequence<Idx...>, std::integer_sequence<size_t, Args...>) {
	(func(std::integral_constant<size_t, Idx>{},
			 std::integral_constant<E, E(Args)>{}),
			...);
}

// Calls your function with each non-type enum values.
// Passes idx std::integral_constant and enum std::integral_constant.
template <auto... Args, class Func>
constexpr void enum_for_each_w_idx(const Func& func) {
	using enum_t = fea::front_t<decltype(Args)...>;
	enum_for_each_w_idx_imp<enum_t>(func,
			std::make_index_sequence<sizeof...(Args)>{},
			std::integer_sequence<size_t, size_t(Args)...>{});
}
} // namespace detail


template <class Enum, size_t N, class Func>
constexpr auto explode_enum(const Func& func) {
	return detail::explode_enum<Enum>(func, std::make_index_sequence<N>{});
}

template <class Enum, class Func>
constexpr auto explode_enum(const Func& func) {
	return detail::explode_enum<Enum>(
			func, std::make_index_sequence<size_t(Enum::count)>{});
}

template <class Enum, size_t N, class Func>
constexpr void enum_for_each(const Func& func) {
	fea::explode_enum<Enum, N>(
			[&](auto... constants) { (func(constants), ...); });
}

template <auto... Args, class Func>
constexpr void enum_for_each(const Func& func) {
	(func(std::integral_constant<decltype(Args), Args>{}), ...);
}

template <auto... Args>
constexpr auto make_enum_lookup() {
	constexpr size_t arr_size
			= size_t(fea::max_nt_v<fea::front_t<decltype(Args)...>, Args...>)
			+ 1u;
	std::array<size_t, arr_size> ret{};

	// Initialize everything with sentinel.
	fea::static_for<arr_size>(
			[&](auto idx) { ret[idx] = (std::numeric_limits<size_t>::max)(); });

	// Create association between enum value and actual index.
	// Aka, slot map from enum -> pack index
	detail::enum_for_each_w_idx<Args...>([&](auto idx, auto e_ic) {
		constexpr size_t i = decltype(idx)::value;
		constexpr auto e = decltype(e_ic)::value;
		constexpr size_t e_pos = size_t(e);
		ret[e_pos] = i;
	});
	return ret;
}
} // namespace fea