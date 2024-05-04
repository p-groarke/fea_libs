#pragma once
namespace fea {

template <class MySet>
constexpr uss_const_iterator<MySet>::uss_const_iterator(
		bool_const_iterator first, bool_const_iterator last,
		bool_const_iterator ptr) noexcept
		: _first(first)
		, _last(last)
		, _current(ptr) {
}

template <class MySet>
constexpr auto uss_const_iterator<MySet>::operator*() const noexcept
		-> key_type {
	assert(*_current);
	return key_type(std::distance(_first, _current));
}

template <class MySet>
constexpr auto uss_const_iterator<MySet>::operator++() noexcept
		-> uss_const_iterator<MySet>& {
	assert(_current != _last);
	do {
		++_current;
	} while (_current != _last && !*_current);
	return *this;
}

template <class MySet>
constexpr auto uss_const_iterator<MySet>::operator++(int) noexcept
		-> uss_const_iterator<MySet>& {
	uss_const_iterator tmp = *this;
	++*this;
	return tmp;
}

template <class MySet>
constexpr auto uss_const_iterator<MySet>::operator--() noexcept
		-> uss_const_iterator<MySet>& {
	assert(_current != _first);
	do {
		--_current;
	} while (!*_current && _current != _first);
	return *this;
}

template <class MySet>
constexpr auto uss_const_iterator<MySet>::operator--(int) noexcept
		-> uss_const_iterator<MySet>& {
	uss_const_iterator tmp = *this;
	--*this;
	return tmp;
}

template <class MySet>
auto uss_const_iterator<MySet>::operator==(
		const uss_const_iterator& rhs) const noexcept -> bool {
	return _current == rhs._current;
}

template <class MySet>
auto uss_const_iterator<MySet>::operator!=(
		const uss_const_iterator& rhs) const noexcept -> bool {
	return !(*this == rhs);
}

template <class MySet>
auto uss_const_iterator<MySet>::operator<(
		const uss_const_iterator& rhs) const noexcept -> bool {
	return _current < rhs._current;
}

template <class MySet>
auto uss_const_iterator<MySet>::operator>(
		const uss_const_iterator& rhs) const noexcept -> bool {
	return rhs < *this;
}

template <class MySet>
auto uss_const_iterator<MySet>::operator<=(
		const uss_const_iterator& rhs) const noexcept -> bool {
	return !(rhs < *this);
}

template <class MySet>
auto uss_const_iterator<MySet>::operator>=(
		const uss_const_iterator& rhs) const noexcept -> bool {
	return !(*this < rhs);
}

template <class MySet>
constexpr uss_iterator<MySet>::uss_iterator(base_t it) noexcept
		: base_t(it) {
}

template <class MySet>
constexpr uss_iterator<MySet>::uss_iterator(
		bool_iterator first, bool_iterator last, bool_iterator current) noexcept
		: base_t(first, last, current) {
}

} // namespace fea