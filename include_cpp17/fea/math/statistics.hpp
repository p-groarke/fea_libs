/*
BSD 3-Clause License

Copyright (c) 2024, Philippe Groarke
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

// Compute population standard deviation.
// Predicate function must return the values to compute.
template <class FwdIt, class Func>
[[nodiscard]]
constexpr auto std_deviation(FwdIt begin, FwdIt end, Func func);

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


// Compute sample variance of values (Bessel's correction, divided by n
// - 1). Predicate function must return value to compute.
template <class FwdIt, class Func>
[[nodiscard]]
constexpr auto sample_variance(FwdIt begin, FwdIt end, Func func);

// Compute sample variance of values (Bessel's correction, divided by n - 1).
template <class FwdIt>
[[nodiscard]]
constexpr auto sample_variance(FwdIt begin, FwdIt end);

// Compute sample standard deviation (Bessel's correction, divides by n - 1).
// Predicate function must return the values to compute.
template <class FwdIt, class Func>
[[nodiscard]]
auto sample_std_deviation(FwdIt begin, FwdIt end, Func func);

// Compute sample standard deviation (Bessel's correction, divides by n - 1).
template <class FwdIt>
[[nodiscard]]
auto sample_std_deviation(FwdIt begin, FwdIt end);

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

// Computes simple linear regression.
// Given 2 dimensional values, outputs a and b respectively.
// Iterator values should support std::get<0/1> (std::pairs or std::tuples).
template <class FwdIt>
[[nodiscard]]
fea::iterator_value_t<FwdIt> simple_linear_regression(FwdIt first, FwdIt last);

} // namespace fea

#include "imp/statistics.imp.hpp"
