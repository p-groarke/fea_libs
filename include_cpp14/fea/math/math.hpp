#pragma once
#include "fea/utils/platform.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

namespace fea {
// Computes the sum of items in container.
template <class Container>
FEA_NODISCARD constexpr auto sum(const Container& cont) {
	using T = typename Container::value_type;
	return std::accumulate(cont.begin(), cont.end(), T(0));
}

// Compute profit.
template <class T>
FEA_NODISCARD constexpr T profit(T gains, T cost) {
	return gains - cost;
}

// Compute Return On Investment.
template <class T>
FEA_NODISCARD constexpr T roi(T gains, T cost) {
	return profit(gains, cost) / cost;
}

// Compute profit margin.
// https://www.investopedia.com/ask/answers/031815/what-formula-calculating-profit-margins.asp
template <class T>
FEA_NODISCARD constexpr T profit_margin(T gains, T cost) {
	return profit(gains, cost) / gains;
}

// Compute mean (average).
// Provided function must return the value to average.
template <class FwdIt, class Func>
FEA_NODISCARD constexpr auto mean(FwdIt begin, FwdIt end, Func func) {
	using type_t = std::decay_t<decltype(func(*begin))>;

	double num = double(std::distance(begin, end));
	if (num == 0.0) {
		return type_t(0);
	}

	type_t ret(0);
	for (auto it = begin; it != end; ++it) {
		ret += func(*it);
	}

	return type_t(ret / num);
}

// Compute mean (average).
template <class FwdIt>
FEA_NODISCARD constexpr auto mean(FwdIt begin, FwdIt end) {
	return mean(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

// Compute the median (middle value of given set).
// Provided callback must return desired value.
// Note : This function heap allocates. Values must be sortable.
template <class FwdIt, class Func>
FEA_NODISCARD constexpr auto median(FwdIt begin, FwdIt end, Func&& func) {
	using T = std::decay_t<decltype(func(*begin))>;
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
		return T((v1 + v2) / 2.0);
	}

	return vals[(vals.size() - 1) / 2];
}

// Compute the median (middle value of given set).
// Note : This function heap allocates. Values
// must be sortable.
template <class FwdIt>
FEA_NODISCARD constexpr auto median(FwdIt begin, FwdIt end) {
	return median(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

// Compute the mode (the most common number in set).
// Returns a vector of iterators pointing to the highest frequency numbers,
// or an empty vector if no mode was found.
// O(n^2) for memory concearns.
template <class FwdIt, class Func>
FEA_NODISCARD constexpr std::vector<FwdIt> mode(
		FwdIt begin, FwdIt end, Func&& func) {
	using T = std::decay_t<decltype(func(*begin))>;

	size_t num = std::distance(begin, end);
	if (num == 0) {
		return {};
	}
	if (num == 1) {
		return { begin };
	}

	// Copy iters in candidate vec.
	// std::vector<std::remove_cv_t<FwdIt>> candidates;
	std::vector<FwdIt> candidates;
	candidates.reserve(num);
	for (auto it = begin; it != end; ++it) {
		candidates.push_back(it);
	}

	// Stores : { value count, iter index }.
	std::vector<std::pair<size_t, size_t>> counts;

	// Count candidates, and remove duplicates.
	for (size_t i = 0; i < candidates.size(); ++i) {
		FwdIt c = candidates[i];
		counts.push_back({ i, 1u });

		auto new_end = std::remove_if(candidates.begin() + i + 1,
				candidates.end(), [&](const auto& v) {
					if (*v == *c) {
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
		return {};
	}

	std::vector<FwdIt> ret;
	size_t max_count = counts.front().second;
	for (const std::pair<size_t, size_t>& c : counts) {
		if (c.second != max_count) {
			break;
		}
		ret.push_back(candidates[c.first]);
	}
	return ret;
}

// Compute the mode (the most common number in set).
// Returns a vector of iterators pointing to the highest frequency numbers,
// or an empty vector if no mode was found.
// O(n^2) for memory concearns.
template <class FwdIt>
FEA_NODISCARD constexpr std::vector<FwdIt> mode(FwdIt begin, FwdIt end) {
	return mode(
			begin, end, [](const auto& v) -> const auto& { return v; });
}


// Compute variance of values, sigma^2.
// Predicate function must return value to compute.
template <class FwdIt, class Func>
FEA_NODISCARD constexpr auto variance(FwdIt begin, FwdIt end, Func func) {
	using type_t = std::decay_t<decltype(func(*begin))>;

	double num = double(std::distance(begin, end));
	if (num == 0.0) {
		return type_t(0);
	}

	type_t avg = fea::mean(begin, end, func);

	type_t ret(0);
	for (auto it = begin; it != end; ++it) {
		type_t v = func(*it) - avg;
		ret += v * v;
	}

	return type_t(ret / num);
}

// Compute variance of values, sigma^2.
template <class FwdIt>
FEA_NODISCARD constexpr auto variance(FwdIt begin, FwdIt end) {
	return variance(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

// Compute population standard deviation.
// Predicate function must return the values to compute.
template <class FwdIt, class Func>
FEA_NODISCARD constexpr auto std_deviation(FwdIt begin, FwdIt end, Func func) {
	using type_t = std::decay_t<decltype(func(*begin))>;
	return type_t(std::sqrt(double(variance(begin, end, func))));
}

// Compute population standard deviation.
template <class FwdIt>
FEA_NODISCARD constexpr auto std_deviation(FwdIt begin, FwdIt end) {
	return std_deviation(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

// Filters values above or below sigma * standard deviation.
// Your callback will be called with values that pass test.
// The ValuePredicate returns values to compute.
template <class FwdIt, class ValuePredicate, class T, class Func>
constexpr void sigma_filter(
		FwdIt begin, FwdIt end, T sigma, ValuePredicate v_pred, Func func) {
	// Compute mean.
	auto avg = fea::mean(begin, end, v_pred);
	// Compute standard deviation.
	auto std_dev = fea::std_deviation(begin, end, v_pred);

	auto high_benchmark = avg + sigma * std_dev;
	auto low_benchmark = avg - sigma * std_dev;

	for (auto it = begin; it != end; ++it) {
		decltype(v_pred(*it)) val = v_pred(*it);
		if (low_benchmark < val && val < high_benchmark) {
			func(*it);
		}
	}
}

// Filters values above or below sigma * standard deviation.
// Your callback will be called with values that pass test.
template <class FwdIt, class T, class Func>
constexpr void sigma_filter(FwdIt begin, FwdIt end, T sigma, Func func) {
	return sigma_filter(
			begin, end, sigma, [](const auto& v) -> const auto& { return v; },
			func);
}


// Compute sample variance of values (Bessel's correction, divided by n
// - 1). Predicate function must return value to compute.
template <class FwdIt, class Func>
FEA_NODISCARD constexpr auto sample_variance(
		FwdIt begin, FwdIt end, Func func) {
	using type_t = std::decay_t<decltype(func(*begin))>;

	double num = double(std::distance(begin, end));
	if (num <= 1.0) {
		return type_t(0);
	}

	type_t avg = fea::mean(begin, end, func);

	type_t ret(0);
	for (auto it = begin; it != end; ++it) {
		type_t v = func(*it) - avg;
		ret += v * v;
	}

	return type_t(ret / (num - 1));
}

// Compute sample variance of values (Bessel's correction, divided by n - 1).
template <class FwdIt>
FEA_NODISCARD constexpr auto sample_variance(FwdIt begin, FwdIt end) {
	return sample_variance(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

// Compute sample standard deviation (Bessel's correction, divides by n - 1).
// Predicate function must return the values to compute.
template <class FwdIt, class Func>
FEA_NODISCARD auto sample_std_deviation(FwdIt begin, FwdIt end, Func func) {
	using type_t = std::decay_t<decltype(func(*begin))>;
	return type_t(std::sqrt(double(sample_variance(begin, end, func))));
}

// Compute sample standard deviation (Bessel's correction, divides by n - 1).
template <class FwdIt>
FEA_NODISCARD auto sample_std_deviation(FwdIt begin, FwdIt end) {
	return sample_std_deviation(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

// Filters values above or below sigma * standard deviation.
// Uses sample standard deviation (Bussel's correction, divided by n - 1).
// Your callback will be called with values that pass test.
// The ValuePredicate returns values to compute.
template <class FwdIt, class ValuePredicate, class T, class Func>
void sample_sigma_filter(
		FwdIt begin, FwdIt end, T sigma, ValuePredicate v_pred, Func func) {
	// Compute mean.
	auto avg = fea::mean(begin, end, v_pred);
	// Compute standard deviation.
	auto std_dev = fea::sample_std_deviation(begin, end, v_pred);

	auto high_benchmark = avg + sigma * std_dev;
	auto low_benchmark = avg - sigma * std_dev;

	for (auto it = begin; it != end; ++it) {
		decltype(v_pred(*it)) val = v_pred(*it);
		if (low_benchmark < val && val < high_benchmark) {
			func(*it);
		}
	}
}

// Filters values above or below sigma * standard deviation.
// Uses sample standard deviation (Bussel's correction, divided by n - 1).
// Your callback will be called with values that pass test.
template <class FwdIt, class T, class Func>
void sample_sigma_filter(FwdIt begin, FwdIt end, T sigma, Func func) {
	return sample_sigma_filter(
			begin, end, sigma, [](const auto& v) -> const auto& { return v; },
			func);
}

// Computes the factorial of n.
template <class T>
FEA_NODISCARD constexpr T factorial(T n) {
	assert(n >= T(0));

	T ret = T(1);
	for (T i = T(1); i <= n; ++i) {
		ret *= i;
	}
	return ret;
}

// Computes the factorial of n.
template <class T>
FEA_NODISCARD constexpr T fact(T n) {
	return factorial(n);
}

// Computes the binomial coefficient given (n k).
template <class T>
FEA_NODISCARD constexpr T binomial_coeff(T n, T k) {
	assert(n >= k && n > 0);
	return fact(n) / (fact(k) * fact(n - k));
}

// Computes stars and bars for positive values (> 0).
// https://en.wikipedia.org/wiki/Stars_and_bars_%28combinatorics%29
template <class T>
FEA_NODISCARD constexpr T stars_and_bars_pos(T n, T k) {
	return binomial_coeff(n - 1, k - 1);
}

// Computes stars and bars for non-negative values (>= 0).
// https://en.wikipedia.org/wiki/Stars_and_bars_%28combinatorics%29
template <class T>
FEA_NODISCARD constexpr T stars_and_bars_zero(T n, T k) {
	return binomial_coeff(n + k - 1, k - 1);
}
} // namespace fea
