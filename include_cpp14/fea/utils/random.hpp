#pragma once
#include "fea/utils/platform.hpp"

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

// Get a random int between [min, max].
template <class T>
T random_int(T min, T max) {
	std::uniform_int_distribution<T> dist(min, max);
	return dist(detail::gen);
}

// Get a random index, from 0 to count - 1.
inline size_t random_idx(size_t count) {
	std::uniform_int_distribution<size_t> dist(0, count - 1);
	return dist(detail::gen);
}

inline std::vector<uint8_t> random_bytes(size_t num_bytes) {
	std::vector<uint8_t> ret;
	ret.reserve(num_bytes);
	std::uniform_int_distribution<uint8_t> dist(
			0, std::numeric_limits<uint8_t>::max());

	for (size_t i = 0; i < num_bytes; ++i) {
		ret.push_back(dist(detail::gen));
	}
	return ret;
}

} // namespace fea
