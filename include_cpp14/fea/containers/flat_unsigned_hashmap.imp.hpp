#pragma once
namespace fea {
namespace detail {
// https://stackoverflow.com/questions/30052316/find-next-prime-number-algorithm
template <class T>
bool is_prime(T number) {
	if (number == 2 || number == 3)
		return true;

	if ((number & 1) == 0 || number % 3 == 0)
		return false;

	T divisor = 6;
	while (divisor * divisor - 2 * divisor + 1 <= number) {

		if (number % (divisor - 1) == 0)
			return false;

		if (number % (divisor + 1) == 0)
			return false;

		divisor += 6;
	}

	return true;
}

template <class T>
T next_prime(T a) {
	switch (a) {
	case 3: {
		return 3;
	} break;
	case 6: {
		return 7;
	} break;
	case 14: {
		return 17;
	} break;
	case 34: {
		return 37;
	} break;
	case 74: {
		return 79;
	} break;
	case 158: {
		return 163;
	} break;
	case 326: {
		return 331;
	} break;
	case 662: {
		return 673;
	} break;
	case 1'346: {
		return 1361;
	} break;
	case 2'722: {
		return 2729;
	} break;
	case 5'458: {
		return 5471;
	} break;
	case 10'942: {
		return 10949;
	} break;
	case 21'898: {
		return 21911;
	} break;
	case 43'822: {
		return 43853;
	} break;
	case 87'706: {
		return 87719;
	} break;
	case 175'438: {
		return 175447;
	} break;
	case 350'894: {
		return 350899;
	} break;
	case 701'798: {
		return 701819;
	} break;
	case 1'403'638: {
		return 1403641;
	} break;
	case 2'807'282: {
		return 2807303;
	} break;
	case 5'614'606: {
		return 5614657;
	} break;
	default: {
		while (!is_prime(++a)) {
		}
		return a;
	} break;
	}
}
} // namespace detail

template <class Key, class T, class Alloc>
flat_unsigned_hashmap<Key, T, Alloc>::flat_unsigned_hashmap(
		size_t reserve_count) {
	reserve(reserve_count);
}

template <class Key, class T, class Alloc>
flat_unsigned_hashmap<Key, T, Alloc>::flat_unsigned_hashmap(
		size_t key_reserve_count, size_t value_reserve_count) {
	_lookup.reserve(key_reserve_count);
	_reverse_lookup.reserve(value_reserve_count);
	_values.reserve(value_reserve_count);
}

