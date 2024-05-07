namespace fea {
template <class Key, class Alloc>
template <class FwdIt>
unsigned_slotset<Key, Alloc>::unsigned_slotset(FwdIt first, FwdIt last) {
	insert(first, last);
}

template <class Key, class Alloc>
unsigned_slotset<Key, Alloc>::unsigned_slotset(
		std::initializer_list<key_type>&& ilist) {
	unsigned_slotset(ilist.begin(), ilist.end());
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::begin() const noexcept -> const_iterator {
	auto ret
			= const_iterator{ _lookup.begin(), _lookup.end(), _lookup.begin() };
	if (!_lookup.empty() && !_lookup.front()) {
		++ret;
	}
	return ret;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::cbegin() const noexcept -> const_iterator {
	return begin();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::end() const noexcept -> const_iterator {
	return const_iterator{ _lookup.begin(), _lookup.end(), _lookup.end() };
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::cend() const noexcept -> const_iterator {
	return end();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::empty() const noexcept -> bool {
	assert(std::none_of(_lookup.begin(), _lookup.end(),
			[this](bool_type v) { return _size == 0 && v; }));
	return _size == size_type(0);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::size() const noexcept -> size_type {
	assert(size_type(std::count(_lookup.begin(), _lookup.end(), uint8_t(true)))
			== _size);
	return _size;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::max_size() const noexcept -> size_type {
	return _lookup.max_size();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::reserve(key_type key) -> void {
	size_type new_cap = size_type(key) + size_type(1);
	_lookup.reserve(new_cap);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::capacity() const noexcept -> size_type {
	return _lookup.capacity();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::shrink_to_fit() -> void {
	_lookup.shrink_to_fit();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::clear() noexcept -> void {
	_lookup.clear();
	_size = 0;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::insert(key_type key)
		-> std::pair<const_iterator, bool> {
	const_iterator it = find(key);
	if (it != end()) {
		return { it, false };
	}

	size_type idx = size_type(key);
	if (idx >= _lookup.size()) {
		_lookup.resize(idx + size_type(1));
	}
	_lookup[idx] = uint8_t(true);
	++_size;

	return {
		const_iterator{ _lookup.begin(), _lookup.end(), _lookup.begin() + idx },
		true,
	};
}

template <class Key, class Alloc>
template <class FwdIt>
auto unsigned_slotset<Key, Alloc>::insert(FwdIt first, FwdIt last) -> void {
	using value_t = typename std::iterator_traits<FwdIt>::value_type;
	static_assert(std::is_same_v<value_t, key_type>,
			"unsigned_set : Invalid iterators, do not point to key_type.");
	if (std::distance(first, last) == 0) {
		return;
	}

	// Allocate once. Find max key to do so.
	FwdIt max_it = std::max_element(
			first, last, [](key_type lhs, key_type rhs) { return lhs < rhs; });

	size_type max_key_idx = size_type(*max_it);
	if (max_key_idx >= _lookup.size()) {
		_lookup.resize(max_key_idx + size_type(1));
	}

	// Fill the set.
	for (FwdIt it = first; it != last; ++it) {
		size_type idx = size_type(*it);
		if (!_lookup[idx]) {
			_lookup[idx] = uint8_t(true);
			++_size;
		}
	}
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::insert(
		std::initializer_list<key_type>&& ilist) -> void {
	insert(ilist.begin(), ilist.end());
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::erase(key_type key) noexcept -> size_type {
	size_type idx = size_type(key);
	if (idx >= _lookup.size() || !_lookup[idx]) {
		return size_type(0);
	}
	_lookup[idx] = uint8_t(false);
	--_size;
	return size_type(1);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::erase(const_iterator cit) noexcept
		-> const_iterator {
	if (cit != end() && *cit._current) {
		*const_cast<bool_type*>(std::addressof(*cit._current)) = uint8_t(false);
		--_size;
		++cit;
	}
	return cit;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::erase(const_iterator cfirst,
		const_iterator clast) noexcept -> const_iterator {
	if (cfirst == clast) {
		return clast;
	}

	auto it = cfirst;
	for (; it != clast; ++it) {
		erase(*it);
	}
	if (it != cend()) {
		++it;
	}
	return it;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::swap(unsigned_slotset& other) noexcept
		-> void {
	_lookup.swap(other._lookup);
	std::swap(_size, other._size);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::merge(unsigned_slotset& source) -> void {
	if (source._lookup.size() > _lookup.size()) {
		_lookup.resize(source._lookup.size());
	}

	assert(_lookup.size() >= source._lookup.size());
	size_type size = (std::min)(_lookup.size(), source._lookup.size());
	for (size_type i = 0; i < size; ++i) {
		if (_lookup[i]) {
			continue;
		}
		if (source._lookup[i]) {
			source._lookup[i] = uint8_t(false);
			--source._size;
			_lookup[i] = uint8_t(true);
			++_size;
		}
	}
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::merge(unsigned_slotset&& source) -> void {
	merge(source);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::count(key_type key) const noexcept
		-> size_type {
	return size_type(contains(key));
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::contains(key_type key) const noexcept
		-> bool {
	size_type idx = size_type(key);
	if (idx >= _lookup.size()) {
		return false;
	}
	return bool(_lookup[idx]);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::find(key_type key) const noexcept
		-> const_iterator {
	size_type idx = size_type(key);
	if (idx >= _lookup.size() || !_lookup[idx]) {
		return end();
	}
	return const_iterator{ _lookup.begin(), _lookup.end(),
		_lookup.begin() + idx };
}

} // namespace fea