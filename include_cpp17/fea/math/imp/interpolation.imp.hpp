namespace fea {

template <class T>
T percentage(const T& v, const T& a, const T& b) {
	T mag = b - a;
	if (mag == T(0)) {
		return T(0);
	}
	return (v - a) / mag;
}

template <class T>
T lerp(const T& per, const T& a, const T& b) {
	return a + per * (b - a);
}

template <class T>
T lerp(const T& v, const T& from_a, const T& from_b, const T& to_a,
		const T& to_b) {
	T per = fea::percentage(v, from_a, from_b);
	return fea::lerp(per, to_a, to_b);
}

template <class T>
T lerp_linquadlog(const T& per, const T& k, T a /*= T(0)*/, T b /*= T(1)*/) {
	if (k == T(0)) {
		return fea::lerp(per, a, b);
	}
	return a + ((std::exp(k * per) - T(1)) / (std::exp(k) - T(1))) * (b - a);
}
} // namespace fea