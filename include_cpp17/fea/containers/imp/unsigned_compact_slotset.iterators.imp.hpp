#pragma once
namespace fea {

template <class MySet>
constexpr ucss_const_iterator<MySet>::ucss_const_iterator(
		bool_const_iterator first, bool_const_iterator last,
		bool_const_iterator ptr, size_type sub_idx) noexcept
		: _first(first)
		, _last(last)
		, _current(ptr)
		, _local_idx(sub_idx) {
}

template <class MySet>
constexpr auto ucss_const_iterator<MySet>::operator*() const noexcept
		-> key_type {
	assert((*_current)[_local_idx]);
	size_type dist = size_type(std::distance(_first, _current)) * bitset_size;
	return key_type(dist + _local_idx);
}

template <class MySet>
constexpr auto ucss_const_iterator<MySet>::operator++() noexcept
		-> ucss_const_iterator<MySet>& {
	assert(_current != _last);
	do {
		++_local_idx;
		if (_local_idx == bitset_size) {
			_local_idx = size_type(0);
			++_current;
		}
	} while (_current != _last && !(*_current)[_local_idx]);
	return *this;
}

template <class MySet>
constexpr auto ucss_const_iterator<MySet>::operator++(int) noexcept
		-> ucss_const_iterator<MySet>& {
	ucss_const_iterator tmp = *this;
	++*this;
	return tmp;
}

template <class MySet>
constexpr auto ucss_const_iterator<MySet>::operator--() noexcept
		-> ucss_const_iterator<MySet>& {
	assert(_current != _first || _local_idx > 0);
	do {
		if (_local_idx == 0) {
			_local_idx = bitset_size - size_type(1);
			--_current;
		} else {
			--_local_idx;
		}
	} while (_current != _first || !(*_current)[_local_idx]);
	return *this;
}

template <class MySet>
constexpr auto ucss_const_iterator<MySet>::operator--(int) noexcept
		-> ucss_const_iterator<MySet>& {
	ucss_const_iterator tmp = *this;
	--*this;
	return tmp;
}

template <class MySet>
auto ucss_const_iterator<MySet>::operator==(
		const ucss_const_iterator& rhs) const noexcept -> bool {
	return _current == rhs._current && _local_idx == rhs._local_idx;
}

template <class MySet>
auto ucss_const_iterator<MySet>::operator!=(
		const ucss_const_iterator& rhs) const noexcept -> bool {
	return !(*this == rhs);
}

template <class MySet>
auto ucss_const_iterator<MySet>::operator<(
		const ucss_const_iterator& rhs) const noexcept -> bool {
	return (_current == rhs._current && _local_idx < rhs._local_idx)
		|| _current < rhs._current;
}

template <class MySet>
auto ucss_const_iterator<MySet>::operator>(
		const ucss_const_iterator& rhs) const noexcept -> bool {
	return rhs < *this;
}

template <class MySet>
auto ucss_const_iterator<MySet>::operator<=(
		const ucss_const_iterator& rhs) const noexcept -> bool {
	return !(rhs < *this);
}

template <class MySet>
auto ucss_const_iterator<MySet>::operator>=(
		const ucss_const_iterator& rhs) const noexcept -> bool {
	return !(*this < rhs);
}

} // namespace fea