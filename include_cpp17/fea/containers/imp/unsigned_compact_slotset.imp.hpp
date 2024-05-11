namespace fea {
template <class Key, class Alloc>
template <class FwdIt>
unsigned_compact_slotset<Key, Alloc>::unsigned_compact_slotset(
		FwdIt first, FwdIt last) {
	insert(first, last);
}

template <class Key, class Alloc>
unsigned_compact_slotset<Key, Alloc>::unsigned_compact_slotset(
		std::initializer_list<key_type>&& ilist) {
	unsigned_compact_slotset(ilist.begin(), ilist.end());
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::begin() const noexcept
		-> const_iterator {
	auto ret = const_iterator{ _lookup.begin(), _lookup.end(), _lookup.begin(),
		size_type(0) };
	if (!_lookup.empty() && !_lookup.front()[0]) {
		++ret;
	}
	return ret;
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::cbegin() const noexcept
		-> const_iterator {
	return begin();
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::end() const noexcept
		-> const_iterator {
	return const_iterator{ _lookup.begin(), _lookup.end(), _lookup.end(),
		size_type(0) };
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::cend() const noexcept
		-> const_iterator {
	return end();
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::empty() const noexcept -> bool {
	assert(std::none_of(_lookup.begin(), _lookup.end(),
			[this](bool_type v) { return _size == 0 && v.any(); }));
	return _size == 0;
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::size() const noexcept -> size_type {
	assert(std::accumulate(_lookup.begin(), _lookup.end(), size_type(0),
				   [](size_t sum, bool_type v) { return sum + v.count(); })
			== _size);

	return _size;
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::max_size() const noexcept
		-> size_type {
	return _lookup.max_size();
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::reserve(key_type key) -> void {
	size_type new_cap = (size_type(key) + size_type(bitset_size)) / bitset_size;
	_lookup.reserve(new_cap);
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::capacity() const noexcept
		-> size_type {
	return _lookup.capacity() * bitset_size;
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::shrink_to_fit() -> void {
	_lookup.shrink_to_fit();
}

template <class Key, class Alloc>
constexpr auto unsigned_compact_slotset<Key, Alloc>::clear() noexcept -> void {
	_lookup.clear();
	_size = size_type(0);
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::insert(key_type key)
		-> std::pair<const_iterator, bool> {
	const_iterator it = find(key);
	if (it != end()) {
		return { it, false };
	}

	size_type lkp_idx = maybe_resize(key);
	size_type lcl_idx = local_idx(key);
	_lookup[lkp_idx][lcl_idx] = true;
	++_size;

	return {
		const_iterator{ _lookup.begin(), _lookup.end(),
				_lookup.begin() + lkp_idx, lcl_idx },
		true,
	};
}

template <class Key, class Alloc>
template <class FwdIt>
void unsigned_compact_slotset<Key, Alloc>::insert(FwdIt first, FwdIt last) {
	using value_t = typename std::iterator_traits<FwdIt>::value_type;
	static_assert(std::is_same_v<value_t, key_type>,
			"unsigned_set : Invalid iterators, do not point to key_type.");
	if (first == last) {
		return;
	}

	// Allocate once. Find max key to do so.
	FwdIt max_it = std::max_element(
			first, last, [](key_type lhs, key_type rhs) { return lhs < rhs; });
	maybe_resize(*max_it);

	// Fill the set.
	for (FwdIt it = first; it != last; ++it) {
		size_type lkp_idx = lookup_idx(*it);
		size_type lcl_idx = local_idx(*it);
		if (!_lookup[lkp_idx][lcl_idx]) {
			_lookup[lkp_idx][lcl_idx] = true;
			++_size;
		}
	}
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::insert(
		std::initializer_list<key_type>&& ilist) -> void {
	insert(ilist.begin(), ilist.end());
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::erase(key_type key) noexcept
		-> size_type {
	size_type lkp_idx = lookup_idx(key);
	if (lkp_idx >= _lookup.size()) {
		return size_type(0);
	}
	size_type lcl_idx = local_idx(key);
	if (!_lookup[lkp_idx][lcl_idx]) {
		return size_type(0);
	}

	_lookup[lkp_idx][lcl_idx] = false;
	assert(_size > size_type(0));
	--_size;
	return size_type(1);
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::erase(const_iterator cit) noexcept
		-> const_iterator {
	if (cit != end() && (*cit._current)[cit._local_idx]) {
		bool_type& bitset
				= *const_cast<bool_type*>(std::addressof(*cit._current));
		bitset[cit._local_idx] = false;
		--_size;
		++cit;
	}
	return cit;
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::erase(const_iterator cfirst,
		const_iterator clast) noexcept -> const_iterator {
	if (cfirst == clast) {
		return clast;
	}

	const_iterator it = cfirst;
	for (; it != clast; ++it) {
		erase(*it);
	}
	if (it != cend()) {
		++it;
	}
	return it;
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::swap(
		unsigned_compact_slotset& other) noexcept -> void {
	_lookup.swap(other._lookup);
	std::swap(_size, other._size);
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::merge(
		unsigned_compact_slotset& source) -> void {
	if (source._lookup.size() > _lookup.size()) {
		_lookup.resize(source._lookup.size());
	}

	assert(_lookup.size() >= source._lookup.size());
	size_type size = (std::min)(_lookup.size(), source._lookup.size());
	for (size_type i = 0; i < size; ++i) {
		for (size_type j = 0; j < bitset_size; ++j) {
			if (_lookup[i][j]) {
				continue;
			}
			if (source._lookup[i][j]) {
				source._lookup[i][j] = false;
				--source._size;
				_lookup[i][j] = true;
				++_size;
			}
		}
	}
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::merge(
		unsigned_compact_slotset&& source) -> void {
	merge(source);
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::count(key_type key) const noexcept
		-> size_type {
	return size_type(contains(key));
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::contains(key_type key) const noexcept
		-> bool {
	size_type lkp_idx = lookup_idx(key);
	if (lkp_idx >= _lookup.size()) {
		return false;
	}
	size_type lcl_idx = local_idx(key);
	return _lookup[lkp_idx][lcl_idx];
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::find(key_type key) const noexcept
		-> const_iterator {
	size_type lkp_idx = lookup_idx(key);
	if (lkp_idx >= _lookup.size()) {
		return end();
	}
	size_type lcl_idx = local_idx(key);
	if (!_lookup[lkp_idx][lcl_idx]) {
		return end();
	}
	return const_iterator{ _lookup.begin(), _lookup.end(),
		_lookup.begin() + lkp_idx, lcl_idx };
}

template <class Key, class Alloc>
const auto* unsigned_compact_slotset<Key, Alloc>::lookup_data() const noexcept {
	return _lookup.data();
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::lookup_size() const noexcept
		-> size_type {
	return _lookup.size();
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::lookup_idx(key_type key) noexcept
		-> size_type {
	return size_type(key) / bitset_size;
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::local_idx(key_type key) noexcept
		-> size_type {
	return size_type(key) % bitset_size;
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::lookup_idx(
		const_iterator it) noexcept -> size_type {
	return size_type(std::distance(it._first, it._current));
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::local_idx(const_iterator it) noexcept
		-> size_type {
	return it._local_idx;
}

template <class Key, class Alloc>
auto unsigned_compact_slotset<Key, Alloc>::maybe_resize(key_type key)
		-> size_type {
	size_type lkp_idx = lookup_idx(key);
	if (lkp_idx >= _lookup.size()) {
		_lookup.resize(lkp_idx + size_type(1));
	}
	return lkp_idx;
}

} // namespace fea
