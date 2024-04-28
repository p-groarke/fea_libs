#pragma once
namespace fea {
namespace experimental {

template <class MySet>
constexpr unsigned_set_const_iterator<MySet>::unsigned_set_const_iterator(
		bool_pointer first, bool_pointer last, bool_pointer ptr) noexcept
		: _first(first)
		, _last(last)
		, _ptr(ptr) {
}

template <class MySet>
constexpr auto unsigned_set_const_iterator<MySet>::operator*() const noexcept
		-> key_type {
	assert(*_ptr);
	return key_type(std::distance(_first, _ptr));
}

template <class MySet>
constexpr auto unsigned_set_const_iterator<MySet>::operator++() noexcept
		-> unsigned_set_const_iterator<MySet>& {
	assert(_ptr != _last);
	do {
		++_ptr;
	} while (!*_ptr && _ptr != _last);
	return *this;
}

template <class MySet>
constexpr auto unsigned_set_const_iterator<MySet>::operator++(int) noexcept
		-> unsigned_set_const_iterator<MySet>& {
	unsigned_set_const_iterator tmp = *this;
	++*this;
	return tmp;
}

template <class MySet>
constexpr auto unsigned_set_const_iterator<MySet>::operator--() noexcept
		-> unsigned_set_const_iterator<MySet>& {
	assert(_ptr != _first);
	do {
		--_ptr;
	} while (!*_ptr && _ptr != _first);
	return *this;
}

template <class MySet>
constexpr auto unsigned_set_const_iterator<MySet>::operator--(int) noexcept
		-> unsigned_set_const_iterator<MySet>& {
	unsigned_set_const_iterator tmp = *this;
	--*this;
	return tmp;
}

template <class MySet>
auto unsigned_set_const_iterator<MySet>::operator==(
		const unsigned_set_const_iterator& rhs) const noexcept -> bool {
	return _ptr == rhs._ptr;
}

template <class MySet>
auto unsigned_set_const_iterator<MySet>::operator!=(
		const unsigned_set_const_iterator& rhs) const noexcept -> bool {
	return !(*this == rhs);
}

template <class MySet>
auto unsigned_set_const_iterator<MySet>::operator<(
		const unsigned_set_const_iterator& rhs) const noexcept -> bool {
	return _ptr < rhs._ptr;
}

template <class MySet>
auto unsigned_set_const_iterator<MySet>::operator>(
		const unsigned_set_const_iterator& rhs) const noexcept -> bool {
	return rhs < *this;
}

template <class MySet>
auto unsigned_set_const_iterator<MySet>::operator<=(
		const unsigned_set_const_iterator& rhs) const noexcept -> bool {
	return !(rhs < *this);
}

template <class MySet>
auto unsigned_set_const_iterator<MySet>::operator>=(
		const unsigned_set_const_iterator& rhs) const noexcept -> bool {
	return !(*this < rhs);
}

template <class MySet>
constexpr unsigned_set_iterator<MySet>::unsigned_set_iterator(
		bool_pointer first, bool_pointer last, bool_pointer ptr) noexcept
		: base_t(first, last, ptr) {
}

} // namespace experimental
} // namespace fea