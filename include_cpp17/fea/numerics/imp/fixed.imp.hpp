namespace fea {
template <class I, size_t S>
constexpr basic_fixed<I, S>::basic_fixed(float f) noexcept
		: value(value_t(f * _float_to_int_v)) {
	// Keep it simple and fast for now.
	// Could do modf for more fractional precision.
}

template <class I, size_t S>
constexpr basic_fixed<I, S>::basic_fixed(double d) noexcept
		: value(value_t(d * _double_to_int_v)) {
}

template <bool B, class TrueT, class FalseT>
constexpr auto mcexpr_if(TrueT lhs, FalseT rhs) {
	if constexpr (B) {
		return lhs;
	} else {
		return rhs;
	}
}

template <class I, size_t S>
constexpr basic_fixed<I, S>::basic_fixed(value_t v) noexcept {
	if constexpr (scaling_is_pow2_v) {
		value = v << scaling_sqrt_v;
	} else {
		value = v * scaling_v;
	}
}

template <class I, size_t S>
constexpr basic_fixed<I, S>::operator float() const noexcept {
	return float(value) * _int_to_float_v;
}

template <class I, size_t S>
constexpr basic_fixed<I, S>::operator double() const noexcept {
	return double(value) * _int_to_double_v;
}

template <class I, size_t S>
constexpr basic_fixed<I, S>::operator value_t() const noexcept {
	if constexpr (scaling_is_pow2_v) {
		return value >> scaling_sqrt_v;
	} else {
		return value / scaling_v;
	}
}
} // namespace fea

namespace std {
template <class T, size_t S>
class numeric_limits<fea::basic_fixed<T, S>> {
	using value_t = fea::basic_fixed<T, S>;

public:
	// Member constants
	static constexpr bool is_specialized = true;
	static constexpr bool is_signed = std::is_signed_v<T>;
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
	static constexpr bool is_modulo = std::numeric_limits<T>::is_modulo;

	static constexpr int digits = std::numeric_limits<T>::digits;
	static constexpr int digits10 = std::numeric_limits<T>::digits10;
	static constexpr int max_digits10 = std::numeric_limits<T>::max_digits10;
	static constexpr int radix = 2;

	static constexpr int min_exponent = 0;
	static constexpr int min_exponent10 = 0;
	static constexpr int max_exponent = 0;
	static constexpr int max_exponent10 = 0;
	static constexpr bool traps = std::numeric_limits<T>::traps;
	static constexpr bool tinyness_before = false;

	// Member functions
	static constexpr value_t min() noexcept {
		// Even though it isn't necessary, since we can perfectly represent
		// zero. Return value closest to zero (0), to allow interchange with
		// floating types.
		value_t ret;
		ret.value = T(0);
		// ret.value = (std::numeric_limits<T>::min)();
		return ret;
	}

	static constexpr value_t lowest() noexcept {
		value_t ret;
		ret.value = std::numeric_limits<T>::lowest();
		return ret;
	}

	static constexpr value_t max() noexcept {
		value_t ret;
		ret.value = (std::numeric_limits<T>::max)();
		return ret;
	}

	static constexpr value_t epsilon() noexcept {
		value_t ret;
		ret.value = T(1);
		return ret;
	}

	static constexpr value_t round_error() noexcept {
		return value_t(0.5);
	}

	static constexpr value_t infinity() noexcept {
		return value_t(T(0));
	}
	static constexpr value_t quiet_NaN() noexcept {
		return value_t(T(0));
	}
	static constexpr value_t signaling_NaN() noexcept {
		return value_t(T(0));
	}
	static constexpr value_t denorm_min() noexcept {
		return value_t(T(0));
	}
};
} // namespace std
