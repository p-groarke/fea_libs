namespace fea {
namespace detail {
template <class T, size_t BucketSize>
struct dl_bucket {
	using size_type = std::size_t;
	using aligned_storage_t = fea::aligned_storage_t<sizeof(T), alignof(T)>;

	dl_bucket() = default;

	const T* begin() const noexcept {
		return reinterpret_cast<const T*>(data.data());
	}
	T* begin() noexcept {
		return const_cast<T*>(std::as_const(*this).begin());
	}

	const T* end() const noexcept {
		assert(size <= BucketSize);
		return reinterpret_cast<const T*>(data.data() + size);
	}
	T* end() noexcept {
		return const_cast<T*>(std::as_const(*this).end());
	}

	// Our raw data.
	std::array<aligned_storage_t, BucketSize> data;
	size_type size = size_type(0);
	std::unique_ptr<dl_bucket> next = nullptr;
	fea::back_ptr<dl_bucket> prev = nullptr;
};

template <class MyList>
struct dl_const_iter {
	// Typedefs
	using value_type = typename MyList::value_type;
	using difference_type = typename MyList::difference_type;
	using size_type = typename MyList::size_type;
	using pointer = const value_type*;
	using reference = const value_type&;
	using iterator_category = std::bidirectional_iterator_tag;
	using bucket_type = typename MyList::bucket;
	static constexpr size_type bucket_size = MyList::bucket_size;

	// Internals.

	// Ctors
	constexpr dl_const_iter() noexcept = default;
	constexpr dl_const_iter(const bucket_type* bucket, size_type idx,
			const bucket_type* last_bucket) noexcept
			: _bucket(bucket)
			, _last_bucket(last_bucket)
			, _idx(idx) {
	}

	~dl_const_iter() noexcept = default;
	constexpr dl_const_iter(const dl_const_iter&) noexcept = default;
	constexpr dl_const_iter(dl_const_iter&&) noexcept = default;
	constexpr dl_const_iter& operator=(const dl_const_iter&) noexcept = default;
	constexpr dl_const_iter& operator=(dl_const_iter&&) noexcept = default;

	[[nodiscard]]
	constexpr pointer operator->() const noexcept {
		assert(_bucket != nullptr);
		assert(_idx < bucket_size);
		return _bucket->begin() + _idx;
	}

	[[nodiscard]]
	constexpr reference operator*() const noexcept {
		return *this->operator->();
	}

	// Pre-fix ++operator.
	constexpr dl_const_iter& operator++() noexcept {
		assert(_bucket != nullptr);
		++_idx;
		if (_idx >= bucket_size && _bucket != _last_bucket) {
			_idx = size_type(0);
			_bucket = _bucket->next.get();
		}
		return *this;
	}

	// Post-fix operator++.
	constexpr dl_const_iter operator++(int) noexcept {
		dl_const_iter tmp = *this;
		++*this;
		return tmp;
	}

	// Pre-fix --operator.
	constexpr dl_const_iter& operator--() noexcept {
		assert(_bucket != nullptr);
		if (_idx == size_type(0)) {
			assert(_bucket->prev.get() != nullptr);
			_bucket = _bucket->prev.get();
			_idx = MyList::bucket_size;
		}
		--_idx;
		return *this;
	}

	// Post-fix operator--.
	constexpr dl_const_iter operator--(int) noexcept {
		dl_const_iter tmp = *this;
		--*this;
		return tmp;
	}

	// Comparison.
	[[nodiscard]]
	bool operator==(const dl_const_iter& rhs) const noexcept {
		return _bucket == rhs._bucket && _idx == rhs._idx;
	}

	// Comparison.
	[[nodiscard]]
	bool operator!=(const dl_const_iter& rhs) const noexcept {
		return !(*this == rhs);
	}

	//// Comparison.
	//[[nodiscard]]
	// bool
	// operator<(const fd_const_iter& rhs) const noexcept {
	//	if (_bucket == nullptr) {
	//		return false
	//	}
	//	return (_bucket == nullptr) || (_bucket == rhs._bucket && idx <
	// rhs._idx)
	//		|| _bucket < rhs._bucket;
	//}

	//// Comparison.
	//[[nodiscard]]
	// bool
	// operator>(const fd_const_iter& rhs) const noexcept {
	//	return rhs < *this;
	//}

	//// Comparison.
	//[[nodiscard]]
	// bool
	// operator<=(const fd_const_iter& rhs) const noexcept {
	//	return !(rhs < *this);
	//}

	//// Comparison.
	//[[nodiscard]]
	// bool
	// operator>=(const fd_const_iter& rhs) const noexcept {
	//	return !(*this < rhs);
	//}

	const bucket_type* _bucket = nullptr;
	const bucket_type* _last_bucket = nullptr;
	size_type _idx = 0u;
};

template <class MyList>
struct dl_iter : dl_const_iter<MyList> {
	using base_t = dl_const_iter<MyList>;

	// Typedefs
	using value_type = typename base_t::value_type;
	using difference_type = typename base_t::difference_type;
	using size_type = typename base_t::size_type;
	using pointer = value_type*;
	using reference = value_type&;
	using iterator_category = typename base_t::iterator_category;
	using bucket_type = typename base_t::bucket_type;

	using base_t::base_t;

	[[nodiscard]]
	constexpr reference operator*() const noexcept {
		return const_cast<reference>(base_t::operator*());
	}

	[[nodiscard]]
	constexpr pointer operator->() const noexcept {
		assert(base_t::_ptr != nullptr);
		return &base_t::_ptr->value;
	}

	// Pre-fix ++operator.
	constexpr dl_iter& operator++() noexcept {
		base_t::operator++();
		return *this;
	}

	// Post-fix operator++.
	constexpr dl_iter operator++(int) noexcept {
		dl_const_iter tmp = *this;
		base_t::operator++();
		return tmp;
	}

	// Pre-fix --operator.
	constexpr dl_iter& operator--() noexcept {
		base_t::operator--();
		return *this;
	}

	// Post-fix operator--.
	constexpr dl_iter operator--(int) noexcept {
		dl_const_iter tmp = *this;
		base_t::operator--();
		return tmp;
	}
};
} // namespace detail


template <class T, size_t BucketSize /*= 32*/>
deque_list<T, BucketSize>::deque_list() noexcept
		: _first_bucket()
		, _size(0)
		, _last_bucket(&_first_bucket) {
}

template <class T, size_t BucketSize /*= 32*/>
deque_list<T, BucketSize>::deque_list(const deque_list& other) {
	this->operator=(other);
}

template <class T, size_t BucketSize /*= 32*/>
deque_list<T, BucketSize>::deque_list(deque_list&& other) {
	this->operator=(std::move(other));
}

template <class T, size_t BucketSize /*= 32*/>
auto deque_list<T, BucketSize>::operator=(const deque_list& other)
		-> deque_list& {
	if (this == &other) {
		return *this;
	}

	_last_bucket = &_first_bucket;
	{
		bucket* my_b = &_first_bucket;
		const bucket* other_b = &other._first_bucket;
		while (other_b != nullptr && other_b->size > size_type(0)) {
			_last_bucket = my_b;

			std::uninitialized_copy(
					other_b->begin(), other_b->end(), my_b->begin());
			my_b->size = other_b->size;

			if (other_b->next && other_b->next->size > size_type(0)) {
				my_b->next = std::make_unique<bucket>();
				my_b->next->prev = my_b;
			}

			my_b = my_b->next.get();
			other_b = other_b->next.get();
		}
	}

	_size = other._size;
	return *this;
}

template <class T, size_t BucketSize /*= 32*/>
auto deque_list<T, BucketSize>::operator=(deque_list&& other) -> deque_list& {
	if (this == &other) {
		return *this;
	}

	_last_bucket = &_first_bucket;
	{
		bucket* my_b = &_first_bucket;
		bucket* other_b = &other._first_bucket;
		while (other_b != nullptr && other_b->size > size_type(0)) {
			_last_bucket = my_b;

			std::uninitialized_move(
					other_b->begin(), other_b->end(), my_b->begin());
			my_b->size = other_b->size;

			if (other_b->next && other_b->next->size > size_type(0)) {
				my_b->next = std::make_unique<bucket>();
				my_b->next->prev = my_b;
			}

			my_b = my_b->next.get();
			other_b->size = size_type(0);
			other_b = other_b->next.get();
		}
	}

	size_type msize = other._size; // threading, always update our size last.
	other._size = size_type(0);
	other._last_bucket = &other._first_bucket;
	_size = msize;
	return *this;
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::front() const -> const_reference {
	assert(!empty());
	assert(_first_bucket.size != size_type(0));
	return *_first_bucket.begin();
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::front() -> reference {
	return const_cast<reference>(std::as_const(*this).front());
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::back() const -> const_reference {
	assert(!empty());
	assert(_last_bucket->size != size_type(0));
	return *(_last_bucket->begin() + (_last_bucket->size - 1));
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::back() -> reference {
	return const_cast<reference>(std::as_const(*this).back());
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::begin() const noexcept -> const_iterator {
	assert(_last_bucket);
	return const_iterator{ &_first_bucket, size_type(0), _last_bucket.get() };
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::cbegin() const noexcept -> const_iterator {
	return begin();
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::begin() noexcept -> iterator {
	assert(_last_bucket);
	return iterator{ &_first_bucket, size_type(0), _last_bucket.get() };
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::end() const noexcept -> const_iterator {
	assert(_last_bucket);
	return const_iterator{ _last_bucket.get(), _last_bucket->size,
		_last_bucket.get() };
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::cend() const noexcept -> const_iterator {
	return end();
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::end() noexcept -> iterator {
	assert(_last_bucket);
	return iterator{ _last_bucket.get(), _last_bucket->size,
		_last_bucket.get() };
}

template <class T, size_t BucketSize>
bool deque_list<T, BucketSize>::empty() const noexcept {
	assert(_last_bucket);
	return _size == size_type(0);
}

template <class T, size_t BucketSize>
auto fea::deque_list<T, BucketSize>::size() const noexcept -> size_type {
	assert(_last_bucket);
	return _size;
}

// template <class T, size_t BucketSize>
// auto forward_deque<T, BucketSize>::max_size() const noexcept -> size_type {
//	return _data.max_size();
// }

template <class T, size_t BucketSize /*= 32*/>
void fea::deque_list<T, BucketSize>::shrink_to_fit() {
	assert(_last_bucket);
	if (_last_bucket->next) {
		assert(_last_bucket->next->size == 0u);
		_last_bucket->next.reset(nullptr);
	}
}

template <class T, size_t BucketSize>
void deque_list<T, BucketSize>::clear() {
	assert(_last_bucket);
	{
		bucket* b = &_first_bucket;
		while (b != nullptr) {
			fea::destroy(b->begin(), b->end());
			b->size = size_type(0);
			b = b->next.get();
		}
	}

	_last_bucket = &_first_bucket;
	_size = 0;
	assert(empty());
}

template <class T, size_t BucketSize>
void deque_list<T, BucketSize>::push_back(const value_type& value) {
	maybe_grow();
	assert(_last_bucket->size < bucket_size);

	std::uninitialized_copy_n(
			&value, 1u, _last_bucket->begin() + _last_bucket->size++);
	++_size;
}

template <class T, size_t BucketSize>
void deque_list<T, BucketSize>::push_back(value_type&& value) {
	maybe_grow();
	assert(_last_bucket->size < bucket_size);

	std::uninitialized_move_n(
			&value, 1u, _last_bucket->begin() + _last_bucket->size++);
	++_size;
}

template <class T, size_t BucketSize>
void deque_list<T, BucketSize>::pop_back() {
	assert(!empty());
	assert(_last_bucket->size > size_type(0)
			&& _last_bucket->size <= bucket_size);

	value_type* last_ptr = &back();
	fea::destroy_at(last_ptr);

	--_last_bucket->size;
	if (_last_bucket->size == size_type(0) && _last_bucket->prev) {
		_last_bucket = _last_bucket->prev.get();
	}
	--_size;
}

template <class T, size_t BucketSize /*= 32*/>
void fea::deque_list<T, BucketSize>::maybe_grow() {
	assert(_last_bucket);
	if (_last_bucket->size == bucket_size) {
		if (_last_bucket->next == nullptr) {
			_last_bucket->next = std::make_unique<bucket>();
			_last_bucket->next->prev = _last_bucket.get();
		}

		_last_bucket = _last_bucket->next.get();
	}
}

} // namespace fea
