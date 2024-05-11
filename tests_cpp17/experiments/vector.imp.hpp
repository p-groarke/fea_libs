namespace fea {
namespace detail {
template <class T>
using has_tuple_size = decltype(std::tuple_size_v<decltype(std::declval<T>())>);

template <class T>
using has_tuple_element = std::tuple_element_t<0, decltype(std::declval<T>())>;

template <class T>
using has_get = decltype(get<0>(std::declval<T>()));

template <class T>
constexpr bool fulfills_math_vector() {
	using std::get;
	return fea::is_detected_v<has_tuple_size, T>
		&& fea::is_detected_v<has_tuple_element, T>
		&& fea::is_detected_v<has_get, T>;
}
} // namespace detail
} // namespace fea

template <class T>
constexpr std::enable_if_t<fea::is_math_vector_v<T>, T> operator+(
		const T& lhs, const T& rhs) {
	static_assert(fea::detail::fulfills_math_vector<T>(),
			"fea::vector : Type doesn't fulfill math vector requirements.");

	using std::get;
	T ret{};
	constexpr size_t dimensions = std::tuple_size_v<T>;
	fea::static_for<dimensions>([&](auto cexpr_i) {
		constexpr size_t i = cexpr_i;
		get<i>(ret) = get<i>(lhs) + get<i>(rhs);
	});
	return ret;
}