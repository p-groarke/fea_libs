#pragma once
namespace fea {
namespace imp {
template <class T, size_t Extent>
template <class It>
constexpr span<T, Extent>::span(It first, size_t count)
		: _size(count) {
	using cat_t = typename std::iterator_traits<It>::iterator_category;
	static_assert(std::is_same<cat_t, std::random_access_iterator_tag>::value,
			"fea::span : iterators must be random access");

	if (_size != 0) {
		_data = &(*first);
	}
}

template <class T, size_t Extent>
template <class It>
constexpr span<T, Extent>::span(It first, It last)
		: _size(size_t(last - first)) {
	using cat_t = typename std::iterator_traits<It>::iterator_category;
	static_assert(std::is_same<cat_t, std::random_access_iterator_tag>::value,
			"fea::span : iterators must be random access");

	if (_size != 0) {
		_data = &(*first);
	}
}

template <class T, size_t Extent>
template <template <class, class...> class Container, class... Args>
constexpr span<T, Extent>::span(
		const Container<std::decay_t<T>, Args...>& container)
		: _data(container.data())
		, _size(container.size()) {
}

template <class T, size_t Extent>
constexpr span<T, Extent>::span(
		const span<std::remove_const_t<T>, Extent>& other)
		: _data(other._data)
		, _size(other._size) {
}

template <class T, size_t Extent>
constexpr span<T, Extent>& span<T, Extent>::operator=(
		const span<std::remove_const_t<T>, Extent>& other) {
	if (this == &other) {
		return *this;
	}
	_data = other._data;
	_size = other._size;
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::iterator
span<T, Extent>::begin() const noexcept {
	return _data;
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::iterator
span<T, Extent>::end() const noexcept {
	return _data + _size;
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::reverse_iterator
span<T, Extent>::rbegin() const noexcept {
	return reverse_iterator(end());
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::reverse_iterator
span<T, Extent>::rend() const noexcept {
	return reverse_iterator(begin());
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::const_reference
span<T, Extent>::front() const {
	assert(!empty());
	return _data[0];
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::reference span<T, Extent>::front() {
	assert(!empty());
	return _data[0];
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::const_reference
span<T, Extent>::back() const {
	assert(!empty());
	return _data[_size - 1];
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::reference span<T, Extent>::back() {
	assert(!empty());
	return _data[_size - 1];
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::const_reference span<T, Extent>::operator[](
		size_type i) const {
	assert(i < _size);
	return _data[i];
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::reference span<T, Extent>::operator[](
		size_type i) {
	assert(i < _size);
	return _data[i];
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::const_pointer
span<T, Extent>::data() const noexcept {
	return _data;
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::pointer span<T, Extent>::data() noexcept {
	return _data;
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::size_type
span<T, Extent>::size() const noexcept {
	return _size;
}

template <class T, size_t Extent>
constexpr typename span<T, Extent>::size_type
span<T, Extent>::size_bytes() const noexcept {
	return _size * sizeof(element_type);
}

template <class T, size_t Extent>
constexpr bool span<T, Extent>::empty() const noexcept {
	return _size == 0;
}

} // namespace imp
} // namespace fea


// Defining the operator== could cause issues.
// You may disable these by defining FEA_DISABLE_SPAN_EQ.
#if !defined(FEA_DISABLE_SPAN_EQ_DEF)
#if FEA_CPP20
namespace std {
template <class U>
[[nodiscard]] constexpr bool operator==(span<U> lhs, span<U> rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}

	if (lhs.data() == rhs.data()) {
		return true;
	}

	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class U>
[[nodiscard]] constexpr bool operator!=(span<U> lhs, span<U> rhs) {
	return !(lhs == rhs);
}
} // namespace std
#endif

namespace fea {
namespace imp {
template <class U>
constexpr bool operator==(span<U> lhs, span<U> rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}

	if (lhs.data() == rhs.data()) {
		return true;
	}

	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class U>
constexpr bool operator!=(span<U> lhs, span<U> rhs) {
	return !(lhs == rhs);
}
} // namespace imp
} // namespace fea
#endif