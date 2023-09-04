#pragma once
#include "fea/utils/platform.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <limits>
#include <numeric>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

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

// Get a random int between [min, max].
template <class T>
std::enable_if_t<std::is_integral<T>::value, T> random_val(T min, T max) {
	std::uniform_int_distribution<T> dist(min, max);
	return dist(detail::gen);
}

// Get a random float between [min, max].
template <class T>
std::enable_if_t<std::is_floating_point<T>::value, T> random_val(T min, T max) {
	std::uniform_real_distribution<T> dist(min, max);
	return dist(detail::gen);
}

// Get a random enum between [min, max].
template <class T>
std::enable_if_t<std::is_enum<T>::value, T> random_val(T min, T max) {
	using u_t = std::underlying_type_t<T>;
	return T(random_val(u_t(min), u_t(max)));
}

// Get a random char between [min, max].
template <>
inline char random_val<char>(char min, char max) {
	return char(random_val(short(min), short(max)));
}

// Get a random unsigned char between [min, max].
template <>
inline uint8_t random_val<uint8_t>(uint8_t min, uint8_t max) {
	return uint8_t(random_val(uint16_t(min), uint16_t(max)));
}


// Get a random value between [T::min, T::max].
template <class T>
std::enable_if_t<!std::is_enum<T>::value, T> random_val() {
	return random_val(
			std::numeric_limits<T>::lowest(), (std::numeric_limits<T>::max)());
}

// Get a random enum between [E{}, E::count[.
template <class E>
std::enable_if_t<std::is_enum<E>::value, E> random_val() {
	using u_t = std::underlying_type_t<E>;
	return random_val(E{}, E(u_t(E::count) - 1));
}

// Get a random value between [T::min, T::max].
template <>
inline char random_val<char>() {
	constexpr short mmin = short(std::numeric_limits<char>::lowest());
	constexpr short mmax = short((std::numeric_limits<char>::max)());
	return char(random_val(mmin, mmax));
}

// Get a random value between [T::min, T::max].
template <>
inline uint8_t random_val<uint8_t>() {
	constexpr uint16_t mmin = uint16_t(std::numeric_limits<uint8_t>::lowest());
	constexpr uint16_t mmax = uint16_t((std::numeric_limits<uint8_t>::max)());
	return uint8_t(random_val(mmin, mmax));
}

// Get a random bool [true, false].
template <>
inline bool random_val<bool>() {
	return bool(random_val(uint8_t(0), uint8_t(1)));
}

// Fills a range, from begin to end, with random values between [min, max].
// Iterator value type should be supported by fea::random_val.
template <class ForwardIt,
		class T = typename std::iterator_traits<ForwardIt>::value_type>
void random_fill(ForwardIt begin, ForwardIt end, T min, T max) {
	for (; begin != end; ++begin) {
		*begin = fea::random_val(min, max);
	}
}

// Fills a range, from begin to end, with random values between
// [numeric_limits::lowest, numeric_limits::max].
// Iterator value type should be supported by fea::random_val.
template <class ForwardIt>
void random_fill(ForwardIt begin, ForwardIt end) {
	using value_t = typename std::iterator_traits<ForwardIt>::value_type;
	random_fill(begin, end, std::numeric_limits<value_t>::lowest(),
			(std::numeric_limits<value_t>::max)());
}

// Get a random index, [0, count[
inline size_t random_idx(size_t count) {
	return random_val(size_t(0), count - 1);
}

// Get a random iterator, [first, last[
template <class FwdIt>
inline FwdIt random_iter(FwdIt first, FwdIt last) {
	size_t count = std::distance(first, last);
	size_t idx = random_idx(count);
	return std::next(first, idx);
}

// Fills the range with random indexes from [0, count[
template <class FwdIt>
void random_idxes(FwdIt begin, FwdIt end, size_t count) {
	for (auto it = begin; it != end; ++it) {
		*it = fea::random_idx(count);
	}
}

// Get a random value from container.
template <class Container>
inline const auto& random_val(const Container& container) {
	return *random_iter(container.begin(), container.end());
}

// Get a random value from container.
template <class Container>
inline auto& random_val(Container& container) {
	return *random_iter(container.begin(), container.end());
}

// Get some random bytes.
template <size_t N>
std::array<uint8_t, N> random_bytes() {
	std::array<uint8_t, N> ret{};
	for (size_t i = 0; i < N; ++i) {
		ret[i] = random_val<uint8_t>();
	}
	return ret;
}

// Get some random bytes.
inline std::vector<uint8_t> random_bytes(size_t num_bytes) {
	std::vector<uint8_t> ret;
	ret.reserve(num_bytes);
	for (size_t i = 0; i < num_bytes; ++i) {
		ret.push_back(random_val<uint8_t>());
	}
	return ret;
}

// Shuffles items in range [begin, end[
template <class ForwardIt>
void random_shuffle(ForwardIt begin, ForwardIt end) {
	std::shuffle(begin, end, detail::gen);
}

// Shuffles items in container.
template <class Container>
void random_shuffle(Container& cont) {
	return fea::random_shuffle(cont.begin(), cont.end());
}

// Given the available sum, fills your range with n values
// so that : v0 + v1 + ... + v(n-1) == sum.
// https://stackoverflow.com/a/8064754
// The iterators must return the same type as sum.
// The range must be sortable.
template <class BidirIt, class T>
void random_fixed_sum(BidirIt begin, BidirIt end, T sum) {
	static_assert(std::is_same<std::decay_t<decltype(*begin)>, T>::value,
			"fea::random_from_available : iterators must return same type as "
			"sum");
	assert(std::distance(begin, end) >= 2);

	// First, generate n - 1 random numbers that are <= sum.
	for (auto it = begin; it != end - 1; ++it) {
		*it = random_val(T(0), sum);
	}

	// Assign the sum to last.
	*(end - 1) = sum;

	// Then sort it.
	std::sort(begin, end);

	// Now, going backwards (so as to not erase our values),
	// assign our final number. We do that by substracting with
	// the previous number.
	auto rbegin = std::make_reverse_iterator(end);
	auto rend = std::make_reverse_iterator(begin);
	for (auto it = rbegin; it != rend - 1; ++it) {
		*it = *it - *(it + 1);
	}
}

// Given the available sum, returns a vector with 'count' values
// so that : v0 + v1 + ... + v(n-1) == 'sum'.
// https://stackoverflow.com/a/8064754
// Returns a vector filled with 'count' values.
template <class T>
std::vector<T> random_fixed_sum(T sum, size_t count) {
	std::vector<T> ret(count);
	random_fixed_sum(ret, sum);
	return ret;
}

// Given the available sum, returns a vector with 'count' values
// so that : v0 + v1 + ... + v(n-1) == 'sum'.
// https://stackoverflow.com/a/8064754
template <size_t N, class T>
std::array<T, N> random_fixed_sum(T sum) {
	std::array<T, N> ret{};
	random_fixed_sum(ret.begin(), ret.end(), sum);
	return ret;
}

} // namespace fea
