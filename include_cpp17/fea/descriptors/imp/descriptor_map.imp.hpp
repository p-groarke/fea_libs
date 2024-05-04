#pragma once
namespace fea {
namespace detail {
template <class T>
using has_key = decltype(std::declval<T>().key);

template <class FuncRet, class T, class Func>
FuncRet dm_unerase(Func&& func) {
	return std::forward<Func>(func)(T{});
}

template <class FuncRet, class Func, class... Ds>
constexpr auto dm_unerase_lookup() {
	using func_t
			= std::common_type_t<decltype(&dm_unerase<FuncRet, Ds, Func>)...>;
	return std::array<func_t, sizeof...(Ds)>{
		&dm_unerase<FuncRet, Ds, Func>...
	};
}
} // namespace detail

template <class KeyT, class... Descriptors>
constexpr size_t descriptor_map<KeyT, Descriptors...>::size() {
	return sizeof...(Descriptors);
}

template <class KeyT, class... Descriptors>
template <KeyT K>
constexpr auto descriptor_map<KeyT, Descriptors...>::descriptor() {
	return std::tuple_element_t<size_t(K), desc_tup_t>{};
}

template <class KeyT, class... Descriptors>
template <class Func>
constexpr decltype(auto) descriptor_map<KeyT, Descriptors...>::descriptor(
		KeyT key, Func&& func) {
	// Unerase lookup (switch-case equivalent).
	using func_ret_t
			= std::invoke_result_t<Func, decltype(descriptor<KeyT(0)>())>;
	constexpr auto lookup
			= detail::dm_unerase_lookup<func_ret_t, Func, Descriptors...>();
	return lookup[size_t(key)](std::forward<Func>(func));
}

template <class KeyT, class... Descriptors>
template <size_t Idx>
constexpr auto descriptor_map<KeyT, Descriptors...>::key() {
	return std::tuple_element_t<Idx, desc_tup_t>::key;
}

template <class KeyT, class... Descriptors>
template <class Func>
constexpr auto descriptor_map<KeyT, Descriptors...>::make_tuple(Func&& func) {
	return std::apply(
			[&](auto... desc) { return std::make_tuple(func(desc)...); },
			desc_tup_t{});
}

template <class KeyT, class... Descriptors>
template <class Func>
constexpr auto descriptor_map<KeyT, Descriptors...>::make_array(Func&& func) {
	using ret_t = decltype(func(fea::front_t<Descriptors...>{}));
	std::array<ret_t, desc_count> ret{};

	fea::static_for<desc_count>([&](auto const_i) {
		constexpr size_t i = const_i;
		using desc_t = std::tuple_element_t<i, desc_tup_t>;
		ret[i] = func(desc_t{});
	});
	return ret;
}

template <class KeyT, class... Descriptors>
template <class Func>
constexpr auto descriptor_map<KeyT, Descriptors...>::make_enum_array(
		Func&& func) {
	using ret_t = decltype(func(fea::front_t<Descriptors...>{}));
	fea::enum_array<ret_t, key_t, desc_count> ret{};

	fea::static_for<desc_count>([&](auto const_i) {
		constexpr size_t i = const_i;
		using desc_t = std::tuple_element_t<i, desc_tup_t>;
		ret[key_t(i)] = func(desc_t{});
	});
	return ret;
}

template <class KeyT, class... Descriptors>
template <class Func>
constexpr auto descriptor_map<KeyT, Descriptors...>::for_each_descriptor(
		Func&& func) {
	return fea::static_for<desc_count>([&](auto const_i) {
		constexpr size_t i = const_i;
		using desc_t = std::tuple_element_t<i, desc_tup_t>;
		return func(desc_t{});
	});
}

template <class KeyT, class... Descriptors>
constexpr bool descriptor_map<KeyT, Descriptors...>::has_valid_keys() {
	bool ret = true;

	constexpr bool convertible = fea::is_static_castable_v<KeyT, size_t>;
	static_assert(convertible,
			"descriptor_map : descriptor key must be convertible "
			"to size_t");

	ret &= convertible;

	constexpr bool has_all_descriptors
			= sizeof...(Descriptors) == size_t(KeyT::count);
	static_assert(has_all_descriptors,
			"descriptor_map : missing descriptors, or key type doesn't "
			"have 'count' value");
	ret &= has_all_descriptors;

	fea::static_for<std::tuple_size_v<desc_tup_t>>([&](auto const_i) {
		constexpr size_t i = const_i;
		using desc_t = std::tuple_element_t<i, desc_tup_t>;

		// Every descriptor has key.
		constexpr bool has_key = fea::is_detected_v<detail::has_key, desc_t>;
		static_assert(has_key,
				"descriptor_map : all descriptors must have the member "
				"variable 'key'");

		ret &= has_key;

		// Key is correct type.
		constexpr auto k = desc_t::key;
		using key_type = std::decay_t<decltype(k)>;
		constexpr bool same_key_t = std::is_same_v<key_type, KeyT>;
		static_assert(same_key_t,
				"descriptor_map : descriptor keys must all be of type "
				"KeyT");

		// Every descriptor ordered at its key index.
		constexpr bool ordered_correctly = i == size_t(k);
		static_assert(ordered_correctly,
				"descriptor_map : descriptors must be ordered "
				"in the same order as their 'key' index");

		ret &= ordered_correctly & same_key_t;
	});

	return ret;
}
} // namespace fea