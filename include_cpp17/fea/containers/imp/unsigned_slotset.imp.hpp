#pragma once
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
	return const_iterator{ _map.data(), _map.data() + _map.size(),
		_map.data() };
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::cbegin() const noexcept -> const_iterator {
	return begin();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::begin() noexcept -> iterator {
	return iterator{ _map.data(), _map.data() + _map.size(), _map.data() };
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::end() const noexcept -> const_iterator {
	return const_iterator{ _map.data(), _map.data() + _map.size(),
		_map.data() + _map.size() };
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::cend() const noexcept -> const_iterator {
	return end();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::end() noexcept -> iterator {
	return iterator{ _map.data(), _map.data() + _map.size(),
		_map.data() + _map.size() };
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::empty() const noexcept -> bool {
	for (uint8_t v : _map) {
		if (v) {
			return false;
		}
	}
	return true;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::size() const noexcept -> size_type {
	// Q : cache?
	return size_type(std::count(_map.begin(), _map.end(), uint8_t(true)));
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::max_size() const noexcept -> size_type {
	return _map.max_size();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::reserve(key_type key) -> void {
	size_type new_cap = size_type(key) + size_type(1);
	_map.reserve(new_cap);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::capacity() const noexcept -> size_type {
	return _map.capacity();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::shrink_to_fit() -> void {
	_map.shrink_to_fit();
}

template <class Key, class Alloc>
constexpr auto unsigned_slotset<Key, Alloc>::clear() noexcept -> void {
	_map.clear();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::insert(key_type key)
		-> std::pair<iterator, bool> {
	iterator it = find(key);
	if (it != end()) {
		return { it, false };
	}

	size_type idx = size_type(key);
	if (idx >= _map.size()) {
		_map.resize(idx + size_type(1));
	}
	_map[idx] = uint8_t(true);
	return {
		iterator{ _map.data(), _map.data() + _map.size(), _map.data() + idx },
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

	size_type size = size_type(*max_it) + size_type(1);
	_map.resize(size);

	// Fill the set.
	for (FwdIt it = first; it != last; ++it) {
		size_type idx = size_type(*it);
		if (!_map[idx]) {
			_map[idx] = uint8_t(true);
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
	if (idx >= _map || !_map[idx]) {
		return size_type(0);
	}
	_map[idx] = uint8_t(false);
	return size_type(1);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::erase(const_iterator cit) noexcept
		-> iterator {
	iterator it{ cit._first, cit._last, cit._ptr };
	if (it == end() || !*it._ptr) {
		return it;
	}

	// Permitted, iterator constructed from non-const pointers.
	using bool_ptr_t = typename iterator::bool_pointer;
	*const_cast<bool_ptr_t>(it._ptr) = uint8_t(false);
	++it;
	return it;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::erase(iterator it) noexcept -> iterator {
	return erase(const_iterator{ it });
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::erase(
		const_iterator cfirst, const_iterator clast) noexcept -> iterator {
	iterator it{ cfirst._first, cfirst._last, cfirst._ptr };
	iterator last{ clast._first, clast._last, clast._ptr };
	if (it == last) {
		return it;
	}

	for (; it != last; ++it) {
		if (!*it._ptr) {
			continue;
		}

		// Permitted, iterator constructed from non-const pointers.
		using bool_ptr_t = typename iterator::bool_pointer;
		*const_cast<bool_ptr_t>(it._ptr) = uint8_t(false);
	}

	if (it != end()) {
		++it;
	}
	return it;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::swap(unsigned_slotset& other) noexcept
		-> void {
	_map.swap(other._map);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::merge(unsigned_slotset<Key, Alloc>& source)
		-> void {
	if (source._map.size() > _map.size()) {
		_map.resize(source._map.size());
	}

	assert(_map.size() >= source._map.size());
	size_type size = (std::min)(_map.size(), source._map.size());
	for (size_type i = 0; i < size; ++i) {
		if (_map[i]) {
			continue;
		}
		if (source._map[i]) {
			source._map[i] = uint8_t(false);
			_map[i] = uint8_t(true);
		}
	}
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::merge(unsigned_slotset<Key, Alloc>&& source)
		-> void {
	merge(source);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::count(key_type key) const noexcept
		-> size_type {
	size_type idx = size_type(key);
	if (idx >= _map.size()) {
		return size_type(0);
	}
	return size_type(_map[idx]);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::contains(key_type key) const noexcept
		-> bool {
	return bool(count(key));
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::find(key_type key) const noexcept
		-> const_iterator {
	size_type idx = size_type(key);
	if (idx >= _map.size() || !_map[idx]) {
		return end();
	}
	return const_iterator{ _map.data(), _map.data() + _map.size(),
		_map.data() + idx };
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::find(key_type key) noexcept -> iterator {
	const_iterator it = std::as_const(*this).find(key);
	return iterator{ it._first, it._last, it._ptr };
}

} // namespace fea