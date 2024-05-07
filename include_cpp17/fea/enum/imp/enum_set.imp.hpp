namespace fea {
template <class Enum, size_t N>
constexpr enum_set<Enum, N>::enum_set(std::initializer_list<Enum> true_set)
		: std::array<bool, N>{} {
	for (Enum e : true_set) {
		operator[](e) = true;
	}
}

template <class Enum, size_t N>
template <class... Vals>
constexpr enum_set<Enum, N>::enum_set(Vals... values)
		: std::array<bool, N>{ values... } {
	static_assert(sizeof...(Vals) == N,
			"fea::enum_set : Invalid number of arguments.");
	static_assert(fea::all_of<std::is_same<Vals, bool>...>::value,
			"fea::enum_set : Invalid constructor parameters, expected "
			"bools.");
}

template <class Enum, size_t N>
template <Enum E>
constexpr typename enum_set<Enum, N>::reference enum_set<Enum, N>::at() {
	return std::get<size_t(E)>(*this);
}

template <class Enum, size_t N>
template <Enum E>
constexpr typename enum_set<Enum, N>::const_reference
enum_set<Enum, N>::at() const {
	return std::get<size_t(E)>(*this);
}

template <class Enum, size_t N>
constexpr typename enum_set<Enum, N>::reference enum_set<Enum, N>::at(Enum e) {
	return array_t::at(size_t(e));
}

template <class Enum, size_t N>
constexpr typename enum_set<Enum, N>::const_reference enum_set<Enum, N>::at(
		Enum e) const {
	return array_t::at(size_t(e));
}

template <class Enum, size_t N>
constexpr typename enum_set<Enum, N>::reference enum_set<Enum, N>::operator[](
		Enum e) {
	return array_t::operator[](size_t(e));
}

template <class Enum, size_t N>
constexpr typename enum_set<Enum, N>::const_reference
enum_set<Enum, N>::operator[](Enum e) const {
	return array_t::operator[](size_t(e));
}

template <auto I, class E, size_t N>
constexpr bool& get(enum_set<E, N>& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_set : passed in wrong enum type");
	return std::get<size_t(I)>(a);
}

template <auto I, class E, size_t N>
constexpr bool&& get(enum_set<E, N>&& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_set : passed in wrong enum type");
	return std::get<size_t(I)>(std::move(a));
}

template <auto I, class E, size_t N>
constexpr const bool& get(const enum_set<E, N>& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_set : passed in wrong enum type");
	return std::get<size_t(I)>(a);
}

template <auto I, class E, size_t N>
constexpr const bool&& get(const enum_set<E, N>&& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_set : passed in wrong enum type");
	return std::get<size_t(I)>(std::move(a));
}

} // namespace fea