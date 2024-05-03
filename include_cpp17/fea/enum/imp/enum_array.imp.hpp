#pragma once

namespace fea {
template <class T, class Enum, size_t N>
template <Enum E>
constexpr typename enum_array<T, Enum, N>::reference
enum_array<T, Enum, N>::at() {
	return std::get<size_t(E)>(*this);
}

template <class T, class Enum, size_t N>
template <Enum E>
constexpr typename enum_array<T, Enum, N>::const_reference
enum_array<T, Enum, N>::at() const {
	return std::get<size_t(E)>(*this);
}

template <class T, class Enum, size_t N>
constexpr typename enum_array<T, Enum, N>::reference enum_array<T, Enum, N>::at(
		Enum e) {
	return array_t::at(size_t(e));
}

template <class T, class Enum, size_t N>
constexpr typename enum_array<T, Enum, N>::const_reference
enum_array<T, Enum, N>::at(Enum e) const {
	return array_t::at(size_t(e));
}

template <class T, class Enum, size_t N>
constexpr typename enum_array<T, Enum, N>::reference
enum_array<T, Enum, N>::operator[](Enum e) {
	return array_t::operator[](size_t(e));
}

template <class T, class Enum, size_t N>
constexpr typename enum_array<T, Enum, N>::const_reference
enum_array<T, Enum, N>::operator[](Enum e) const {
	return array_t::operator[](size_t(e));
}


template <auto I, class T, class E, size_t N>
constexpr T& get(enum_array<T, E, N>& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_array : passed in wrong enum type");
	return std::get<size_t(I)>(a);
}

template <auto I, class T, class E, size_t N>
constexpr T&& get(enum_array<T, E, N>&& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_array : passed in wrong enum type");
	return std::get<size_t(I)>(std::move(a));
}

template <auto I, class T, class E, size_t N>
constexpr const T& get(const enum_array<T, E, N>& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_array : passed in wrong enum type");
	return std::get<size_t(I)>(a);
}

template <auto I, class T, class E, size_t N>
constexpr const T&& get(const enum_array<T, E, N>&& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_array : passed in wrong enum type");
	return std::get<size_t(I)>(std::move(a));
}

} // namespace fea