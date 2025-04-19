/*
BSD 3-Clause License

Copyright (c) 2025, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "fea/meta/traits.hpp"
#include "fea/numerics/numerics.hpp"
#include "fea/utility/platform.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

/*
Statistics functions.

Notes :
Tries to use double precision for intermediate computations if possible.
*/

namespace fea {
// Computes the sum of items in range, predicate should return value to sum.
template <class FwdIt, class Func>
[[nodiscard]]
constexpr auto sum(FwdIt first, FwdIt last, Func&& func);

// Computes the sum of items in range.
template <class FwdIt>
[[nodiscard]]
constexpr auto sum(FwdIt first, FwdIt last);

// Computes the sum of items in container.
template <class Container>
[[nodiscard]]
constexpr auto sum(const Container& cont);

// Compute profit.
template <class T>
[[nodiscard]]
constexpr T profit(T gains, T cost);

// Compute Return On Investment.
template <class T>
[[nodiscard]]
constexpr T roi(T gains, T cost);

// Compute profit margin.
// https://www.investopedia.com/ask/answers/031815/what-formula-calculating-profit-margins.asp
template <class T>
[[nodiscard]]
constexpr T profit_margin(T gains, T cost);

// Compute mean (average).
// Provided function must return the value to average.
template <class FwdIt, class Func>
[[nodiscard]]
constexpr auto mean(FwdIt begin, FwdIt end, Func func);

// Compute mean (average).
template <class FwdIt>
[[nodiscard]]
constexpr auto mean(FwdIt begin, FwdIt end);

// Compute the median (middle value of given set).
// Provided callback must return desired value.
// Note : This function heap allocates. Values must be sortable.
template <class FwdIt, class Func>
[[nodiscard]]
constexpr auto median(FwdIt begin, FwdIt end, Func&& func);

// Compute the median (middle value of given set).
// Note : This function heap allocates. Values
// must be sortable.
template <class FwdIt>
[[nodiscard]]
constexpr auto median(FwdIt begin, FwdIt end);

// Compute the mode (the most common number in set).
// Returns a vector of highest frequency items,
// or an empty vector if no mode was found.
// Note : Heap allocates.
template <class FwdIt, class Func>
[[nodiscard]]
auto mode(FwdIt begin, FwdIt end, Func&& func);

// Compute the mode (the most common number in set).
// Returns a vector of the highest frequency items,
// or an empty vector if no mode was found.
// Note : Heap allocates.
template <class FwdIt>
[[nodiscard]]
auto mode(FwdIt begin, FwdIt end);


// Compute variance of values, sigma^2.
// Predicate function must return value to compute.
template <class FwdIt, class Func>
[[nodiscard]]
constexpr auto variance(FwdIt begin, FwdIt end, Func func);

// Compute variance of values, sigma^2.
template <class FwdIt>
[[nodiscard]]
constexpr auto variance(FwdIt begin, FwdIt end);

// Compute sample variance of values (Bessel's correction, divided by n
// - 1). Predicate function must return value to compute.
template <class FwdIt, class Func>
[[nodiscard]]
constexpr auto sample_variance(FwdIt begin, FwdIt end, Func func);

// Compute sample variance of values (Bessel's correction, divided by n - 1).
template <class FwdIt>
[[nodiscard]]
constexpr auto sample_variance(FwdIt begin, FwdIt end);

// Compute population standard deviation.
// Predicate function must return the values to compute.
template <class FwdIt, class Func>
[[nodiscard]]
constexpr auto std_deviation(FwdIt begin, FwdIt end, Func func);

// Compute sample standard deviation (Bessel's correction, divides by n - 1).
// Predicate function must return the values to compute.
template <class FwdIt, class Func>
[[nodiscard]]
auto sample_std_deviation(FwdIt begin, FwdIt end, Func func);

// Compute sample standard deviation (Bessel's correction, divides by n - 1).
template <class FwdIt>
[[nodiscard]]
auto sample_std_deviation(FwdIt begin, FwdIt end);


// Compute population standard deviation.
template <class FwdIt>
[[nodiscard]]
constexpr auto std_deviation(FwdIt begin, FwdIt end);

// Filters values above or below sigma * standard deviation.
// Your callback will be called with values that pass test.
// The ValuePredicate returns values to compute.
template <class FwdIt, class ValuePredicate, class T, class Func>
constexpr void sigma_filter(
		FwdIt begin, FwdIt end, T sigma, ValuePredicate v_pred, Func func);

// Filters values above or below sigma * standard deviation.
// Your callback will be called with values that pass test.
template <class FwdIt, class T, class Func>
constexpr void sigma_filter(FwdIt begin, FwdIt end, T sigma, Func func);

// Filters values above or below sigma * standard deviation.
// Uses sample standard deviation (Bussel's correction, divided by n - 1).
// Your callback will be called with values that pass test.
// The ValuePredicate returns values to compute.
template <class FwdIt, class ValuePredicate, class T, class Func>
void sample_sigma_filter(
		FwdIt begin, FwdIt end, T sigma, ValuePredicate v_pred, Func func);

// Filters values above or below sigma * standard deviation.
// Uses sample standard deviation (Bussel's correction, divided by n - 1).
// Your callback will be called with values that pass test.
template <class FwdIt, class T, class Func>
void sample_sigma_filter(FwdIt begin, FwdIt end, T sigma, Func func);

// Computes the factorial of n.
template <class T>
[[nodiscard]]
constexpr T factorial(T n);

// Computes the factorial of n.
template <class T>
[[nodiscard]]
constexpr T fact(T n);

// Computes the binomial coefficient given (n k).
template <class T>
[[nodiscard]]
constexpr T binomial_coeff(T n, T k);

// Computes stars and bars for positive values (> 0).
// https://en.wikipedia.org/wiki/Stars_and_bars_%28combinatorics%29
template <class T>
[[nodiscard]]
constexpr T stars_and_bars_pos(T n, T k);

// Computes stars and bars for non-negative values (>= 0).
// https://en.wikipedia.org/wiki/Stars_and_bars_%28combinatorics%29
template <class T>
[[nodiscard]]
constexpr T stars_and_bars_zero(T n, T k);

// Computes simple linear regression (2d) on values returned by func.
// https://en.wikipedia.org/wiki/Simple_linear_regression
// Given 2 dimensional values, outputs a and b respectively.
//
// Returned values should support std::get<0> and std::get<1>,
// or the equivalent in your namespace.
template <class FwdIt, class Func>
[[nodiscard]]
fea::iterator_value_t<FwdIt> simple_linear_regression(
		FwdIt first, FwdIt last, Func&& func);

// Computes simple linear regression (2d)
// https://en.wikipedia.org/wiki/Simple_linear_regression
// Given 2 dimensional values, outputs a and b respectively.
//
// Iterator values should support std::get<0> and std::get<1>,
// or the equivalent in your namespace.
template <class FwdIt>
[[nodiscard]]
fea::iterator_value_t<FwdIt> simple_linear_regression(FwdIt first, FwdIt last);
} // namespace fea


