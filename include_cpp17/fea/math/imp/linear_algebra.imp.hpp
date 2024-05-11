namespace fea {

template <class T>
T magnitude(const T& x, const T& y) {
	return std::sqrt(x * x + y * y);
}

template <class T>
void normalize(T& x, T& y) {
	T mag = magnitude(x, y);
	x /= mag;
	y /= mag;
}

template <class T>
constexpr T divide_round(const T& dividend, const T& divisor) {
	return (dividend + (divisor / T(2))) / divisor;
}
} // namespace fea