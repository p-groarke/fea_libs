namespace fea {
namespace detail {

template <size_t S, class I>
constexpr fixed<S, I>::fixed() noexcept
		: value(value_t{}) {
}

template <size_t S, class I>
constexpr fixed<S, I>::fixed(float f) noexcept
		: value(value_t(f * _float_to_int_v)) {
	// Keep it simple and fast for now.
	// Could do modf for more fractional precision.
}

template <size_t S, class I>
constexpr fixed<S, I>::fixed(double d) noexcept
		: value(value_t(d * _double_to_int_v)) {
}

template <size_t S, class I>
constexpr fixed<S, I>::fixed(value_t v) noexcept {
	if constexpr (scaling_is_pow2_v) {
		value = v << scaling_sqrt_v;
	} else {
		value = v * scaling_v;
	}
}

template <size_t S, class I>
constexpr fixed<S, I>::operator float() const noexcept {
	return float(value) * _int_to_float_v;
}

template <size_t S, class I>
constexpr fixed<S, I>::operator double() const noexcept {
	return double(value) * _int_to_double_v;
}

template <size_t S, class I>
constexpr fixed<S, I>::operator value_t() const noexcept {
	if constexpr (scaling_is_pow2_v) {
		return value >> scaling_sqrt_v;
	} else {
		return value / scaling_v;
	}
}
} // namespace detail
} // namespace fea

namespace std {
template <size_t Scaling, class IntT>
class numeric_limits<fea::detail::fixed<Scaling, IntT>> {
	using value_t = fea::detail::fixed<Scaling, IntT>;

public:
	// Member constants
	static constexpr bool is_specialized = true;
	static constexpr bool is_signed = std::is_signed_v<IntT>;
	static constexpr bool is_integer = false;
	static constexpr bool is_exact = true;
	static constexpr bool has_infinity = false;
	static constexpr bool has_quiet_NaN = false;
	static constexpr bool has_signaling_NaN = false;
	static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
	static constexpr bool has_denorm_loss = false;
	static constexpr std::float_round_style round_style
			= std::round_toward_zero;
	static constexpr bool is_iec559 = false;
	static constexpr bool is_bounded = true;
	static constexpr bool is_modulo = std::numeric_limits<IntT>::is_modulo;

	// static constexpr int digits = ?; /*todo*/
	// static constexpr int digits10 = false; /*todo*/
	// static constexpr int max_digits10 = false; /*todo*/
	// static constexpr int radix = false; /*todo*/

	static constexpr int min_exponent = 0;
	static constexpr int min_exponent10 = 0;
	static constexpr int max_exponent = 0;
	static constexpr int max_exponent10 = 0;
	static constexpr bool traps = std::numeric_limits<IntT>::traps;
	static constexpr bool tinyness_before = false;

	// Member functions
	static constexpr value_t min() noexcept {
		value_t ret;
		// ret.value = IntT(0); ??
		ret.value = (std::numeric_limits<IntT>::min)();
		return ret;
	}

	static constexpr value_t lowest() noexcept {
		value_t ret;
		ret.value = std::numeric_limits<IntT>::lowest();
		return ret;
	}

	static constexpr value_t max() noexcept {
		value_t ret;
		ret.value = (std::numeric_limits<IntT>::max)();
		return ret;
	}

	static constexpr value_t epsilon() noexcept {
		value_t ret;
		ret.value = IntT(1);
		return ret;
	}

	static constexpr value_t round_error() noexcept {
		return value_t(0.5);
	}

	static constexpr value_t infinity() noexcept {
		return value_t(IntT(0));
	}
	static constexpr value_t quiet_NaN() noexcept {
		return value_t(IntT(0));
	}
	static constexpr value_t signaling_NaN() noexcept {
		return value_t(IntT(0));
	}
	static constexpr value_t denorm_min() noexcept {
		return value_t(IntT(0));
	}
};
} // namespace std
