#pragma once
namespace fea {
template <class T, size_t StackSize>
template <size_t InSize>
constexpr stack_vector<T, StackSize>::stack_vector(
		const std::array<T, InSize>& arr)
		: _size(InSize) {
	static_assert(InSize <= StackSize,
			"stack_vector : initializing with too many values");
	std::copy(arr.begin(), arr.end(), _data.begin());
}

template <class T, size_t StackSize>
template <size_t InSize>
constexpr stack_vector<T, StackSize>::stack_vector(std::array<T, InSize>&& arr)
		: _size(InSize) {
	static_assert(InSize <= StackSize,
			"stack_vector : initializing with too many values");
	std::move(arr.begin(), arr.end(), _data.begin());
}

template <class T, size_t StackSize>
constexpr stack_vector<T, StackSize>::stack_vector(
		size_type count, const_reference value)
		: _size(count) {
	assert(_size <= StackSize);

	std::fill_n(_data.begin(), _size, value);
}

template <class T, size_t StackSize>
constexpr stack_vector<T, StackSize>::stack_vector(size_type count)
		: stack_vector(count, value_type{}) {
}

template <class T, size_t StackSize>
constexpr stack_vector<T, StackSize>::stack_vector(
		std::initializer_list<value_type>&& init)
		: _size(init.size()) {
	assert(init.size() <= StackSize);
	fea::copy_or_move(init.begin(), init.end(), _data.begin());
}

template <class T, size_t StackSize>
template <class InputIt, class>
constexpr stack_vector<T, StackSize>::stack_vector(InputIt start, InputIt stop)
		: _size(std::distance(start, stop)) {
	assert(size_t(std::distance(start, stop)) <= StackSize);
	fea::copy_or_move(start, stop, _data.begin());
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_reference
stack_vector<T, StackSize>::at(size_type i) const {
	if (i >= _size) {
		fea::maybe_throw(
				__FUNCTION__, __LINE__, "accessing out-of-range element");
	}
	return _data.at(i);
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::reference
stack_vector<T, StackSize>::at(size_type i) {
	if (i >= _size) {
		fea::maybe_throw(
				__FUNCTION__, __LINE__, "accessing out-of-range element");
	}
	return _data.at(i);
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_reference
stack_vector<T, StackSize>::operator[](size_type i) const noexcept {
	assert(i < _size);
	return _data[i];
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::reference
stack_vector<T, StackSize>::operator[](size_type i) noexcept {
	assert(i < _size);
	return _data[i];
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_reference
stack_vector<T, StackSize>::front() const noexcept {
	assert(_size > 0);
	return _data.front();
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::reference
stack_vector<T, StackSize>::front() noexcept {
	assert(_size > 0);
	return _data.front();
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_reference
stack_vector<T, StackSize>::back() const noexcept {
	assert(_size > 0);
	return _data[_size - 1];
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::reference
stack_vector<T, StackSize>::back() noexcept {
	assert(_size > 0);
	return _data[_size - 1];
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_pointer
stack_vector<T, StackSize>::data() const noexcept {
	return _data.data();
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::pointer
stack_vector<T, StackSize>::data() noexcept {
	return _data.data();
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_iterator
stack_vector<T, StackSize>::begin() const noexcept {
	return _data.begin();
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::iterator
stack_vector<T, StackSize>::begin() noexcept {
	return _data.begin();
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_iterator
stack_vector<T, StackSize>::end() const noexcept {
	return _data.begin() + _size;
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::iterator
stack_vector<T, StackSize>::end() noexcept {
	return _data.begin() + _size;
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_reverse_iterator
stack_vector<T, StackSize>::rbegin() const noexcept {
	return std::make_reverse_iterator(end());
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::reverse_iterator
stack_vector<T, StackSize>::rbegin() noexcept {
	return std::make_reverse_iterator(end());
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_reverse_iterator
stack_vector<T, StackSize>::rend() const noexcept {
	return std::make_reverse_iterator(begin());
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::reverse_iterator
stack_vector<T, StackSize>::rend() noexcept {
	return std::make_reverse_iterator(begin());
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_iterator
stack_vector<T, StackSize>::cbegin() const noexcept {
	return begin();
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_iterator
stack_vector<T, StackSize>::cend() const noexcept {
	return end();
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_reverse_iterator
stack_vector<T, StackSize>::crbegin() const noexcept {
	return rbegin();
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::const_reverse_iterator
stack_vector<T, StackSize>::crend() const noexcept {
	return rend();
}

template <class T, size_t StackSize>
constexpr bool stack_vector<T, StackSize>::empty() const noexcept {
	return _size == 0;
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::size_type
stack_vector<T, StackSize>::size() const noexcept {
	return _size;
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::size_type
stack_vector<T, StackSize>::max_size() const noexcept {
	return StackSize;
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::reserve(size_type) noexcept {
	// Provided to match std::vector api.
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::size_type
stack_vector<T, StackSize>::capacity() const noexcept {
	return StackSize;
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::shrink_to_fit() noexcept {
	// Provided to match std::vector api.
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::clear() {
	fea::destroy(begin(), end());
	_size = 0;
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::iterator
stack_vector<T, StackSize>::erase(const_iterator pos) {
	size_type dist = size_type(std::distance(cbegin(), pos));
	assert(dist < size());
	auto it = begin() + dist;
	fea::copy_or_move(it + 1, end(), it);
	--_size;
	fea::destroy_at(_data.data() + _size);
	return it;
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::iterator
stack_vector<T, StackSize>::erase(const_iterator first, const_iterator last) {
	if (first == last) {
		return begin() + std::distance(cbegin(), last);
	}

	size_type begin_idx = size_type(std::distance(cbegin(), first));
	size_type end_idx = size_type(std::distance(cbegin(), last));
	assert(begin_idx <= size());
	assert(end_idx <= size());

	auto beg_it = begin() + begin_idx;
	auto end_it = begin() + end_idx;
	fea::copy_or_move(end_it, end(), beg_it);
	size_type range_size = size_type(std::distance(first, last));
	_size -= range_size;
	fea::destroy(end(), end() + range_size);
	return beg_it;
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::iterator
stack_vector<T, StackSize>::insert(const_iterator pos, const_reference value) {
	assert(_size < _data.size());
	size_type dist = size_type(std::distance(cbegin(), pos));
	auto start_it = begin() + dist;
	fea::copy_or_move_backward(start_it, end(), end() + 1);
	*start_it = value;
	++_size;
	return start_it;
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::iterator
stack_vector<T, StackSize>::insert(const_iterator pos, value_type&& value) {
	assert(_size < _data.size());
	size_type dist = size_type(std::distance(cbegin(), pos));
	auto start_it = begin() + dist;
	fea::copy_or_move_backward(start_it, end(), end() + 1);
	*start_it = std::move(value);
	++_size;
	return start_it;
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::iterator
stack_vector<T, StackSize>::insert(
		const_iterator pos, size_type count, const_reference value) {
	assert(_size <= _data.size() - count);
	size_type dist = size_type(std::distance(cbegin(), pos));
	if (count == 0) {
		return begin() + dist;
	}

	auto start_it = begin() + dist;
	fea::copy_or_move_backward(start_it, end(), end() + count);
	std::fill_n(start_it, count, value);
	_size += count;
	return start_it;
}

template <class T, size_t StackSize>
template <class InputIt, class>
constexpr typename stack_vector<T, StackSize>::iterator
stack_vector<T, StackSize>::insert(
		const_iterator pos, InputIt first, InputIt last) {
	size_type count = size_type(std::distance(first, last));
	size_type dist = size_type(std::distance(cbegin(), pos));
	if (count == 0) {
		return begin() + dist;
	}

	assert(_size <= _data.size() - count);
	auto start_it = begin() + dist;
	fea::copy_or_move_backward(start_it, end(), end() + count);
	fea::copy_or_move(first, last, start_it);
	_size += count;
	return start_it;
}

template <class T, size_t StackSize>
constexpr typename stack_vector<T, StackSize>::iterator
stack_vector<T, StackSize>::insert(
		const_iterator pos, std::initializer_list<value_type> ilist) {
	return insert(pos, ilist.begin(), ilist.end());
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::push_back(const T& value) {
	assert(_size < StackSize);
	_data[_size++] = value;
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::push_back(T&& value) {
	assert(_size < StackSize);
	_data[_size++] = std::move(value);
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::pop_back() {
	assert(_size > 0);
	--_size;
	fea::destroy_at(_data.data() + _size);
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::resize(size_type new_size) {
	resize(new_size, T{});
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::resize(
		size_type new_size, const_reference value) {
	assert(new_size <= StackSize);

	if (new_size > _size) {
		std::fill(end(), begin() + new_size, value);
	} else {
		fea::destroy(begin() + new_size, begin() + _size);
	}

	_size = new_size;
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::swap(stack_vector& other) {
	using std::swap;
	swap(_data, other._data);
	swap(_size, other._size);
}


template <class K, size_t S>
constexpr bool operator==(
		const stack_vector<K, S>& lhs, const stack_vector<K, S>& rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}

	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class K, size_t S>
constexpr bool operator!=(
		const stack_vector<K, S>& lhs, const stack_vector<K, S>& rhs) {
	return !(rhs == lhs);
}
} // namespace fea
