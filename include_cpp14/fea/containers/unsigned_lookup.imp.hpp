#pragma once
namespace fea {
template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::size_type
unsigned_lookup<Key, TAlloc>::at_prehashed(underlying_key_type uk) const {
	size_type ret = find_prehashed(uk, sentinel());
	if (ret == sentinel()) {
		fea::maybe_throw<std::out_of_range>(
				__FUNCTION__, __LINE__, "invalid key");
	}
	return ret;
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::size_type
unsigned_lookup<Key, TAlloc>::at(const Key& k) const {
	underlying_key_type uk = hash(k);
	return at_prehashed(uk);
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::size_type
unsigned_lookup<Key, TAlloc>::at_unchecked_prehashed(
		underlying_key_type uk) const noexcept {
	assert(contains_prehashed(uk));
	return size_type(_indexes[uk]);
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::size_type
unsigned_lookup<Key, TAlloc>::at_unchecked(const Key& k) const noexcept {
	underlying_key_type uk = hash(k);
	return at_unchecked_prehashed(uk);
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::size_type
unsigned_lookup<Key, TAlloc>::find_prehashed(
		underlying_key_type uk, size_type end_size) const noexcept {
	if (uk >= _indexes.size()) {
		return end_size;
	}
	pos_type pos = _indexes[uk];
	if (pos == sentinel()) {
		return end_size;
	}
	assert(pos < end_size);
	return size_type(pos);
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::size_type
unsigned_lookup<Key, TAlloc>::find(
		const Key& k, size_type end_size) const noexcept {
	underlying_key_type uk = hash(k);
	return find_prehashed(uk, end_size);
}

template <class Key, class TAlloc>
constexpr bool unsigned_lookup<Key, TAlloc>::contains_prehashed(
		underlying_key_type uk) const noexcept {
	if (uk >= _indexes.size() || _indexes[uk] == sentinel()) {
		return false;
	}
	return true;
}

template <class Key, class TAlloc>
constexpr bool unsigned_lookup<Key, TAlloc>::contains(
		const Key& k) const noexcept {
	underlying_key_type uk = hash(k);
	return contains_prehashed(uk);
}

template <class Key, class TAlloc>
constexpr const typename unsigned_lookup<Key, TAlloc>::pos_type*
unsigned_lookup<Key, TAlloc>::data() const noexcept {
	return _indexes.data();
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::size_type
unsigned_lookup<Key, TAlloc>::size() const noexcept {
	return _indexes.size();
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::iterator
unsigned_lookup<Key, TAlloc>::begin() noexcept {
	return _indexes.begin();
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::const_iterator
unsigned_lookup<Key, TAlloc>::begin() const noexcept {
	return _indexes.begin();
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::const_iterator
unsigned_lookup<Key, TAlloc>::cbegin() const noexcept {
	return _indexes.cbegin();
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::iterator
unsigned_lookup<Key, TAlloc>::end() noexcept {
	return _indexes.end();
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::const_iterator
unsigned_lookup<Key, TAlloc>::end() const noexcept {
	return _indexes.end();
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::const_iterator
unsigned_lookup<Key, TAlloc>::cend() const noexcept {
	return _indexes.cend();
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::size_type
unsigned_lookup<Key, TAlloc>::max_size() const noexcept {
	// Reserve 1 slot for sentinel.
	return size_type(sentinel()) - size_type(1);
}

template <class Key, class TAlloc>
constexpr void unsigned_lookup<Key, TAlloc>::reserve(size_type new_cap) {
	_indexes.reserve(new_cap);
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::size_type
unsigned_lookup<Key, TAlloc>::capacity() const noexcept {
	return _indexes.capacity();
}

template <class Key, class TAlloc>
constexpr void unsigned_lookup<Key, TAlloc>::shrink_to_fit() {
	_indexes.shrink_to_fit();
}

template <class Key, class TAlloc>
constexpr void unsigned_lookup<Key, TAlloc>::clear() noexcept {
	_indexes.clear();
}

template <class Key, class TAlloc>
constexpr void unsigned_lookup<Key, TAlloc>::insert_prehashed(
		underlying_key_type uk, size_type new_idx) {
	assert(!contains_prehashed(uk));
	maybe_resize(uk);
	_indexes[uk] = pos_type(new_idx);
}

template <class Key, class TAlloc>
constexpr void unsigned_lookup<Key, TAlloc>::insert(
		const Key& k, size_type new_idx) {
	underlying_key_type uk = hash(k);
	insert_prehashed(uk, new_idx);
}

template <class Key, class TAlloc>
template <class FwdIt>
constexpr void unsigned_lookup<Key, TAlloc>::insert(
		FwdIt&& k_begin, FwdIt&& k_end, size_type first_new_idx) {
	auto max_it = std::max_element(
			k_begin, k_end, [](const Key& lhs, const Key& rhs) {
				return hash(lhs) < hash(rhs);
			});
	maybe_resize(*max_it);

	for (auto it = k_begin; it != k_end; ++it) {
		underlying_key_type uk = hash(*it);
		assert(!contains_prehashed(uk));
		_indexes[uk] = pos_type(first_new_idx++);
	}
}

template <class Key, class TAlloc>
constexpr void unsigned_lookup<Key, TAlloc>::swap(
		unsigned_lookup& other) noexcept {
	_indexes.swap(other._indexes);
}

template <class Key, class TAlloc>
constexpr void unsigned_lookup<Key, TAlloc>::invalidate_prehashed(
		underlying_key_type uk) noexcept {
	assert(contains_prehashed(uk));
	_indexes[uk] = sentinel();
}

template <class Key, class TAlloc>
constexpr void unsigned_lookup<Key, TAlloc>::invalidate(const Key& k) noexcept {
	underlying_key_type uk = hash(k);
	invalidate_prehashed(uk);
}

template <class Key, class TAlloc>
constexpr void unsigned_lookup<Key, TAlloc>::update_prehashed(
		underlying_key_type uk, size_type new_idx) noexcept {
	assert(contains_prehashed(uk));
	_indexes[uk] = pos_type(new_idx);
}

template <class Key, class TAlloc>
constexpr void unsigned_lookup<Key, TAlloc>::update(
		const Key& k, size_type new_idx) noexcept {
	underlying_key_type uk = hash(k);
	update_prehashed(uk, new_idx);
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::pos_type
unsigned_lookup<Key, TAlloc>::sentinel() noexcept {
	return (std::numeric_limits<pos_type>::max)();
}

template <class Key, class TAlloc>
constexpr typename unsigned_lookup<Key, TAlloc>::underlying_key_type
unsigned_lookup<Key, TAlloc>::hash(const Key& k) noexcept {
	return hasher{}(k);
}

template <class Key, class TAlloc>
void unsigned_lookup<Key, TAlloc>::maybe_resize(underlying_key_type uk) {
	if (uk < _indexes.size()) {
		return;
	}

	if (uk == sentinel()) {
		fea::maybe_throw<std::out_of_range>(
				__FUNCTION__, __LINE__, "maximum size reached");
	}

	_indexes.resize(size_t(uk) + 1u, sentinel());
	assert(uk < _indexes.size());
}
} // namespace fea