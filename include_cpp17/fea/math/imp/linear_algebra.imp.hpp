namespace fea {

template <class T>
T magnitude(const T& x, const T& y) {
	return std::sqrt(x * x + y * y);
}

template <class T>
void normalize(T& x, T& y) {
	if (x == T(0) && y == T(0)) {
		return;
	}

	T mag = magnitude(x, y);
	x /= mag;
	y /= mag;
}

template <class T>
constexpr T divide_round(const T& dividend, const T& divisor) {
	static_assert(std::is_integral_v<T>,
			"fea::divide_round : Useless for non integral types.");
	if (divisor == T(0)) {
		return T(0);
	}
	return (dividend + (divisor / T(2))) / divisor;
}
} // namespace fea