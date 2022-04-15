#pragma once
#include "fea/utils/platform.hpp"

#include <array>
#include <cassert>
#include <chrono>
#include <limits>
#include <random>
#include <type_traits>

namespace fea {
namespace detail {
template <size_t Bytes>
struct platform_mersenne;

template <>
struct platform_mersenne<4> {
	using type = std::mt19937;
};

template <>
struct platform_mersenne<8> {
	using type = std::mt19937_64;
};

using platform_mt19937 =
		typename detail::platform_mersenne<sizeof(size_t)>::type;

FEA_INLINE_VAR std::random_device rd;
FEA_INLINE_VAR platform_mt19937 gen(
		size_t(std::chrono::system_clock::now().time_since_epoch().count()));

} // namespace detail

// Get a random int, from lowest to highest int value.
template <class T>
T random_int() {
	std::uniform_int_distribution<T> dist(
			std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max());
	return dist(detail::gen);
}
template <>
int8_t random_int<int8_t>() {
	return int8_t(random_int<short>());
}
template <>
uint8_t random_int<uint8_t>() {
	return uint8_t(random_int<uint16_t>());
}

// Get a random int between [min, max].
template <class T>
T random_int(T min, T max) {
	std::uniform_int_distribution<T> dist(min, max);
	return dist(detail::gen);
}
template <>
int8_t random_int(int8_t min, int8_t max) {
	return int8_t(random_int(int16_t(min), int16_t(max)));
}
template <>
uint8_t random_int(uint8_t min, uint8_t max) {
	return uint8_t(random_int(uint16_t(min), uint16_t(max)));
}

// Get a random index, from 0 to count - 1.
inline size_t random_idx(size_t count) {
	std::uniform_int_distribution<size_t> dist(0, count - 1);
	return dist(detail::gen);
}

// Returns a random enum value between [min, max].
template <class E>
E random_enum(E min, E max) {
	static_assert(std::is_enum_v<E>, "E must be enum");
	using u_t = std::underlying_type_t<E>;
	return E(random_int(u_t(min), u_t(max)));
}

// Returns a random enum value between [E{}, E::count[.
// Undefined if 'count' == 0.
template <class E>
E random_enum() {
	static_assert(std::is_enum_v<E>, "E must be enum");
	using u_t = std::underlying_type_t<E>;
	assert(u_t(E::count) != 0);
	return random_enum(E{}, E(u_t(E::count) - 1));
}


template <size_t N>
std::array<uint8_t, N> random_bytes() {
	std::array<uint8_t, N> ret{};

	std::uniform_int_distribution<uint16_t> dist(
			0, std::numeric_limits<uint8_t>::max());

	for (size_t i = 0; i < N; ++i) {
		ret[i] = uint8_t(dist(detail::gen));
	}
	return ret;
}

inline std::vector<uint8_t> random_bytes(size_t num_bytes) {
	std::vector<uint8_t> ret;
	ret.reserve(num_bytes);

	std::uniform_int_distribution<uint16_t> dist(
			0, std::numeric_limits<uint8_t>::max());

	for (size_t i = 0; i < num_bytes; ++i) {
		ret.push_back(uint8_t(dist(detail::gen)));
	}
	return ret;
}

} // namespace fea
