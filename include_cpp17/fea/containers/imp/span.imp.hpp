#pragma once
namespace fea {
namespace imp {
template <class T, size_t Extent>
template <class It>
constexpr span<T, Extent>::span(It first, size_t count)
		: _size(count) {
	using cat_t = typename std::iterator_traits<It>::iterator_category;
	static_assert(std::is_same_v<cat_t, std::random_access_iterator_tag>,
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
	static_assert(std::is_same_v<cat_t, std::random_access_iterator_tag>,
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
template <class U>
constexpr span<T, Extent>::span(const span<U, Extent>& other)
		: span(other.data(), other.size()) {
}

template <class T, size_t Extent>
template <class U>
constexpr auto span<T, Extent>::operator=(const span<U, Extent>& other)
		-> span<T, Extent>& {
	_data = other.data();
	_size = other.size();
	return *this;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::begin() const noexcept -> iterator {
	return _data;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::end() const noexcept -> iterator {
	return _data + _size;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::rbegin() const noexcept -> reverse_iterator {
	return reverse_iterator(end());
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::rend() const noexcept -> reverse_iterator {
	return reverse_iterator(begin());
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::front() const noexcept -> const_reference {
	assert(!empty());
	return _data[0];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::front() noexcept -> reference {
	assert(!empty());
	return _data[0];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::back() const noexcept -> const_reference {
	assert(!empty());
	return _data[_size - 1];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::back() noexcept -> reference {
	assert(!empty());
	return _data[_size - 1];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::operator[](size_type i) const noexcept
		-> const_reference {
	assert(i < _size);
	return _data[i];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::operator[](size_type i) noexcept -> reference {
	assert(i < _size);
	return _data[i];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::data() const noexcept -> const_pointer {
	return _data;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::data() noexcept -> pointer {
	return _data;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::size() const noexcept -> size_type {
	return _size;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::size_bytes() const noexcept -> size_type {
	return _size * sizeof(element_type);
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::empty() const noexcept -> bool {
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
[[nodiscard]]
constexpr auto
operator==(span<U> lhs, span<U> rhs) -> bool {
	if (lhs.size() != rhs.size()) {
		return false;
	}

	if (lhs.data() == rhs.data()) {
		return true;
	}

	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class U>
[[nodiscard]]
constexpr auto
operator!=(span<U> lhs, span<U> rhs) -> bool {
	return !(lhs == rhs);
}
} // namespace std
#endif

namespace fea {
namespace imp {
template <class U>
constexpr auto operator==(span<U> lhs, span<U> rhs) -> bool {
	if (lhs.size() != rhs.size()) {
		return false;
	}

	if (lhs.data() == rhs.data()) {
		return true;
	}

	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class U>
constexpr auto operator!=(span<U> lhs, span<U> rhs) -> bool {
	return !(lhs == rhs);
}
} // namespace imp
} // namespace fea
#endif