// Implementation
namespace fea {
namespace detail {
template <class To, class From>
constexpr To maybe_round(const From& from) {
	// You can add a round overload in your namespace if your custom type is to
	// be treated as an integral. Also specialize std::is_integral.
	using std::round;
	if constexpr (std::is_integral_v<To>) {
		return To(round(from));
	} else {
		return To(from);
	}
}
} // namespace detail

template <class FwdIt, class Func>
[[nodiscard]]
constexpr auto sum(FwdIt first, FwdIt last, Func&& func) {
	using T = std::decay_t<decltype(func(*first))>;
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;

	cast_t ret(0);
	for (auto it = first; it != last; ++it) {
		ret += cast_t(func(*it));
	}
	return detail::maybe_round<T>(ret);
}

template <class FwdIt>
[[nodiscard]]
constexpr auto sum(FwdIt first, FwdIt last) {
	return sum(first, last, [](const auto& v) { return v; });
}

template <class Container>
[[nodiscard]]
constexpr auto sum(const Container& cont) {
	return sum(cont.begin(), cont.end());
}

template <class T>
constexpr T profit(T gains, T cost) {
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;
	cast_t ret = cast_t(gains) - cast_t(cost);
	return detail::maybe_round<T>(ret);
}

template <class T>
constexpr T roi(T gains, T cost) {
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;
	cast_t ret = cast_t(profit(gains, cost)) / cast_t(cost);
	return detail::maybe_round<T>(ret);
}

template <class T>
constexpr T profit_margin(T gains, T cost) {
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;
	cast_t ret = cast_t(profit(gains, cost)) / cast_t(gains);
	return detail::maybe_round<T>(ret);
}

template <class FwdIt, class Func>
constexpr auto mean(FwdIt begin, FwdIt end, Func func) {
	using T = std::decay_t<decltype(func(*begin))>;
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;

	cast_t count = cast_t(std::distance(begin, end));
	if (count == cast_t(0)) {
		return T(0);
	}

	cast_t ret(0);
	for (auto it = begin; it != end; ++it) {
		ret += cast_t(func(*it));
	}
	return detail::maybe_round<T>(ret / count);
}

template <class FwdIt>
constexpr auto mean(FwdIt begin, FwdIt end) {
	return mean(begin, end, [](const auto& v) -> const auto& { return v; });
}

template <class FwdIt, class Func>
constexpr auto median(FwdIt begin, FwdIt end, Func&& func) {
	using T = std::decay_t<decltype(func(*begin))>;
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;

	std::vector<T> vals;
	vals.reserve(std::distance(begin, end));
	for (auto it = begin; it != end; ++it) {
		vals.push_back(func(*it));
	}
	std::sort(vals.begin(), vals.end());

	if (vals.size() % 2 == 0) {
		// Even set, average middle values.
		const T& v1 = vals[(vals.size() / 2) - 1];
		const T& v2 = vals[vals.size() / 2];
		return detail::maybe_round<T>((cast_t(v1) + cast_t(v2)) / cast_t(2));
	}

	return vals[(vals.size() - 1) / 2];
}

template <class FwdIt>
constexpr auto median(FwdIt begin, FwdIt end) {
	return median(begin, end, [](const auto& v) -> const auto& { return v; });
}

template <class FwdIt, class Func>
auto mode(FwdIt begin, FwdIt end, Func&& func) {
	using T = std::decay_t<decltype(func(*begin))>;

	size_t num = std::distance(begin, end);
	if (num == 0) {
		return std::vector<T>{};
	}
	if (num == 1) {
		return std::vector<T>{ func(*begin) };
	}

	// Copy iters in candidate vec.
	// std::vector<std::remove_cv_t<FwdIt>> candidates;
	std::vector<T> candidates;
	candidates.reserve(num);
	for (auto it = begin; it != end; ++it) {
		candidates.push_back(func(*it));
	}

	// Stores : { value count, iter index }.
	std::vector<std::pair<size_t, size_t>> counts;

	// Count candidates, and remove duplicates.
	for (size_t i = 0; i < candidates.size(); ++i) {
		const T& c = candidates[i];
		counts.push_back({ i, 1u });

		auto new_end = std::remove_if(
				candidates.begin() + i + 1, candidates.end(), [&](const T& v) {
					if (v == c) {
						++counts[i].second;
						return true;
					}
					return false;
				});
		candidates.erase(new_end, candidates.end());
	}

	// Now reverse sort counts to find highest frequency numbers.
	std::sort(counts.begin(), counts.end(),
			[](const std::pair<size_t, size_t>& lhs,
					const std::pair<size_t, size_t>& rhs) {
				return lhs.second > rhs.second;
			});

	// The final candidates are the front values who's counts are equal.
	// If first count is 1, no mode found.
	if (counts.front().second == 1u) {
		return std::vector<T>{};
	}

	std::vector<T> ret;
	size_t max_count = counts.front().second;
	for (const std::pair<size_t, size_t>& c : counts) {
		if (c.second != max_count) {
			break;
		}
		ret.push_back(candidates[c.first]);
	}
	return ret;
}

template <class FwdIt>
auto mode(FwdIt begin, FwdIt end) {
	return mode(begin, end, [](const auto& v) -> const auto& { return v; });
}

namespace detail {
// Computes variance numerator.
template <class FwdIt, class Func>
constexpr auto variance_num_imp(FwdIt begin, FwdIt end, Func func) {
	using T = std::decay_t<decltype(func(*begin))>;
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;

	cast_t avg{};
	if constexpr (!std::is_same_v<T, cast_t>) {
		// Converts the return value of user 'func' to biggest float type.
		// Keeps as much precision as possible when computing mean.
		avg = fea::mean(
				begin, end, [&](const auto& v) { return cast_t(func(v)); });
	} else {
		avg = fea::mean(begin, end, func);
	}

	cast_t ret(0);
	for (auto it = begin; it != end; ++it) {
		if constexpr (!std::is_same_v<T, cast_t>) {
			cast_t v = cast_t(func(*it)) - avg;
			ret += v * v;
		} else {
			auto v = func(*it) - avg;
			ret += v * v;
		}
	}
	return ret;
}
} // namespace detail

template <class FwdIt, class Func>
constexpr auto variance(FwdIt begin, FwdIt end, Func func) {
	using T = std::decay_t<decltype(func(*begin))>;
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;

	cast_t count = cast_t(std::distance(begin, end));
	if (count == cast_t(0)) {
		return T(0);
	}
	cast_t ret = detail::variance_num_imp(begin, end, func);
	return detail::maybe_round<T>(ret / count);
}

template <class FwdIt>
constexpr auto variance(FwdIt begin, FwdIt end) {
	return variance(begin, end, [](const auto& v) -> const auto& { return v; });
}

template <class FwdIt, class Func>
constexpr auto sample_variance(FwdIt begin, FwdIt end, Func func) {
	using T = std::decay_t<decltype(func(*begin))>;
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;

	cast_t count = cast_t(std::distance(begin, end));
	if (count <= cast_t(1)) {
		return T(0);
	}
	cast_t ret = detail::variance_num_imp(begin, end, func);
	return detail::maybe_round<T>(ret / (count - cast_t(1)));
}

template <class FwdIt>
constexpr auto sample_variance(FwdIt begin, FwdIt end) {
	return sample_variance(
			begin, end, [](const auto& v) -> const auto& { return v; });
}


template <class FwdIt, class Func>
constexpr auto std_deviation(FwdIt begin, FwdIt end, Func func) {
	using T = std::decay_t<decltype(func(*begin))>;
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;

	// You can customize sqrt for your custom types.
	using std::sqrt;

	if constexpr (!std::is_same_v<T, cast_t>) {
		auto mfunc = [&](const auto& v) { return cast_t(func(v)); };
		cast_t ret = sqrt(variance(begin, end, mfunc));
		return detail::maybe_round<T>(ret);
	} else {
		static_assert(!std::is_integral_v<T>,
				"fea::std_deviation : should never happen");
		return T(sqrt(variance(begin, end, func)));
	}
}

template <class FwdIt>
constexpr auto std_deviation(FwdIt begin, FwdIt end) {
	return std_deviation(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

template <class FwdIt, class Func>
auto sample_std_deviation(FwdIt begin, FwdIt end, Func func) {
	using T = std::decay_t<decltype(func(*begin))>;
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;

	// You can customize sqrt for your custom types.
	using std::sqrt;

	if constexpr (!std::is_same_v<T, cast_t>) {
		auto mfunc = [&](const auto& v) { return cast_t(func(v)); };
		cast_t ret = sqrt(sample_variance(begin, end, mfunc));
		return detail::maybe_round<T>(ret);
	} else {
		static_assert(!std::is_integral_v<T>,
				"fea::std_deviation : should never happen");
		return T(sqrt(sample_variance(begin, end, func)));
	}
}

template <class FwdIt>
auto sample_std_deviation(FwdIt begin, FwdIt end) {
	return sample_std_deviation(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

namespace detail {
template <bool Sample, class FwdIt, class ValuePredicate, class SigmaT,
		class Func>
constexpr void sigma_filter_imp(FwdIt begin, FwdIt end, SigmaT sigma,
		ValuePredicate v_pred, Func func) {
	using T = std::decay_t<decltype(v_pred(*begin))>;
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;

	// Mean and standard deviation.
	cast_t avg{};
	cast_t std_dev{};

	auto mv_pred = [&](const auto& v) { return cast_t(v_pred(v)); };
	avg = fea::mean(begin, end, mv_pred);
	if constexpr (Sample) {
		std_dev = fea::sample_std_deviation(begin, end, mv_pred);
	} else {
		std_dev = fea::std_deviation(begin, end, mv_pred);
	}

	cast_t msigma = cast_t(sigma);
	cast_t high_benchmark = avg + msigma * std_dev;
	cast_t low_benchmark = avg - msigma * std_dev;

	for (auto it = begin; it != end; ++it) {
		cast_t val = cast_t(v_pred(*it));
		if (low_benchmark < val && val < high_benchmark) {
			func(*it);
		}
	}
}
} // namespace detail

template <class FwdIt, class ValuePredicate, class T, class Func>
constexpr void sigma_filter(
		FwdIt begin, FwdIt end, T sigma, ValuePredicate v_pred, Func func) {
	detail::sigma_filter_imp<false>(begin, end, sigma, v_pred, func);
}

template <class FwdIt, class T, class Func>
constexpr void sigma_filter(FwdIt begin, FwdIt end, T sigma, Func func) {
	return sigma_filter(
			begin, end, sigma, [](const auto& v) -> const auto& { return v; },
			func);
}

template <class FwdIt, class ValuePredicate, class T, class Func>
void sample_sigma_filter(
		FwdIt begin, FwdIt end, T sigma, ValuePredicate v_pred, Func func) {
	detail::sigma_filter_imp<true>(begin, end, sigma, v_pred, func);
}

template <class FwdIt, class T, class Func>
void sample_sigma_filter(FwdIt begin, FwdIt end, T sigma, Func func) {
	return sample_sigma_filter(
			begin, end, sigma, [](const auto& v) -> const auto& { return v; },
			func);
}

template <class T>
constexpr T factorial(T n) {
	assert(n >= T(0));

	size_t ret = 1;
	size_t count = size_t(n);
	for (size_t i = 2; i <= count; ++i) {
		ret *= i;
	}
	return T(ret);
}

template <class T>
constexpr T fact(T n) {
	return factorial(n);
}

template <class T>
constexpr T binomial_coeff(T n, T k) {
	assert(n >= k && n > 0);
	return fact(n) / (fact(k) * fact(n - k));
}

template <class T>
constexpr T stars_and_bars_pos(T n, T k) {
	return binomial_coeff(n - 1, k - 1);
}

template <class T>
constexpr T stars_and_bars_zero(T n, T k) {
	return binomial_coeff(n + k - 1, k - 1);
}

template <class FwdIt, class Func>
fea::iterator_value_t<FwdIt> simple_linear_regression(
		FwdIt first, FwdIt last, Func&& func) {
	// You can customize std::get for your types in your namespace.
	using std::get;

	using vec_t = std::decay_t<decltype(func(*first))>;
	using T = std::decay_t<decltype(get<0>(std::declval<vec_t>()))>;
	using cast_t = std::conditional_t<fea::is_static_castable_v<T, floatmax_t>,
			floatmax_t, T>;

	cast_t count = cast_t(std::distance(first, last));
	if (count == cast_t(0)) {
		return {};
	}

	// Compute the means.
	cast_t xmean(0);
	cast_t ymean(0);
	for (auto it = first; it != last; ++it) {
		decltype(func(*it)) pt = func(*it);
		xmean += cast_t(get<0>(pt));
		ymean += cast_t(get<1>(pt));
	}
	xmean /= count;
	ymean /= count;

	// Compute intermediates.
	cast_t num(0);
	cast_t denom(0);
	for (auto it = first; it != last; ++it) {
		decltype(func(*it)) pt = func(*it);
		num += (cast_t(get<0>(pt)) - xmean) * (cast_t(get<1>(pt)) - ymean);
		cast_t d = get<0>(pt) - xmean;
		denom += d * d;
	}

	// Compute a and b.
	cast_t b = num;
	if (denom == cast_t(0)) {
		b = cast_t(0);
	} else {
		b /= denom;
	}
	cast_t a = ymean - (b * xmean);

	return { detail::maybe_round<T>(a), detail::maybe_round<T>(b) };
}

template <class FwdIt>
fea::iterator_value_t<FwdIt> simple_linear_regression(FwdIt first, FwdIt last) {
	return simple_linear_regression(
			first, last, [](const auto& val) -> const auto& { return val; });
}

} // namespace fea