template <class Key, class T, class Alloc>
flat_unsigned_hashmap<Key, T, Alloc>::flat_unsigned_hashmap(
		const std::initializer_list<std::pair<key_type, value_type>>& init) {
	// TODO : benchmark and potentially optimize
	for (const std::pair<key_type, value_type>& kv : init) {
		insert(kv.first, kv.second);
	}
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::iterator
flat_unsigned_hashmap<Key, T, Alloc>::begin() noexcept {
	return _values.begin();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::const_iterator
flat_unsigned_hashmap<Key, T, Alloc>::begin() const noexcept {
	return _values.begin();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::const_iterator
flat_unsigned_hashmap<Key, T, Alloc>::cbegin() const noexcept {
	return _values.cbegin();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::iterator
flat_unsigned_hashmap<Key, T, Alloc>::end() noexcept {
	return _values.end();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::const_iterator
flat_unsigned_hashmap<Key, T, Alloc>::end() const noexcept {
	return _values.end();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::const_iterator
flat_unsigned_hashmap<Key, T, Alloc>::cend() const noexcept {
	return _values.cend();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::const_key_iterator
flat_unsigned_hashmap<Key, T, Alloc>::key_begin() const noexcept {
	return _reverse_lookup.begin();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::const_key_iterator
flat_unsigned_hashmap<Key, T, Alloc>::key_end() const noexcept {
	return _reverse_lookup.end();
}

template <class Key, class T, class Alloc>
bool flat_unsigned_hashmap<Key, T, Alloc>::empty() const noexcept {
	return _values.empty();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::size_type
flat_unsigned_hashmap<Key, T, Alloc>::size() const noexcept {
	return _values.size();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::size_type
flat_unsigned_hashmap<Key, T, Alloc>::max_size() const noexcept {
	// -1 due to sentinel
	return idx_sentinel() - 1;
}

template <class Key, class T, class Alloc>
void flat_unsigned_hashmap<Key, T, Alloc>::reserve(size_type new_cap) {
	_lookup.reserve(new_cap);
	_reverse_lookup.reserve(new_cap);
	_values.reserve(new_cap);
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::size_type
flat_unsigned_hashmap<Key, T, Alloc>::capacity() const noexcept {
	return _values.capacity();
}

template <class Key, class T, class Alloc>
void flat_unsigned_hashmap<Key, T, Alloc>::shrink_to_fit() {
	_lookup.shrink_to_fit();
	_reverse_lookup.shrink_to_fit();
	_values.shrink_to_fit();
}

template <class Key, class T, class Alloc>
void flat_unsigned_hashmap<Key, T, Alloc>::clear() noexcept {
	_hash_max = 0;
	_lookup.clear();
	_reverse_lookup.clear();
	_values.clear();
}

template <class Key, class T, class Alloc>
std::pair<typename flat_unsigned_hashmap<Key, T, Alloc>::iterator, bool>
flat_unsigned_hashmap<Key, T, Alloc>::insert(
		key_type key, const value_type& value) {
	return minsert(key, value);
}

template <class Key, class T, class Alloc>
std::pair<typename flat_unsigned_hashmap<Key, T, Alloc>::iterator, bool>
flat_unsigned_hashmap<Key, T, Alloc>::insert(key_type key, value_type&& value) {
	return minsert(key, fea::maybe_move(value));
}

template <class Key, class T, class Alloc>
void flat_unsigned_hashmap<Key, T, Alloc>::insert(
		const std::initializer_list<std::pair<key_type, value_type>>& ilist) {
	// TODO : benchmark and potentially optimize
	for (const std::pair<key_type, value_type>& kv : ilist) {
		insert(kv.first, kv.second);
	}
}

template <class Key, class T, class Alloc>
std::pair<typename flat_unsigned_hashmap<Key, T, Alloc>::iterator, bool>
flat_unsigned_hashmap<Key, T, Alloc>::insert_or_assign(
		key_type key, const value_type& value) {
	return minsert(key, value, true);
}

template <class Key, class T, class Alloc>
std::pair<typename flat_unsigned_hashmap<Key, T, Alloc>::iterator, bool>
flat_unsigned_hashmap<Key, T, Alloc>::insert_or_assign(
		key_type key, value_type&& value) {
	return minsert(key, fea::maybe_move(value), true);
}

template <class Key, class T, class Alloc>
template <class... Args>
std::pair<typename flat_unsigned_hashmap<Key, T, Alloc>::iterator, bool>
flat_unsigned_hashmap<Key, T, Alloc>::emplace(key_type key, Args&&... args) {
	// Standard emplace behavior doesn't apply. Use try_emplace.
	return try_emplace(key, std::forward<Args>(args)...);
}

template <class Key, class T, class Alloc>
template <class... Args>
std::pair<typename flat_unsigned_hashmap<Key, T, Alloc>::iterator, bool>
flat_unsigned_hashmap<Key, T, Alloc>::try_emplace(
		key_type key, Args&&... args) {
	if (load_factor() >= max_load_factor()) {
		size_type new_size = hash_max() * 2;
		rehash(new_size);
	}

	auto lookup_it = find_first_slot_or_hole(key);
	if (lookup_it == _lookup.end()) {
		// Need to grow _lookup for trailing collisions.
		lookup_it = trailing_resize(_lookup);
	}

	if (lookup_it->idx != idx_sentinel()) {
		// Found valid key.
		return { _values.begin() + lookup_it->idx, false };
	}

	idx_type new_pos = idx_type(_values.size());
	_values.emplace_back(std::forward<Args>(args)...);
	_reverse_lookup.push_back(key);
	lookup_it->key = key;
	lookup_it->idx = new_pos;

	assert(_reverse_lookup.size() == _values.size());
	return { begin() + new_pos, true };
}

template <class Key, class T, class Alloc>
void flat_unsigned_hashmap<Key, T, Alloc>::erase(const_iterator pos) {
	size_t idx = std::distance(_values.cbegin(), pos);
	erase(_reverse_lookup[idx]);
}

template <class Key, class T, class Alloc>
void flat_unsigned_hashmap<Key, T, Alloc>::erase(
		const_iterator first, const_iterator last) {
	size_t first_idx = std::distance(_values.cbegin(), first);
	size_t last_idx = std::distance(_values.cbegin(), last);

	std::vector<key_type> to_erase;
	to_erase.reserve(last_idx - first_idx);
	for (auto it = _reverse_lookup.begin() + first_idx;
			it != _reverse_lookup.begin() + last_idx; ++it) {
		to_erase.push_back(*it);
	}

	for (key_type& k : to_erase) {
		erase(k);
	}
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::size_type
flat_unsigned_hashmap<Key, T, Alloc>::erase(key_type k) {
	auto lookup_it = find_first_slot_or_hole(k);
	if (lookup_it == _lookup.end()) {
		return 0;
	}

	if (lookup_it->idx == idx_sentinel()) {
		return 0;
	}

	auto e = fea::make_on_exit(
			[lookup_idx = std::distance(_lookup.begin(), lookup_it), this]() {
				repack_collisions(lookup_idx);
			});

	if (lookup_it->idx == _values.size() - 1) {
		// No need for swap, object is already at end.
		*lookup_it = {};
		_reverse_lookup.pop_back();
		_values.pop_back();
		assert(_values.size() == _reverse_lookup.size());

		return 1;
	}

	// todo : Better way than doing a key search? could be slow
	// if we store the index of the key lookup, rehash gets slower
	key_type last_key = _reverse_lookup.back();
	auto last_lookup_it = find_first_slot_or_hole(last_key);

	// set new pos on last element.
	last_lookup_it->idx = lookup_it->idx;

	// invalidate erased lookup
	*lookup_it = {};

	// "swap" the elements
	_values[last_lookup_it->idx] = fea::maybe_move(_values.back());
	_reverse_lookup[last_lookup_it->idx] = last_key;

	// delete last
	_values.pop_back();
	_reverse_lookup.pop_back();

	assert(_values.size() == _reverse_lookup.size());
	return 1;
}

template <class Key, class T, class Alloc>
void flat_unsigned_hashmap<Key, T, Alloc>::swap(
		flat_unsigned_hashmap& other) noexcept {
	std::swap(_max_load_factor, other._max_load_factor);
	std::swap(_hash_max, other._hash_max);
	_lookup.swap(other._lookup);
	_reverse_lookup.swap(other._reverse_lookup);
	_values.swap(other._values);
}

template <class Key, class T, class Alloc>
const typename flat_unsigned_hashmap<Key, T, Alloc>::value_type*
flat_unsigned_hashmap<Key, T, Alloc>::data() const noexcept {
	return _values.data();
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::value_type*
flat_unsigned_hashmap<Key, T, Alloc>::data() noexcept {
	return _values.data();
}

template <class Key, class T, class Alloc>
const typename flat_unsigned_hashmap<Key, T, Alloc>::key_type*
flat_unsigned_hashmap<Key, T, Alloc>::key_data() const noexcept {
	return _reverse_lookup.data();
}

template <class Key, class T, class Alloc>
const typename flat_unsigned_hashmap<Key, T, Alloc>::mapped_type&
flat_unsigned_hashmap<Key, T, Alloc>::at(key_type k) const {
	if (!contains(k)) {
		fea::maybe_throw<std::out_of_range>(
				__FUNCTION__, __LINE__, "value doesn't exist");
	}

	return at_unchecked(k);
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::mapped_type&
flat_unsigned_hashmap<Key, T, Alloc>::at(key_type k) {
	return const_cast<mapped_type&>(
			static_cast<const flat_unsigned_hashmap*>(this)->at(k));
}

template <class Key, class T, class Alloc>
const typename flat_unsigned_hashmap<Key, T, Alloc>::mapped_type&
flat_unsigned_hashmap<Key, T, Alloc>::at_unchecked(key_type k) const {
	const_iterator it = find(k);
	return *it;
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::mapped_type&
flat_unsigned_hashmap<Key, T, Alloc>::at_unchecked(key_type k) {
	return const_cast<mapped_type&>(
			static_cast<const flat_unsigned_hashmap*>(this)->at_unchecked(k));
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::mapped_type&
flat_unsigned_hashmap<Key, T, Alloc>::operator[](key_type k) {
	iterator it = find(k);
	if (it != end()) {
		return *it;
	}

	return *insert(k, {}).first;
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::size_type
flat_unsigned_hashmap<Key, T, Alloc>::count(key_type k) const {
	if (contains(k))
		return 1;

	return 0;
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::const_iterator
flat_unsigned_hashmap<Key, T, Alloc>::find(key_type k) const {
	auto lookup_it = find_first_slot_or_hole(k);
	if (lookup_it == _lookup.end()) {
		return end();
	}

	if (lookup_it->idx == idx_sentinel()) {
		return end();
	}

	assert(lookup_it->key == k);
	assert(lookup_it->idx < _values.size());
	assert(lookup_it->idx < _reverse_lookup.size());

	return begin() + lookup_it->idx;
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::iterator
flat_unsigned_hashmap<Key, T, Alloc>::find(key_type k) {
	auto const_it = static_cast<const flat_unsigned_hashmap*>(this)->find(k);

	// Convert to non-const iterator.
	return _values.erase(const_it, const_it);
}

template <class Key, class T, class Alloc>
bool flat_unsigned_hashmap<Key, T, Alloc>::contains(key_type k) const {
	return find(k) != end();
}

template <class Key, class T, class Alloc>
float flat_unsigned_hashmap<Key, T, Alloc>::load_factor() const noexcept {
	size_type h_max = hash_max();
	if (h_max == 0) {
		return 2.f; // dummy value to trigger growth, must be > 1.f
	}
	return _values.size() / float(h_max);
}

template <class Key, class T, class Alloc>
float flat_unsigned_hashmap<Key, T, Alloc>::max_load_factor() const noexcept {
	return _max_load_factor;
}

template <class Key, class T, class Alloc>
void flat_unsigned_hashmap<Key, T, Alloc>::max_load_factor(float ml) noexcept {
	_max_load_factor = ml;
}

template <class Key, class T, class Alloc>
void flat_unsigned_hashmap<Key, T, Alloc>::rehash(size_type count) {
	if (count < init_count()) {
		count = init_count();
	} else {
		count = detail::next_prime(count);
	}
	assert(detail::is_prime(count));

	std::vector<lookup_data, lookup_allocator_type> new_lookup(count * 2);

	for (const lookup_data& lookup : _lookup) {
		if (lookup.idx == idx_sentinel()) {
			continue;
		}

		// new lookup position
		size_type new_bucket_pos = key_to_index(lookup.key, count);
		auto it = find_first_hole(
				new_lookup.begin(), new_lookup.end(), new_bucket_pos);

		if (it == new_lookup.end()) {
			// Expand the lookup past the end.
			it = trailing_resize(new_lookup);
		}

		// creates new lookup, assigns the existing element pos
		it->key = lookup.key;
		it->idx = lookup.idx;
	}

	_lookup = std::move(new_lookup);
	_hash_max = count;
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::size_type
flat_unsigned_hashmap<Key, T, Alloc>::hash_max() const {
	assert(detail::is_prime(_hash_max) || _hash_max == 0);
	return _hash_max;
}

template <class Key, class T, class Alloc>
typename flat_unsigned_hashmap<Key, T, Alloc>::size_type
flat_unsigned_hashmap<Key, T, Alloc>::key_to_index(key_type key) const {
	size_type ret = key_to_index(key, hash_max());
	assert(ret < _lookup.size());
	return ret;
}

template <class Key, class T, class Alloc>
constexpr typename flat_unsigned_hashmap<Key, T, Alloc>::size_type
flat_unsigned_hashmap<Key, T, Alloc>::key_to_index(
		key_type key, size_type h_max) {
	return (size_type(key) % h_max) * 2;
}

template <class Key, class T, class Alloc>
constexpr typename flat_unsigned_hashmap<Key, T, Alloc>::key_type
flat_unsigned_hashmap<Key, T, Alloc>::key_sentinel() noexcept {
	return (std::numeric_limits<key_type>::max)();
}

template <class Key, class T, class Alloc>
constexpr typename flat_unsigned_hashmap<Key, T, Alloc>::idx_type
flat_unsigned_hashmap<Key, T, Alloc>::idx_sentinel() noexcept {
	return (std::numeric_limits<idx_type>::max)();
}

template <class Key, class T, class Alloc>
constexpr typename flat_unsigned_hashmap<Key, T, Alloc>::size_type
flat_unsigned_hashmap<Key, T, Alloc>::init_count() noexcept {
	// return 14; // 7 * 2, hash_max == 7
	return 3;
}

template <class Key, class T, class Alloc>
template <class Iter, class Func>
auto flat_unsigned_hashmap<Key, T, Alloc>::find_slot(
		Iter start, Iter end, Func func) {
	// Test first.
	if (func(*start)) {
		return start;
	}

	// Now test "holes" (collisions) in-between keys.
	// Collisions are stored in odd slots only.
	for (auto it = start + 1; it < end;) {
		assert(std::distance(start, it) % 2 != 0);
		if (func(*it)) {
			return it;
		}

		if (it == end - 1) {
			break;
		}
		it += 2;
	}

	return end;
}

template <class Key, class T, class Alloc>
auto flat_unsigned_hashmap<Key, T, Alloc>::find_first_slot_or_hole(
		key_type key) const {
	if (hash_max() == 0) {
		return _lookup.end();
	}

	size_type search_pos = key_to_index(key);

	auto it = find_slot(_lookup.begin() + search_pos, _lookup.end(),
			[&](const lookup_data& search) {
				return search.key == key || search.idx == idx_sentinel();
			});

	return it;
}

template <class Key, class T, class Alloc>
auto flat_unsigned_hashmap<Key, T, Alloc>::find_first_slot_or_hole(
		key_type key) {
	auto const_it = static_cast<const flat_unsigned_hashmap*>(this)
							->find_first_slot_or_hole(key);

	if (const_it == _lookup.end()) {
		return _lookup.end();
	}

	// Convert to non-const iterator.
	return _lookup.erase(const_it, const_it);
}

template <class Key, class T, class Alloc>
template <class Iter>
auto flat_unsigned_hashmap<Key, T, Alloc>::find_first_hole(
		Iter beg, Iter end, size_type bucket_idx) {

	// find first free slot
	auto it = find_slot(beg + bucket_idx, end, [](const lookup_data& search) {
		return search.idx == idx_sentinel();
	});

	return it;
}

template <class Key, class T, class Alloc>
auto flat_unsigned_hashmap<Key, T, Alloc>::trailing_resize(
		std::vector<lookup_data, lookup_allocator_type>& lookup) {
	size_type idx = lookup.size();
	if ((idx & 1) == 0) {
		// Make sure idx is odd, to be a hole
		++idx;
	}
	assert(idx % 2 != 0);

	lookup.resize(size_type(idx * _lookup_trailing_amount));
	return lookup.begin() + idx;
}

template <class Key, class T, class Alloc>
void flat_unsigned_hashmap<Key, T, Alloc>::repack_collisions(
		size_type hole_idx) {
	assert(hole_idx < _lookup.size());
	assert(_lookup[hole_idx].idx == idx_sentinel());

	size_type swap_left_idx = hole_idx;
	size_type swap_right_idx = hole_idx + 1;

	// Only test odd slots, since that's where collisions are stored.
	if ((swap_right_idx & 1) == 0) {
		// This can happen if the hole was actually a collision.
		++swap_right_idx;
	}

	// Sort the collisions.
	// Do this until you find a hole. The container must guarantee
	// collisions are packed in a first serve manner.
	while (swap_right_idx < _lookup.size()) {
		assert(swap_right_idx % 2 != 0); // only odds

		if (_lookup[swap_right_idx].idx == idx_sentinel()) {
			// We are done, have reached the end of this collision "group".
			return;
		}

		// This tests if the right candidate collisions index is would be
		// "lost" when swapped with left hole.
		// Since the map stores and searches for collisions after the key,
		// this would break searches.
		size_type candidate_idx = key_to_index(_lookup[swap_right_idx].key);
		if (candidate_idx > swap_left_idx) {
			// Continue searching for swappable collisions.
			swap_right_idx += 2;
			continue;
		}

		lookup_data& right = _lookup[swap_right_idx];
		_lookup[swap_left_idx] = right;
		right = {}; // Invalidate in case it is the last.

		swap_left_idx = swap_right_idx;
		swap_right_idx += 2;
	}

	// We fall through if erasing the last valid element.
}

template <class Key, class T, class Alloc>
template <class M>
std::pair<typename flat_unsigned_hashmap<Key, T, Alloc>::iterator, bool>
flat_unsigned_hashmap<Key, T, Alloc>::minsert(
		key_type key, M&& value, bool assign_found) {
	if (load_factor() >= max_load_factor()) {
		size_type new_size = hash_max() * 2;
		rehash(new_size);
	}

	auto lookup_it = find_first_slot_or_hole(key);
	if (lookup_it == _lookup.end()) {
		lookup_it = trailing_resize(_lookup);
	}

	if (lookup_it->idx != idx_sentinel()) {
		// Found valid key.
		auto data_it = _values.begin() + lookup_it->idx;
		if (assign_found) {
			*data_it = std::forward<M>(value);
		}
		return { data_it, false };
	}

	idx_type new_pos = idx_type(_values.size());
	_values.push_back(std::forward<M>(value));
	_reverse_lookup.push_back(key);
	lookup_it->key = key;
	lookup_it->idx = new_pos;

	assert(_reverse_lookup.size() == _values.size());
	assert(_values.size() < idx_sentinel()
			&& "container has reached max capacity");
	return { begin() + new_pos, true };
}


template <class Key, class T, class Alloc>
inline bool operator==(const flat_unsigned_hashmap<Key, T, Alloc>& lhs,
		const flat_unsigned_hashmap<Key, T, Alloc>& rhs) {
	if (lhs.size() != rhs.size())
		return false;

	for (size_t i = 0; i < lhs.size(); ++i) {
		Key k = lhs._reverse_lookup[i];
		auto it = rhs.find(k);
		if (it == rhs.end()) {
			return false;
		}

		if (*it != lhs._values[i]) {
			return false;
		}
	}

	return true;
}

template <class Key, class T, class Alloc>
inline bool operator!=(const flat_unsigned_hashmap<Key, T, Alloc>& lhs,
		const flat_unsigned_hashmap<Key, T, Alloc>& rhs) {
	return !operator==(lhs, rhs);
}
} // namespace fea