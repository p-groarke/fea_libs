namespace fea {

template <class FwdIt, class Func>
[[nodiscard]]
constexpr auto sum(FwdIt first, FwdIt last, Func&& func) {
	using T = std::decay_t<decltype(func(*first))>;
	T ret = T(0);
	for (auto it = first; it != last; ++it) {
		ret += func(*it);
	}
	return ret;
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
	return gains - cost;
}

template <class T>
constexpr T roi(T gains, T cost) {
	return profit(gains, cost) / cost;
}

template <class T>
constexpr T profit_margin(T gains, T cost) {
	return profit(gains, cost) / gains;
}

template <class FwdIt, class Func>
constexpr auto mean(FwdIt begin, FwdIt end, Func func) {
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

template <class FwdIt>
constexpr auto mean(FwdIt begin, FwdIt end) {
	return mean(begin, end, [](const auto& v) -> const auto& { return v; });
}

template <class FwdIt, class Func>
constexpr auto median(FwdIt begin, FwdIt end, Func&& func) {
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

template <class FwdIt, class Func>
constexpr auto variance(FwdIt begin, FwdIt end, Func func) {
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

template <class FwdIt>
constexpr auto variance(FwdIt begin, FwdIt end) {
	return variance(begin, end, [](const auto& v) -> const auto& { return v; });
}

template <class FwdIt, class Func>
constexpr auto std_deviation(FwdIt begin, FwdIt end, Func func) {
	using type_t = std::decay_t<decltype(func(*begin))>;
	return type_t(std::sqrt(double(variance(begin, end, func))));
}

template <class FwdIt>
constexpr auto std_deviation(FwdIt begin, FwdIt end) {
	return std_deviation(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

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

template <class FwdIt, class T, class Func>
constexpr void sigma_filter(FwdIt begin, FwdIt end, T sigma, Func func) {
	return sigma_filter(
			begin, end, sigma, [](const auto& v) -> const auto& { return v; },
			func);
}

template <class FwdIt, class Func>
constexpr auto sample_variance(FwdIt begin, FwdIt end, Func func) {
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

template <class FwdIt>
constexpr auto sample_variance(FwdIt begin, FwdIt end) {
	return sample_variance(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

template <class FwdIt, class Func>
auto sample_std_deviation(FwdIt begin, FwdIt end, Func func) {
	using type_t = std::decay_t<decltype(func(*begin))>;
	return type_t(std::sqrt(double(sample_variance(begin, end, func))));
}

template <class FwdIt>
auto sample_std_deviation(FwdIt begin, FwdIt end) {
	return sample_std_deviation(
			begin, end, [](const auto& v) -> const auto& { return v; });
}

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

template <class FwdIt, class T, class Func>
void sample_sigma_filter(FwdIt begin, FwdIt end, T sigma, Func func) {
	return sample_sigma_filter(
			begin, end, sigma, [](const auto& v) -> const auto& { return v; },
			func);
}

template <class T>
constexpr T factorial(T n) {
	assert(n >= T(0));

	T ret = T(1);
	for (T i = T(1); i <= n; ++i) {
		ret *= i;
	}
	return ret;
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

template <class FwdIt>
fea::iterator_value_t<FwdIt> simple_linear_regression_2d(
		FwdIt first, FwdIt last) {
	auto xmean = fea::mean(
			first, last, [](const auto& pt) { return std::get<0>(pt); });
	auto ymean = fea::mean(
			first, last, [](const auto& pt) { return std::get<1>(pt); });

	auto snum = fea::sum(first, last, [&](const auto& pt) {
		return (std::get<0>(pt) - xmean) * (std::get<1>(pt) - ymean);
	});
	auto sdenom = fea::sum(first, last, [&](const auto& pt) {
		auto v = std::get<0>(pt) - xmean;
		return v * v;
	});

	using T = std::decay_t<decltype(sdenom)>;
	auto b = snum;
	if (sdenom == T(0)) {
		b = T(0);
	} else {
		b /= sdenom;
	}

	auto a = ymean - (b * xmean);
	return { a, b };
}

} // namespace fea