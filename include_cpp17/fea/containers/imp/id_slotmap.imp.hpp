namespace fea {
template <class Key, class T, class Alloc>
id_slotmap<Key, T, Alloc>::id_slotmap(size_t new_cap) {
	reserve(new_cap);
}

template <class Key, class T, class Alloc>
id_slotmap<Key, T, Alloc>::id_slotmap(
		size_t key_new_cap, size_t value_new_cap) {
	reserve(key_new_cap, value_new_cap);
}

template <class Key, class T, class Alloc>
template <class InputIt>
id_slotmap<Key, T, Alloc>::id_slotmap(InputIt first, InputIt last) {
	// TODO : benchmark and potentially optimize
	for (auto it = first; it != last; ++it) {
		insert(*it);
	}
}

template <class Key, class T, class Alloc>
id_slotmap<Key, T, Alloc>::id_slotmap(
		const std::initializer_list<value_type>& init) {
	// TODO : benchmark and potentially optimize
	for (const value_type& kv : init) {
		insert(kv);
	}
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::iterator id_slotmap<Key, T,
		Alloc>::begin() noexcept {
	return _values.begin();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::const_iterator id_slotmap<Key, T,
		Alloc>::begin() const noexcept {
	return _values.begin();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::const_iterator id_slotmap<Key, T,
		Alloc>::cbegin() const noexcept {
	return _values.cbegin();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::iterator id_slotmap<Key, T,
		Alloc>::end() noexcept {
	return _values.end();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::const_iterator id_slotmap<Key, T,
		Alloc>::end() const noexcept {
	return _values.end();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::const_iterator id_slotmap<Key, T,
		Alloc>::cend() const noexcept {
	return _values.cend();
}

template <class Key, class T, class Alloc>
bool id_slotmap<Key, T, Alloc>::empty() const noexcept {
	return _values.empty();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T, Alloc>::size()
		const noexcept {
	return _values.size();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T,
		Alloc>::max_size() const noexcept {
	return _lookup.max_size();
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::reserve(size_type new_cap) {
	_lookup.reserve(new_cap);
	_values.reserve(new_cap);
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::reserve(
		size_type key_new_cap, size_type value_new_cap) {
	_lookup.reserve(key_new_cap);
	_values.reserve(value_new_cap);
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T,
		Alloc>::capacity() const noexcept {
	return _values.capacity();
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::shrink_to_fit() {
	_lookup.shrink_to_fit();
	_values.shrink_to_fit();
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::clear() noexcept {
	_lookup.clear();
	_values.clear();
}

template <class Key, class T, class Alloc>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::insert(const value_type& value) {
	return minsert(value.first, value.second);
}

template <class Key, class T, class Alloc>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::insert(value_type&& value) {
	return minsert(value.first, fea::move_if_moveable(value.second));
}

template <class Key, class T, class Alloc>
template <class InputIt>
void id_slotmap<Key, T, Alloc>::insert(InputIt first, InputIt last) {
	// TODO : benchmark and potentially optimize
	for (auto it = first; it != last; ++it) {
		insert(*it);
	}
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::insert(
		const std::initializer_list<value_type>& ilist) {
	// TODO : benchmark and potentially optimize
	for (const value_type& kv : ilist) {
		insert(kv);
	}
}

template <class Key, class T, class Alloc>
template <class M>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::insert_or_assign(const key_type& k, M&& obj) {
	return minsert(k, std::forward<M>(obj), true);
}

template <class Key, class T, class Alloc>
template <class... Args>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::emplace(const key_type& k, Args&&... args) {
	iterator it = find(k);
	if (it != end()) {
		return { it, false };
	}

	_lookup.insert(k, _values.size());
	_values.emplace_back(k, std::forward<Args>(args)...);
	return { std::prev(_values.end()), true };
}

template <class Key, class T, class Alloc>
template <class... Args>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::try_emplace(const key_type& key, Args&&... args) {
	// Standard emplace behavior doesn't apply, always use try_emplace
	// behavior.
	return emplace(key, std::forward<Args>(args)...);
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::iterator id_slotmap<Key, T, Alloc>::erase(
		const_iterator pos) {
	size_t idx = std::distance(_values.cbegin(), pos);
	erase(pos->first);

	if (idx >= _values.size())
		return end();

	return find(_values[idx].first);
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::iterator id_slotmap<Key, T, Alloc>::erase(
		const_iterator first, const_iterator last) {
	size_t first_idx = std::distance(_values.cbegin(), first);
	size_t last_idx = std::distance(_values.cbegin(), last);


	std::vector<key_type> to_erase;
	to_erase.reserve(last_idx - first_idx);
	for (auto it = _values.begin() + first_idx;
			it != _values.begin() + last_idx; ++it) {
		to_erase.push_back(it->first);
	}

	for (key_type& k : to_erase) {
		erase(k);
	}

	if (first_idx >= _values.size())
		return end();
	return find(_values[first_idx].first);
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T, Alloc>::erase(
		const key_type& k) {
	iterator it = find(k);
	if (it == end()) {
		return 0;
	}

	_lookup.invalidate(k);
	iterator last_it = std::prev(end());

	// No need to swap if object is already at end.
	if (last_it == it) {
		_values.pop_back();
		return 1;
	}

	// swap & pop
	size_type new_idx = size_type(std::distance(_values.begin(), it));
	key_type last_key = _values.back().first;

	*it = fea::move_if_moveable(_values.back());
	_values.pop_back();
	_lookup.update(last_key, new_idx);
	return 1;
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::swap(id_slotmap& other) noexcept {
	_lookup.swap(other._lookup);
	_values.swap(other._values);
}

template <class Key, class T, class Alloc>
const typename id_slotmap<Key, T, Alloc>::value_type* id_slotmap<Key, T,
		Alloc>::data() const noexcept {
	return _values.data();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::value_type* id_slotmap<Key, T,
		Alloc>::data() noexcept {
	return _values.data();
}

template <class Key, class T, class Alloc>
const auto* id_slotmap<Key, T, Alloc>::lookup_data() const noexcept {
	return _lookup.data();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T,
		Alloc>::lookup_size() const noexcept {
	return _lookup.size();
}

template <class Key, class T, class Alloc>
const typename id_slotmap<Key, T, Alloc>::mapped_type& id_slotmap<Key, T,
		Alloc>::at(const key_type& k) const {
	auto it = find(k);
	if (it == end()) {
		fea::maybe_throw<std::out_of_range>(
				__FUNCTION__, __LINE__, "value doesn't exist");
	}
	return it->second;
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::mapped_type& id_slotmap<Key, T, Alloc>::at(
		const key_type& k) {
	return const_cast<mapped_type&>(
			static_cast<const id_slotmap*>(this)->at(k));
}

template <class Key, class T, class Alloc>
const typename id_slotmap<Key, T, Alloc>::mapped_type& id_slotmap<Key, T,
		Alloc>::at_unchecked(const key_type& k) const noexcept {
	return _values[_lookup.at_unchecked(k)].second;
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::mapped_type& id_slotmap<Key, T,
		Alloc>::at_unchecked(const key_type& k) noexcept {
	return const_cast<mapped_type&>(
			static_cast<const id_slotmap*>(this)->at_unchecked(k));
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::mapped_type& id_slotmap<Key, T,
		Alloc>::operator[](const key_type& k) {
	if (!contains(k)) {
		emplace(k, mapped_type{});
	}

	return at_unchecked(k);
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T, Alloc>::count(
		const key_type& k) const noexcept {
	return contains(k) ? 1 : 0;
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::const_iterator id_slotmap<Key, T,
		Alloc>::find(const key_type& k) const noexcept {
	return begin() + _lookup.find(k, size());
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::iterator id_slotmap<Key, T, Alloc>::find(
		const key_type& k) noexcept {
	return begin() + _lookup.find(k, size());
}

template <class Key, class T, class Alloc>
bool id_slotmap<Key, T, Alloc>::contains(const key_type& k) const noexcept {
	return _lookup.contains(k);
}

template <class Key, class T, class Alloc>
std::pair<typename id_slotmap<Key, T, Alloc>::const_iterator,
		typename id_slotmap<Key, T, Alloc>::const_iterator>
id_slotmap<Key, T, Alloc>::equal_range(const key_type& k) const noexcept {
	const_iterator it = find(k);
	if (it == end()) {
		return { it, it };
	}
	return { it, it + 1 };
}

template <class Key, class T, class Alloc>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator,
		typename id_slotmap<Key, T, Alloc>::iterator>
id_slotmap<Key, T, Alloc>::equal_range(const key_type& k) noexcept {
	iterator it = find(k);
	if (it == end()) {
		return { it, it };
	}
	return { it, it + 1 };
}

template <class Key, class T, class Alloc>
template <class M>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::minsert(const key_type& k, M&& obj, bool assign_found) {
	iterator it = find(k);
	if (it != end()) {
		if (assign_found) {
			it->second = std::forward<M>(obj);
		}
		return { it, false };
	}

	_lookup.insert(k, _values.size());
	_values.push_back({ k, std::forward<M>(obj) });
	return { std::prev(_values.end()), true };
}

template <class Key, class T, class Alloc>
[[nodiscard]]
bool operator==(const id_slotmap<Key, T, Alloc>& lhs,
		const id_slotmap<Key, T, Alloc>& rhs) {
	if (lhs.size() != rhs.size())
		return false;

	for (const auto& kv : lhs) {
		// Key doesn't exist in rhs, not equal.
		if (!rhs.contains(kv.first))
			return false;

		// Compare value.
		if (!(kv == *rhs.find(kv.first)))
			return false;
	}

	return true;
}

template <class Key, class T, class Alloc>
[[nodiscard]]
bool operator!=(const id_slotmap<Key, T, Alloc>& lhs,
		const id_slotmap<Key, T, Alloc>& rhs) {
	return !operator==(lhs, rhs);
}
} // namespace fea

namespace std {
template <class Key, class T, class Alloc>
inline void swap(fea::id_slotmap<Key, T, Alloc>& lhs,
		fea::id_slotmap<Key, T, Alloc>& rhs) noexcept {
	lhs.swap(rhs);
}
} // namespace std