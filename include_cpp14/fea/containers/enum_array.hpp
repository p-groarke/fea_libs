#pragma once
#include "fea/utils/platform.hpp"

#include <array>

/*
enum_array is a wrapper on std::array, which allows to access elements with
an enum class directly. Your enum must be from 0 to N!

If your enum declares 'count', you do not need to provide the size.

enum_array doesn't change anything to std::array other than overriding the
accessors (at, operator[] and std::get) to accept your enums directly.
*/

namespace fea {

template <class T, class Enum, size_t N = size_t(Enum::count)>
struct enum_array : public std::array<T, N> {
	using array_t = std::array<T, N>;
	using reference = typename array_t::reference;
	using const_reference = typename array_t::const_reference;

	constexpr reference at(Enum e) {
		return array_t::at(size_t(e));
	}
	constexpr const_reference at(Enum e) const {
		return array_t::at(size_t(e));
	}

	constexpr reference operator[](Enum e) {
		return array_t::operator[](size_t(e));
	}
	constexpr const_reference operator[](Enum e) const {
		return array_t::operator[](size_t(e));
	}
};

#if FEA_CPP17
template <auto I, class T, class E, size_t N>
constexpr T& get(enum_array<T, E, N>& a) noexcept {
	return std::get<size_t(I)>(a);
}
template <auto I, class T, class E, size_t N>
constexpr T&& get(enum_array<T, E, N>&& a) noexcept {
	return std::get<size_t(I)>(std::move(a));
}
template <auto I, class T, class E, size_t N>
constexpr const T& get(const enum_array<T, E, N>& a) noexcept {
	return std::get<size_t(I)>(a);
}
template <auto I, class T, class E, size_t N>
constexpr const T&& get(const enum_array<T, E, N>&& a) noexcept {
	return std::get<size_t(I)>(std::move(a));
}
#endif

} // namespace fea
