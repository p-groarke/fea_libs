namespace fea {

namespace detail {
template <class Func, class T>
using tm_has_imp = decltype(std::declval<Func>()(std::declval<T>()));

template <class FuncRet, size_t I, class TMap, class Func>
FuncRet tm_unerase(TMap& tmap, Func&& func) {
	return std::forward<Func>(func)(tmap.template at<I>());
}

// WIP : Use id_detected idiom to filter out unavailable
// instantiations and move the problem to runtime.
// template <class FuncPtrT, size_t I, class Func, class TMap>
// constexpr FuncPtrT tm_filter_uncallable() {
//	using called_t = decltype(std::declval<TMap>().at<I>());
//	// using called_t = std::tuple_element_t<I, std::decay_t<TMap>::values_t>;
//
//	if constexpr (fea::is_detected_v<tm_has_imp, Func, called_t>) {
//		return &tm_unerase<I, TMap, Func>;
//	} else {
//		return nullptr;
//	}
//}

template <class FuncRet, class Func, class TMap, size_t... Is>
constexpr auto tm_unerase_lookup(std::index_sequence<Is...>) {
	using func_t = std::common_type_t<
			decltype(&tm_unerase<FuncRet, Is, TMap, Func>)...>;
	return std::array<func_t, sizeof...(Is)>{
		&tm_unerase<FuncRet, Is, TMap, Func>...
	};

	// WIP
	// using func_t = decltype(&tm_unerase<0, TMap, Func>);
	// return std::array<func_t, sizeof...(Is)>{
	//	tm_filter_uncallable<func_t, Is, Func, TMap>()...
	//};
}

template <class... Values>
struct type_map_base {
	using values_t = std::tuple<Values...>;
	using front_t = fea::front_t<Values...>;
	using back_t = fea::back_t<Values...>;

	constexpr type_map_base() = default;
	constexpr type_map_base(const std::tuple<Values...>& values)
			: _values(values) {
	}
	constexpr type_map_base(std::tuple<Values...>&& values)
			: _values(std::move(values)) {
	}

	template <class Val>
	static constexpr bool contains_value() {
		// Just to make sure we are in constexpr land.
		constexpr bool ret = fea::any_of_v<std::is_same<Val, Values>...>;
		return ret;
	}

	// Get value at index.
	template <size_t Idx>
	constexpr const auto& at() const {
		static_assert(Idx < size(), "type_map : index out-of-range");
		return std::get<Idx>(data());
	}

	// Get value at index.
	template <size_t Idx>
	constexpr auto& at() {
		static_assert(Idx < size(), "type_map : index out-of-range");
		return std::get<Idx>(data());
	}

	// Get value at runtime index.
	template <class Func>
	constexpr decltype(auto) at(size_t idx, Func&& func) const {
		// Unerase lookup (switch-case equivalent).
		using func_ret_t = std::invoke_result_t<Func, decltype(at<0>())>;
		constexpr auto lookup
				= detail::tm_unerase_lookup<func_ret_t, Func, decltype(*this)>(
						std::make_index_sequence<size()>{});
		return lookup[idx](*this, std::forward<Func>(func));
	}

	// Get value at runtime index.
	template <class Func>
	constexpr decltype(auto) at(size_t idx, Func&& func) {
		// Unerase lookup (switch-case equivalent).
		using func_ret_t = std::invoke_result_t<Func, decltype(at<0>())>;
		constexpr auto lookup
				= detail::tm_unerase_lookup<func_ret_t, Func, decltype(*this)>(
						std::make_index_sequence<size()>{});
		return lookup[idx](*this, std::forward<Func>(func));
	}


	// The data, a tuple of your values.
	constexpr const auto& data() const {
		return _values;
	}
	constexpr auto& data() {
		return _values;
	}

	static constexpr size_t size() {
		return std::tuple_size_v<values_t>;
	}

private:
	// Your values.
	values_t _values;
};
} // namespace detail


template <class... Keys, class... Values>
template <class Key>
constexpr bool type_map<pack<Keys...>, Values...>::contains() {
	// Just to make sure we are in constexpr land.
	constexpr bool ret = pack_contains_v<Key, pack_t>;
	return ret;
}

template <class... Keys, class... Values>
template <class Key>
constexpr const auto& type_map<pack<Keys...>, Values...>::find() const {
	static_assert(contains<Key>(), "type_map : doesn't contain requested key");

	constexpr size_t idx = pack_idx_v<Key, pack_t>;
	return std::get<idx>(base_t::data());
}

template <class... Keys, class... Values>
template <class Key>
constexpr auto& type_map<pack<Keys...>, Values...>::find() {
	static_assert(contains<Key>(), "type_map : doesn't contain requested key");

	constexpr size_t idx = pack_idx_v<Key, pack_t>;
	return std::get<idx>(base_t::data());
}

template <class... Keys, class... Values>
template <class Key>
constexpr size_t type_map<pack<Keys...>, Values...>::idx() const {
	static_assert(contains<Key>(), "type_map : doesn't contain requested key");

	return pack_idx_v<Key, pack_t>;
}

template <class... Keys, class... Values>
template <class Func>
constexpr void type_map<pack<Keys...>, Values...>::for_each(Func&& func) const {
	fea::pack_for_each(
			[&, this](auto* ptr) {
				using K = std::remove_pointer_t<decltype(ptr)>;
				func(ptr, this->template find<K>());
			},
			fea::pack<Keys...>{});
}

template <class... Keys, class... Values>
template <class Func>
constexpr void type_map<pack<Keys...>, Values...>::for_each(Func&& func) {
	fea::pack_for_each(
			[&, this](auto* ptr) {
				using K = std::remove_pointer_t<decltype(ptr)>;
				func(ptr, this->template find<K>());
			},
			fea::pack<Keys...>{});
}

template <auto... Keys, class... Values>
template <auto Key>
constexpr bool type_map<pack_nt<Keys...>, Values...>::contains() {
	// Just to make sure we are in constexpr land.
	constexpr bool ret = pack_contains_nt_v<Key, pack_t>;
	return ret;
}

template <auto... Keys, class... Values>
template <auto Key>
constexpr const auto& type_map<pack_nt<Keys...>, Values...>::find() const {
	static_assert(contains<Key>(), "type_map : doesn't contain requested key");

	constexpr size_t idx = pack_idx_nt_v<Key, pack_t>;
	return std::get<idx>(base_t::data());
}

template <auto... Keys, class... Values>
template <auto Key>
constexpr auto& type_map<pack_nt<Keys...>, Values...>::find() {
	static_assert(contains<Key>(), "type_map : doesn't contain requested key");

	constexpr size_t idx = pack_idx_nt_v<Key, pack_t>;
	return std::get<idx>(base_t::data());
}

template <auto... Keys, class... Values>
template <auto Key>
constexpr size_t type_map<pack_nt<Keys...>, Values...>::idx() const {
	static_assert(contains<Key>(), "type_map : doesn't contain requested key");

	return pack_idx_nt_v<Key, pack_t>;
}

template <auto... Keys, class... Values>
template <class Func>
constexpr void type_map<pack_nt<Keys...>, Values...>::for_each(
		Func&& func) const {
	fea::pack_for_each(
			[&, this](auto key) { func(key, this->template find<key()>()); },
			fea::pack_nt<Keys...>{});
}

template <auto... Keys, class... Values>
template <class Func>
constexpr void type_map<pack_nt<Keys...>, Values...>::for_each(Func&& func) {
	fea::pack_for_each(
			[&, this](auto key) { func(key, this->template find<key()>()); },
			fea::pack_nt<Keys...>{});
}

template <class... Keys, class... Values>
constexpr auto make_type_map(
		pack<Keys...>, const std::tuple<Values...>& values) {
	static_assert(sizeof...(Keys) == sizeof...(Values),
			"fea::make_type_map : tuple must be same size as Keys");
	return type_map<pack<Keys...>, Values...>(values);
}

template <auto... Keys, class... Values>
constexpr auto make_type_map(
		pack_nt<Keys...>, const std::tuple<Values...>& values) {
	static_assert(sizeof...(Keys) == sizeof...(Values),
			"fea::make_type_map : tuple must be same size as Keys");
	return type_map<pack_nt<Keys...>, Values...>(values);
}

template <class... Keys, class T, size_t N>
constexpr auto make_type_map(pack<Keys...>, const std::array<T, N>& values) {
	static_assert(sizeof...(Keys) == N,
			"fea::make_type_map : array must be same size as Keys");

	return std::apply(
			[](const auto&... ts) {
				return type_map<pack<Keys...>, std::decay_t<decltype(ts)>...>(
						std::forward_as_tuple(ts...));
			},
			values);
}

template <auto... Keys, class T, size_t N>
constexpr auto make_type_map(pack_nt<Keys...>, const std::array<T, N>& values) {
	static_assert(sizeof...(Keys) == N,
			"fea::make_type_map : array must be same size as Keys");

	return std::apply(
			[](const auto&... ts) {
				return type_map<pack_nt<Keys...>,
						std::decay_t<decltype(ts)>...>(
						std::forward_as_tuple(ts...));
			},
			values);
}


template <class Key, class Value>
kv_t<Key, Value>::kv_t(Value&& v)
		: v(std::forward<Value>(v)) {
}

template <class Key, class Value>
kv_t<Key, Value>::kv_t(Key&&, Value&& v)
		: v(std::forward<Value>(v)) {
}

template <auto Key, class Value>
kv_nt<Key, Value>::kv_nt(Value&& v)
		: v(std::forward<Value>(v)) {
}

template <auto Key, class Value>
kv_nt<Key, Value> make_kv_nt(Value&& v) {
	return kv_nt<Key, Value>{ std::forward<Value>(v) };
}


template <class... Keys, class... Values>
constexpr auto make_type_map(kv_t<Keys, Values>&&... kvs) {
	return type_map<pack<Keys...>, Values...>(
			std::make_tuple(fea::move_if_moveable(kvs.v)...));
}

template <auto... Keys, class... Values>
constexpr auto make_type_map(kv_nt<Keys, Values>&&... kvs) {
	return type_map<pack_nt<Keys...>, Values...>(
			std::make_tuple(fea::move_if_moveable(kvs.v)...));
}


template <class Func, class Key, Key... Keys, class... Values>
decltype(auto) runtime_get(Func&& func, Key e,
		const fea::type_map<fea::pack_nt<Keys...>, Values...>& t_map) {
	// First, get the associated index for the enum value.
	// The underlying enum value is not necessarily == position.
	size_t val_idx = fea::runtime_get_idx(e, fea::pack_nt<Keys...>{});
	return fea::runtime_get(std::forward<Func>(func), val_idx, t_map.data());
}

template <class Func, class Key, Key... Keys, class... Values>
decltype(auto) runtime_get(Func&& func, Key e,
		fea::type_map<fea::pack_nt<Keys...>, Values...>& t_map) {
	// First, get the associated index for the enum value.
	// The underlying enum value is not necessarily == position.
	size_t val_idx = fea::runtime_get_idx(e, fea::pack_nt<Keys...>{});
	return fea::runtime_get(std::forward<Func>(func), val_idx, t_map.data());
}
} // namespace fea