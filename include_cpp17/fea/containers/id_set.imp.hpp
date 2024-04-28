#pragma once
namespace fea {
namespace experimental {

template <class MySet>
constexpr id_set_const_iterator<MySet>::id_set_const_iterator(
		bool_pointer first, bool_pointer last, bool_pointer ptr) noexcept
		: _first(first)
		, _last(last)
		, _ptr(ptr) {
}

template <class MySet>
constexpr auto id_set_const_iterator<MySet>::operator*() const noexcept
		-> key_type {
	assert(*_ptr);
	return key_type{ underlying_key_type(std::distance(_first, _ptr)) };
}

template <class MySet>
constexpr id_set_const_iterator<MySet>&
id_set_const_iterator<MySet>::operator++() noexcept {
	assert(_ptr != _last);
	do {
		++_ptr;
	} while (!*_ptr && _ptr != _last);
	return *this;
}

template <class MySet>
constexpr id_set_const_iterator<MySet>&
id_set_const_iterator<MySet>::operator++(int) noexcept {
	id_set_const_iterator tmp = *this;
	++*this;
	return tmp;
}

template <class MySet>
constexpr id_set_const_iterator<MySet>&
id_set_const_iterator<MySet>::operator--() noexcept {
	assert(_ptr != _first);
	do {
		--_ptr;
	} while (!*_ptr && _ptr != _first);
	return *this;
}

template <class MySet>
constexpr id_set_const_iterator<MySet>&
id_set_const_iterator<MySet>::operator--(int) noexcept {
	id_set_const_iterator tmp = *this;
	--*this;
	return tmp;
}

template <class MySet>
bool id_set_const_iterator<MySet>::operator==(
		const id_set_const_iterator& rhs) const noexcept {
	return _ptr == rhs._ptr;
}

template <class MySet>
bool id_set_const_iterator<MySet>::operator!=(
		const id_set_const_iterator& rhs) const noexcept {
	return !(*this == rhs);
}

template <class MySet>
bool id_set_const_iterator<MySet>::operator<(
		const id_set_const_iterator& rhs) const noexcept {
	return _ptr < rhs._ptr;
}

template <class MySet>
bool id_set_const_iterator<MySet>::operator>(
		const id_set_const_iterator& rhs) const noexcept {
	return rhs < *this;
}

template <class MySet>
bool id_set_const_iterator<MySet>::operator<=(
		const id_set_const_iterator& rhs) const noexcept {
	return !(rhs < *this);
}

template <class MySet>
bool id_set_const_iterator<MySet>::operator>=(
		const id_set_const_iterator& rhs) const noexcept {
	return !(*this < rhs);
}


template <class Key, class Alloc>
template <class FwdIt>
id_set<Key, Alloc>::id_set(FwdIt first, FwdIt last) {
	insert(first, last);
}

template <class Key, class Alloc>
id_set<Key, Alloc>::id_set(std::initializer_list<key_type>&& ilist) {
	id_set(ilist.begin(), ilist.end());
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::begin() const noexcept -> const_iterator {
	return const_iterator{ _map.data(), _map.data() + _map.size(),
		_map.data() };
}
template <class Key, class Alloc>
auto id_set<Key, Alloc>::cbegin() const noexcept -> const_iterator {
	return begin();
}
template <class Key, class Alloc>
auto id_set<Key, Alloc>::begin() noexcept -> iterator {
	return iterator{ _map.data(), _map.data() + _map.size(), _map.data() };
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::end() const noexcept -> const_iterator {
	return const_iterator{ _map.data(), _map.data() + _map.size(),
		_map.data() + _map.size() };
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::cend() const noexcept -> const_iterator {
	return end();
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::end() noexcept -> iterator {
	return iterator{ _map.data(), _map.data() + _map.size(),
		_map.data() + _map.size() };
}

template <class Key, class Alloc>
bool id_set<Key, Alloc>::empty() const noexcept(
		noexcept(std::find(const_iterator{}, const_iterator{}, uint8_t{}))) {
	if (_map.empty()) {
		return true;
	}
	auto it = std::find(_map.begin(), _map.end(), uint8_t(true));
	return it == _map.end();
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::size() const noexcept -> size_type {
	// Q : cache?
	return size_type(std::count(_map.begin(), _map.end(), uint8_t(true)));
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::max_size() const noexcept -> size_type {
	return _map.max_size();
}

template <class Key, class Alloc>
void id_set<Key, Alloc>::reserve(const key_type& key) {
	size_type new_cap = hasher{}(key) + size_type(1);
	_map.reserve(new_cap);
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::capacity() const noexcept -> size_type {
	return _map.capacity();
}

template <class Key, class Alloc>
void id_set<Key, Alloc>::shrink_to_fit() {
	_map.shrink_to_fit();
}

template <class Key, class Alloc>
constexpr void id_set<Key, Alloc>::clear() noexcept {
	_map.clear();
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::insert(const key_type&) -> std::pair<iterator, bool> {
	assert(false); // todo
	return {};
}

template <class Key, class Alloc>
template <class FwdIt>
void id_set<Key, Alloc>::insert(FwdIt first, FwdIt last) {
	using value_t = typename std::iterator_traits<FwdIt>::value_type;
	static_assert(std::is_same_v<value_t, key_type>,
			"unsigned_set : Invalid iterators, do not point to key_type.");
	if (std::distance(first, last) == 0) {
		return;
	}

	// Allocate once. Find max key to do so.
	FwdIt max_it = std::max_element(
			first, last, [](const key_type& lhs, const key_type& rhs) {
				return hasher{}(lhs) < hasher{}(rhs);
			});

	size_type size = size_type(hasher{}(*max_it)) + size_type(1);
	_map.resize(size);

	// Fill the set.
	for (FwdIt it = first; it != last; ++it) {
		size_type idx = size_type(hasher{}(*it));
		if (!_map[idx]) {
			_map[idx] = uint8_t(true);
		}
	}
}

template <class Key, class Alloc>
void id_set<Key, Alloc>::insert(std::initializer_list<key_type>&& ilist) {
	insert(ilist.begin(), ilist.end());
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::erase(iterator) -> iterator {
	assert(false); // todo
	return {};
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::erase(const_iterator) -> iterator {
	assert(false); // todo
	return {};
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::erase(const key_type& key) -> size_type {
	size_type idx = size_type(hasher{}(key));
	if (idx >= _map || !_map[idx]) {
		return size_type(0);
	}
	_map[idx] = uint8_t(false);
	return size_type(1);
}

template <class Key, class Alloc>
void id_set<Key, Alloc>::swap(id_set& other) noexcept {
	_map.swap(other._map);
}

template <class Key, class Alloc>
void id_set<Key, Alloc>::merge(id_set<Key, Alloc>& source) {
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
void id_set<Key, Alloc>::merge(id_set<Key, Alloc>&& source) {
	merge(source);
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::count(const key_type& key) const
		noexcept(noexcept(hasher{}(key))) -> size_type {
	size_type idx = size_type(hasher{}(key));
	if (idx >= _map.size()) {
		return size_type(0);
	}
	return size_type(_map[idx]);
}

template <class Key, class Alloc>
bool id_set<Key, Alloc>::contains(const key_type& key) const {
	return bool(count(key));
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::find(const key_type& key) const
		noexcept(noexcept(hasher{}(key))) -> const_iterator {
	size_type idx = size_type(hasher{}(key));
	if (idx >= _map.size() || !_map[idx]) {
		return const_iterator{ _map.end() };
	}
	return const_iterator{ _map.begin() + idx };
}

template <class Key, class Alloc>
auto id_set<Key, Alloc>::find(const key_type& key) noexcept(
		noexcept(hasher{}(key))) -> iterator {
	size_type idx = size_type(hasher{}(key));
	if (idx >= _map.size() || !_map[idx]) {
		return end();
	}
	return iterator{ _map.data(), _map.data() + _map.size(),
		_map.data() + idx };
}

} // namespace experimental
} // namespace fea