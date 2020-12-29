#pragma once
#include <chrono>
#include <limits>
#include <random>
#include <type_traits>

// TODO : move to fea_libs
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

inline std::random_device rd;
inline platform_mt19937 gen(
		size_t(std::chrono::system_clock::now().time_since_epoch().count()));

} // namespace detail

template <class T>
T random_int() {
	std::uniform_int_distribution<T> dist(
			std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max());
	return dist(detail::gen);
}

template <class T>
T random_int(T min, T max) {
	std::uniform_int_distribution<T> dist(min, max);
	return dist(detail::gen);
}

inline size_t random_idx(size_t count) {
	std::uniform_int_distribution<size_t> dist(0, count - 1);
	return dist(detail::gen);
}

} // namespace fea
