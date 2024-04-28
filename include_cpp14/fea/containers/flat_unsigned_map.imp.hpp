#pragma once
namespace fea {
template <class Key, class T, class Alloc>
flat_unsigned_map<Key, T, Alloc>::flat_unsigned_map(size_type reserve_count) {
	reserve(reserve_count);
}

template <class Key, class T, class Alloc>
flat_unsigned_map<Key, T, Alloc>::flat_unsigned_map(
		size_type key_reserve_count, size_type value_reserve_count) {
	reserve(key_reserve_count, value_reserve_count);
}

template <class Key, class T, class Alloc>
template <class InputIt, class InputValIt>
flat_unsigned_map<Key, T, Alloc>::flat_unsigned_map(InputIt first_key,
		InputIt last_key, InputValIt first_val, InputValIt last_val) {
	if (std::distance(first_key, last_key)
			!= std::distance(first_val, last_val)) {
		fea::maybe_throw(
				__FUNCTION__, __LINE__, "mismatch amount of keys and values");
	}

	// TODO : benchmark and potentially optimize
	auto val_it = first_val;
	for (auto it = first_key; it != last_key; ++it) {
		insert(*it, *val_it);
		++val_it;
	}
	assert(_values.size() == _reverse_lookup.size());
}

template <class Key, class T, class Alloc>
flat_unsigned_map<Key, T, Alloc>::flat_unsigned_map(
		const std::initializer_list<key_type>& keys,
		const std::initializer_list<value_type>& values) {
	if (keys.size() != values.size()) {
		fea::maybe_throw(
				__FUNCTION__, __LINE__, "mismatch amount of keys and values");
	}

	// TODO : benchmark and potentially optimize
	auto val_it = values.begin();
	for (auto it = keys.begin(); it != keys.end(); ++it) {
		insert(*it, *val_it);
		++val_it;
	}
	assert(_values.size() == _reverse_lookup.size());
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::iterator
flat_unsigned_map<Key, T, Alloc>::begin() noexcept {
	return _values.begin();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::const_iterator
flat_unsigned_map<Key, T, Alloc>::begin() const noexcept {
	return _values.begin();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::const_iterator
flat_unsigned_map<Key, T, Alloc>::cbegin() const noexcept {
	return _values.cbegin();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::iterator
flat_unsigned_map<Key, T, Alloc>::end() noexcept {
	return _values.end();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::const_iterator
flat_unsigned_map<Key, T, Alloc>::end() const noexcept {
	return _values.end();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::const_iterator
flat_unsigned_map<Key, T, Alloc>::cend() const noexcept {
	return _values.cend();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::const_key_iterator
flat_unsigned_map<Key, T, Alloc>::key_begin() const noexcept {
	return _reverse_lookup.begin();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::const_key_iterator
flat_unsigned_map<Key, T, Alloc>::key_cbegin() const noexcept {
	return _reverse_lookup.cbegin();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::const_key_iterator
flat_unsigned_map<Key, T, Alloc>::key_end() const noexcept {
	return _reverse_lookup.end();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::const_key_iterator
flat_unsigned_map<Key, T, Alloc>::key_cend() const noexcept {
	return _reverse_lookup.cend();
}

template <class Key, class T, class Alloc>
bool flat_unsigned_map<Key, T, Alloc>::empty() const noexcept {
	return _values.empty();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::size_type
flat_unsigned_map<Key, T, Alloc>::size() const noexcept {
	return _values.size();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::size_type
flat_unsigned_map<Key, T, Alloc>::max_size() const noexcept {
	return _lookup.max_size();
}

template <class Key, class T, class Alloc>
void flat_unsigned_map<Key, T, Alloc>::reserve(size_type new_cap) {
	_lookup.reserve(new_cap);
	_reverse_lookup.reserve(new_cap);
	_values.reserve(new_cap);
}

template <class Key, class T, class Alloc>
void flat_unsigned_map<Key, T, Alloc>::reserve(
		size_type lookup_new_cap, size_type value_new_cap) {
	_lookup.reserve(lookup_new_cap);
	_reverse_lookup.reserve(value_new_cap);
	_values.reserve(value_new_cap);
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::size_type
flat_unsigned_map<Key, T, Alloc>::lookup_capacity() const noexcept {
	return _lookup.capacity();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::size_type
flat_unsigned_map<Key, T, Alloc>::capacity() const noexcept {
	return _values.capacity();
}

template <class Key, class T, class Alloc>
void flat_unsigned_map<Key, T, Alloc>::shrink_to_fit() {
	_lookup.shrink_to_fit();
	_reverse_lookup.shrink_to_fit();
	_values.shrink_to_fit();
}

template <class Key, class T, class Alloc>
void flat_unsigned_map<Key, T, Alloc>::clear() noexcept {
	_lookup.clear();
	_reverse_lookup.clear();
	_values.clear();
}

template <class Key, class T, class Alloc>
std::pair<typename flat_unsigned_map<Key, T, Alloc>::iterator, bool>
flat_unsigned_map<Key, T, Alloc>::insert(
		const key_type& k, const value_type& v) {
	return minsert(k, v);
}

template <class Key, class T, class Alloc>
std::pair<typename flat_unsigned_map<Key, T, Alloc>::iterator, bool>
flat_unsigned_map<Key, T, Alloc>::insert(key_type&& k, value_type&& v) {
	return minsert(k, fea::maybe_move(v));
}

template <class Key, class T, class Alloc>
template <class KeyIt, class ValIt>
void flat_unsigned_map<Key, T, Alloc>::insert(
		KeyIt first_key, KeyIt last_key, ValIt first_val, ValIt last_val) {
	if (std::distance(first_key, last_key)
			!= std::distance(first_val, last_val)) {
		fea::maybe_throw(
				__FUNCTION__, __LINE__, "mismatch amount of keys and values");
	}

	// TODO : benchmark and potentially optimize
	auto val_it = first_val;
	for (auto it = first_key; it != last_key; ++it) {
		insert(*it, *val_it);
		++val_it;
	}
	assert(_values.size() == _reverse_lookup.size());
}

template <class Key, class T, class Alloc>
void flat_unsigned_map<Key, T, Alloc>::insert(
		const std::initializer_list<key_type>& keys,
		const std::initializer_list<value_type>& values) {
	if (keys.size() != values.size()) {
		fea::maybe_throw(
				__FUNCTION__, __LINE__, "mismatch amount of keys and values");
	}

	// TODO : benchmark and potentially optimize
	auto val_it = values.begin();
	for (auto it = keys.begin(); it != keys.end(); ++it) {
		insert(*it, *val_it);
		++val_it;
	}
	assert(_values.size() == _reverse_lookup.size());
}

template <class Key, class T, class Alloc>
template <class M>
std::pair<typename flat_unsigned_map<Key, T, Alloc>::iterator, bool>
flat_unsigned_map<Key, T, Alloc>::insert_or_assign(const key_type& k, M&& obj) {
	return minsert(k, std::forward<M>(obj), true);
}

template <class Key, class T, class Alloc>
template <class... Args>
std::pair<typename flat_unsigned_map<Key, T, Alloc>::iterator, bool>
flat_unsigned_map<Key, T, Alloc>::emplace(const key_type& k, Args&&... args) {
	iterator it = find(k);
	if (it != end()) {
		return { it, false };
	}

	_lookup.insert(k, _values.size());
	_reverse_lookup.push_back(k);
	_values.emplace_back(std::forward<Args>(args)...);

	assert(_values.size() == _reverse_lookup.size());
	return { std::prev(_values.end()), true };
}

template <class Key, class T, class Alloc>
template <class... Args>
std::pair<typename flat_unsigned_map<Key, T, Alloc>::iterator, bool>
flat_unsigned_map<Key, T, Alloc>::try_emplace(
		const key_type& key, Args&&... args) {
	// Standard emplace behavior doesn't apply, always use try_emplace
	// behavior.
	return emplace(key, std::forward<Args>(args)...);
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::iterator
flat_unsigned_map<Key, T, Alloc>::erase(const_iterator pos) {
	size_t idx = std::distance(_values.cbegin(), pos);
	erase(_reverse_lookup[idx]);
	assert(_values.size() == _reverse_lookup.size());

	if (idx >= _values.size()) {
		return end();
	}

	return _values.begin() + idx;
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::iterator
flat_unsigned_map<Key, T, Alloc>::erase(
		const_iterator first, const_iterator last) {
	size_t first_idx = std::distance(_values.cbegin(), first);
	size_t last_idx = std::distance(_values.cbegin(), last);
	assert(first_idx < _values.size());
	assert(last_idx <= _values.size());

	std::vector<key_type> to_erase;
	to_erase.reserve(last_idx - first_idx);
	for (auto it = _reverse_lookup.begin() + first_idx;
			it != _reverse_lookup.begin() + last_idx; ++it) {
		to_erase.push_back(*it);
	}

	for (key_type& k : to_erase) {
		erase(k);
	}

	assert(_values.size() == _reverse_lookup.size());
	if (first_idx >= _values.size()) {
		return end();
	}
	return _values.begin() + first_idx;
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::size_type
flat_unsigned_map<Key, T, Alloc>::erase(const key_type& k) {
	iterator it = find(k);
	if (it == end()) {
		return 0;
	}

	iterator last_val_it = std::prev(end());
	_lookup.invalidate(k);

	// No need for swap, object is already at end.
	if (last_val_it == it) {
		_values.pop_back();
		_reverse_lookup.pop_back();
		assert(_values.size() == _reverse_lookup.size());
		return 1;
	}

	// swap & pop
	size_type new_idx = size_type(std::distance(_values.begin(), it));
	key_iterator key_it = _reverse_lookup.begin() + new_idx;
	key_type last_key = _reverse_lookup.back();

	*it = fea::maybe_move(_values.back());
	*key_it = _reverse_lookup.back();
	_values.pop_back();
	_reverse_lookup.pop_back();
	_lookup.update(last_key, new_idx);
	assert(_values.size() == _reverse_lookup.size());
	return 1;
}

template <class Key, class T, class Alloc>
void flat_unsigned_map<Key, T, Alloc>::swap(flat_unsigned_map& other) noexcept {
	_lookup.swap(other._lookup);
	_reverse_lookup.swap(other._reverse_lookup);
	_values.swap(other._values);
}

template <class Key, class T, class Alloc>
const typename flat_unsigned_map<Key, T, Alloc>::value_type*
flat_unsigned_map<Key, T, Alloc>::data() const noexcept {
	return _values.data();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::value_type*
flat_unsigned_map<Key, T, Alloc>::data() noexcept {
	return _values.data();
}

template <class Key, class T, class Alloc>
const typename flat_unsigned_map<Key, T, Alloc>::key_type*
flat_unsigned_map<Key, T, Alloc>::key_data() const noexcept {
	return _reverse_lookup.data();
}

template <class Key, class T, class Alloc>
const auto* flat_unsigned_map<Key, T, Alloc>::lookup_data() const noexcept {
	return _lookup.data();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::size_type
flat_unsigned_map<Key, T, Alloc>::lookup_size() const noexcept {
	return _lookup.size();
}

template <class Key, class T, class Alloc>
const typename flat_unsigned_map<Key, T, Alloc>::mapped_type&
flat_unsigned_map<Key, T, Alloc>::at(const key_type& k) const {
	auto it = find(k);
	if (it == end()) {
		fea::maybe_throw<std::out_of_range>(
				__FUNCTION__, __LINE__, "key doesn't exist");
	}
	return *it;
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::mapped_type&
flat_unsigned_map<Key, T, Alloc>::at(const key_type& k) {
	return const_cast<mapped_type&>(
			static_cast<const flat_unsigned_map*>(this)->at(k));
}

template <class Key, class T, class Alloc>
const typename flat_unsigned_map<Key, T, Alloc>::mapped_type&
flat_unsigned_map<Key, T, Alloc>::at_unchecked(
		const key_type& k) const noexcept {
	return _values[_lookup.at_unchecked(k)];
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::mapped_type&
flat_unsigned_map<Key, T, Alloc>::at_unchecked(const key_type& k) noexcept {
	return const_cast<mapped_type&>(
			static_cast<const flat_unsigned_map*>(this)->at_unchecked(k));
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::mapped_type&
flat_unsigned_map<Key, T, Alloc>::operator[](const key_type& k) {
	if (!contains(k)) {
		emplace(k, mapped_type{});
	}

	return at_unchecked(k);
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::size_type
flat_unsigned_map<Key, T, Alloc>::count(const key_type& k) const noexcept {
	return contains(k) ? 1 : 0;
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::const_iterator
flat_unsigned_map<Key, T, Alloc>::find(const key_type& k) const noexcept {
	return begin() + _lookup.find(k, size());
}

template <class Key, class T, class Alloc>
typename flat_unsigned_map<Key, T, Alloc>::iterator
flat_unsigned_map<Key, T, Alloc>::find(const key_type& k) noexcept {
	return begin() + _lookup.find(k, size());
}

template <class Key, class T, class Alloc>
bool flat_unsigned_map<Key, T, Alloc>::contains(
		const key_type& k) const noexcept {
	return _lookup.contains(k);
}

template <class Key, class T, class Alloc>
std::pair<typename flat_unsigned_map<Key, T, Alloc>::const_iterator,
		typename flat_unsigned_map<Key, T, Alloc>::const_iterator>
flat_unsigned_map<Key, T, Alloc>::equal_range(
		const key_type& k) const noexcept {
	const_iterator it = find(k);
	if (it == end()) {
		return { it, it };
	}
	return { it, it + 1 };
}

template <class Key, class T, class Alloc>
std::pair<typename flat_unsigned_map<Key, T, Alloc>::iterator,
		typename flat_unsigned_map<Key, T, Alloc>::iterator>
flat_unsigned_map<Key, T, Alloc>::equal_range(const key_type& k) noexcept {
	iterator it = find(k);
	if (it == end()) {
		return { it, it };
	}
	return { it, it + 1 };
}

template <class Key, class T, class Alloc>
template <class M>
std::pair<typename flat_unsigned_map<Key, T, Alloc>::iterator, bool>
flat_unsigned_map<Key, T, Alloc>::minsert(
		const key_type& k, M&& obj, bool assign_found) {
	iterator it = find(k);
	if (it != end()) {
		if (assign_found) {
			*it = std::forward<M>(obj);
		}
		return { it, false };
	}

	_lookup.insert(k, _values.size());
	_reverse_lookup.push_back(k);
	_values.push_back(std::forward<M>(obj));
	assert(_values.size() == _reverse_lookup.size());
	return { std::prev(_values.end()), true };
}


template <class Key, class T, class Alloc>
bool operator==(const flat_unsigned_map<Key, T, Alloc>& lhs,
		const flat_unsigned_map<Key, T, Alloc>& rhs) {
	if (lhs.size() != rhs.size())
		return false;


	for (auto it = lhs.key_begin(); it != lhs.key_end(); ++it) {
		// Key doesn't exist in rhs, not equal.
		if (!rhs.contains(*it))
			return false;

		// Compare value.
		if (!(lhs.at_unchecked(*it) == rhs.at_unchecked(*it)))
			return false;
	}

	return true;
}
template <class Key, class T, class Alloc>
bool operator!=(const flat_unsigned_map<Key, T, Alloc>& lhs,
		const flat_unsigned_map<Key, T, Alloc>& rhs) {
	return !operator==(lhs, rhs);
}
} // namespace fea

namespace std {
template <class Key, class T, class Alloc>
void swap(fea::flat_unsigned_map<Key, T, Alloc>& lhs,
		fea::flat_unsigned_map<Key, T, Alloc>& rhs) noexcept {
	lhs.swap(rhs);
}
} // namespace std
